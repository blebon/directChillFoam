/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2014-2021 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "mushyZoneSource.H"
#include "fvMatrices.H"
#include "basicThermo.H"
#include "uniformDimensionedFields.H"
#include "zeroGradientFvPatchFields.H"
#include "extrapolatedCalculatedFvPatchFields.H"
#include "addToRunTimeSelectionTable.H"
#include "geometricOneField.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
    template<>
    const char* NamedEnum
    <
        fv::mushyZoneSource::thermoMode,
        2
    >::names[] =
    {
        "thermo",
        "lookup"
    };

    namespace fv
    {
        defineTypeNameAndDebug(mushyZoneSource, 0);

        addToRunTimeSelectionTable
        (
            fvModel,
            mushyZoneSource,
            dictionary
        );
    }
}

const Foam::NamedEnum<Foam::fv::mushyZoneSource::thermoMode, 2>
    Foam::fv::mushyZoneSource::thermoModeTypeNames_;


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::fv::mushyZoneSource::readCoeffs()
{
    Tsol_ = coeffs().lookup<scalar>("Tsol");
    Tliq_ = coeffs().lookupOrDefault<scalar>("Tliq", Tsol_);
    g_env_ = coeffs().lookupOrDefault<scalar>("g_env", 0.7);
    L_ = coeffs().lookup<scalar>("L");

    relax_ = coeffs().lookupOrDefault("relax", 0.9);

    // castingVelocity_ = coeffs().lookup<vector>("castingVelocity");
    tStar_ = Function1<scalar>::New("tStar", coeffs());

    mode_ = thermoModeTypeNames_.read(coeffs().lookup("thermoMode"));

    rhoRef_ = coeffs().lookup<scalar>("rhoRef");
    TName_ = coeffs().lookupOrDefault<word>("T", "T");
    CpName_ = coeffs().lookupOrDefault<word>("Cp", "Cp");
    UName_ = coeffs().lookupOrDefault<word>("U", "U");
    phiName_ = coeffs().lookupOrDefault<word>("phi", "phi");

    Cu_ = coeffs().lookupOrDefault<scalar>("Cu", 100000);
    q_ = coeffs().lookupOrDefault("q", 0.001);

    beta_ = coeffs().lookup<scalar>("beta");
}


Foam::tmp<Foam::volScalarField>
Foam::fv::mushyZoneSource::Cp() const
{
    switch (mode_)
    {
        case thermoMode::thermo:
        {
            const basicThermo& thermo =
                mesh().lookupObject<basicThermo>(basicThermo::dictName);

            return thermo.Cp();
            break;
        }
        case thermoMode::lookup:
        {
            if (CpName_ == "CpRef")
            {
                scalar CpRef = coeffs().lookup<scalar>("CpRef");

                return volScalarField::New
                (
                    name() + ":Cp",
                    mesh(),
                    dimensionedScalar
                    (
                        dimEnergy/dimMass/dimTemperature,
                        CpRef
                    ),
                    extrapolatedCalculatedFvPatchScalarField::typeName
                );
            }
            else
            {
                return mesh().lookupObject<volScalarField>(CpName_);
            }

            break;
        }
        default:
        {
            FatalErrorInFunction
                << "Unhandled thermo mode: " << thermoModeTypeNames_[mode_]
                << abort(FatalError);
        }
    }

    return tmp<volScalarField>(nullptr);
}


Foam::vector Foam::fv::mushyZoneSource::g() const
{
    if (mesh().foundObject<uniformDimensionedVectorField>("g"))
    {
        const uniformDimensionedVectorField& value =
            mesh().lookupObject<uniformDimensionedVectorField>("g");
        return value.value();
    }
    else
    {
        return coeffs().lookup("g");
    }
}


void Foam::fv::mushyZoneSource::update
(
    const volScalarField& Cp
) const
{
    if (curTimeIndex_ == mesh().time().timeIndex())
    {
        return;
    }

    if (debug)
    {
        Info<< type() << ": " << name()
            << " - updating phase indicator" << endl;
    }

    // update old time alpha1 field
    alpha1_.oldTime();

    const volScalarField& T = mesh().lookupObject<volScalarField>(TName_);

    const labelList& cells = set_.cells();

    forAll(cells, i)
    {
        const label celli = cells[i];

        const scalar Tc = T[celli];
        const scalar Cpc = Cp[celli];
        scalar alpha1New;
        scalar Tstar;

        scalar alpha1_star = alpha1_[celli];
        Tstar = tStar_->value(max(0, min(alpha1_star, 1)));
        alpha1New = alpha1_star + relax_*Cpc*(Tc - Tstar)/L_;
        
        alpha1_[celli] = max(0, min(alpha1New, 1));
    }

    alpha1_.correctBoundaryConditions();

    curTimeIndex_ = mesh().time().timeIndex();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::mushyZoneSource::mushyZoneSource
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    fvModel(name, modelType, dict, mesh),
    set_(coeffs(), mesh),
    Tsol_(NaN),
    Tliq_(NaN),
    L_(NaN),
    g_env_(NaN),
    relax_(NaN),
    castingVelocity_
    (
        IOobject
        (
            "castingVelocity_",
            mesh.time().timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimensionedVector("castingVelocity", dimVelocity, coeffs().lookup<vector>("castingVelocity"))
    ),
    tStar_(),
    mode_(thermoMode::thermo),
    rhoRef_(NaN),
    TName_(word::null),
    CpName_(word::null),
    UName_(word::null),
    phiName_(word::null),
    Cu_(NaN),
    q_(NaN),
    beta_(NaN),
    alpha1_
    (
        IOobject
        (
            this->name() + "_alpha1",
            mesh.time().timeName(),
            mesh,
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar(dimless, 0),
        zeroGradientFvPatchScalarField::typeName
    ),
    curTimeIndex_(-1)
{
    readCoeffs();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::wordList Foam::fv::mushyZoneSource::addSupFields() const
{
    switch (mode_)
    {
        case thermoMode::thermo:
        {
            const basicThermo& thermo =
                mesh().lookupObject<basicThermo>(basicThermo::dictName);

            return wordList({UName_, thermo.he().name()});
        }
        case thermoMode::lookup:
        {
            return wordList({UName_, TName_});
        }
    }

    return wordList::null();
}


void Foam::fv::mushyZoneSource::addSup
(
    fvMatrix<scalar>& eqn,
    const word& fieldName
) const
{
    apply(geometricOneField(), eqn);
}


void Foam::fv::mushyZoneSource::addSup
(
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const word& fieldName
) const
{
    apply(rho, eqn);
}


void Foam::fv::mushyZoneSource::addSup
(
    fvMatrix<vector>& eqn,
    const word& fieldName
) const
{
    if (debug)
    {
        Info<< type() << ": applying source to " << eqn.psi().name() << endl;
    }

    const volScalarField Cp(this->Cp());

    update(Cp);

    vector g = this->g();

    scalarField& Sp = eqn.diag();
    vectorField& Su = eqn.source();
    const scalarField& V = mesh().V();

    const volScalarField& T = mesh().lookupObject<volScalarField>("T");

    const labelList& cells = set_.cells();

    forAll(cells, i)
    {
        const label celli = cells[i];

        const scalar Vc = V[celli];
        const scalar alpha1c = alpha1_[celli];

        if (alpha1c < g_env_)
        {
            const scalar S = -Cu_*sqr(1.0 - alpha1c)/(pow3(alpha1c) + q_);
            const vector Sc = S*castingVelocity_[celli];
            Sp[celli] += Vc*S;
            Su[celli] += Vc*Sc;
        }
        
        if (T[celli] > Tsol_)
        {
            const vector Sb = rhoRef_*g*beta_*(T[celli] - Tsol_);
            Su[celli] += Vc*Sb;
        }
    }
}


void Foam::fv::mushyZoneSource::addSup
(
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const word& fieldName
) const
{
    // Momentum source uses a Boussinesq approximation - redirect
    addSup(eqn, fieldName);
}


void Foam::fv::mushyZoneSource::updateMesh(const mapPolyMesh& mpm)
{
    set_.updateMesh(mpm);
}


bool Foam::fv::mushyZoneSource::read(const dictionary& dict)
{
    if (fvModel::read(dict))
    {
        set_.read(coeffs());
        readCoeffs();
        return true;
    }
    else
    {
        return false;
    }

    return false;
}


// ************************************************************************* //
