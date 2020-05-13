/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2014-2016 OpenFOAM Foundation
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
#include "interpolationTable.H"

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
            option,
            mushyZoneSource,
            dictionary
        );
    }
}

const Foam::NamedEnum<Foam::fv::mushyZoneSource::thermoMode, 2>
    Foam::fv::mushyZoneSource::thermoModeTypeNames_;


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::fv::mushyZoneSource::Cp() const
{
    switch (mode_)
    {
        case mdThermo:
        {
            const basicThermo& thermo =
                mesh_.lookupObject<basicThermo>(basicThermo::dictName);

            return thermo.Cp();
            break;
        }
        case mdLookup:
        {
            if (CpName_ == "CpRef")
            {
                scalar CpRef = readScalar(coeffs_.lookup("CpRef"));

                return volScalarField::New
                (
                    name_ + ":Cp",
                    mesh_,
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
                return mesh_.lookupObject<volScalarField>(CpName_);
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
    if (mesh_.foundObject<uniformDimensionedVectorField>("g"))
    {
        const uniformDimensionedVectorField& value =
            mesh_.lookupObject<uniformDimensionedVectorField>("g");
        return value.value();
    }
    else
    {
        return coeffs_.lookup("g");
    }
}


void Foam::fv::mushyZoneSource::update()
{
    if (curTimeIndex_ == mesh_.time().timeIndex())
    {
        return;
    }

    if (debug)
    {
        Info<< type() << ": " << name_ << " - updating phase indicator" << endl;
    }

    // update old time alpha1 field
    alpha1_.oldTime();

    const volScalarField& T = mesh_.lookupObject<volScalarField>("T");
    const volScalarField Cp(this->Cp());

    interpolationTable<scalar> fraction_curve("constant/fL");
    interpolationTable<scalar> tstar_curve("constant/tStar");

    forAll(cells_, i)
    {
        label celli = cells_[i];

        scalar Tc = T[celli];
        scalar Cpc = Cp[celli];
        scalar alpha1New;
        scalar Tstar;

        scalar alpha1_star = alpha1_[celli];
        Tstar = tstar_curve(max(0, min(alpha1_star, 1)));
        alpha1New = alpha1_star + relax_*Cpc*(Tc - Tstar)/L_;
        
        alpha1_[celli] = max(0, min(alpha1New, 1));
        // deltaT_[i] = Tc - Tstar;
    }

    alpha1_.correctBoundaryConditions();

    curTimeIndex_ = mesh_.time().timeIndex();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::mushyZoneSource::mushyZoneSource
(
    const word& sourceName,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    cellSetOption(sourceName, modelType, dict, mesh),
    Tliquidus_(readScalar(coeffs_.lookup("Tliquidus"))),
    Tsolidus_(readScalar(coeffs_.lookup("Tsolidus"))),
    L_(readScalar(coeffs_.lookup("L"))),
    g_env_(coeffs_.lookupOrDefault("g_env", 0.7)),
    relax_(coeffs_.lookupOrDefault("relax", 0.9)),
    // castingVelocity_(coeffs_.lookup("castingVelocity")),
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
        dimensionedVector("castingVelocity", dimVelocity, coeffs_.lookup("castingVelocity"))
    ),
    mode_(thermoModeTypeNames_.read(coeffs_.lookup("thermoMode"))),
    rhoRef_(readScalar(coeffs_.lookup("rhoRef"))),
    TName_(coeffs_.lookupOrDefault<word>("T", "T")),
    CpName_(coeffs_.lookupOrDefault<word>("Cp", "Cp")),
    UName_(coeffs_.lookupOrDefault<word>("U", "U")),
    phiName_(coeffs_.lookupOrDefault<word>("phi", "phi")),
    Cu_(coeffs_.lookupOrDefault<scalar>("Cu", 100000)),
    q_(coeffs_.lookupOrDefault("q", 0.001)),
    beta_(readScalar(coeffs_.lookup("beta"))),
    alpha1_
    (
        IOobject
        (
            name_ + "_alpha1",
            mesh.time().timeName(),
            mesh,
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("alpha1", dimless, 0.0),
        zeroGradientFvPatchScalarField::typeName
    ),
    curTimeIndex_(-1),
    deltaT_(cells_.size(), 0.0)
{
    fieldNames_.setSize(2);
    fieldNames_[0] = UName_;

    switch (mode_)
    {
        case mdThermo:
        {
            const basicThermo& thermo =
                mesh_.lookupObject<basicThermo>(basicThermo::dictName);

            fieldNames_[1] = thermo.he().name();
            break;
        }
        case mdLookup:
        {
            fieldNames_[1] = TName_;
            break;
        }
        default:
        {
            FatalErrorInFunction
                << "Unhandled thermo mode: " << thermoModeTypeNames_[mode_]
                << abort(FatalError);
        }
    }

    applied_.setSize(2, false);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::fv::mushyZoneSource::addSup
(
    fvMatrix<scalar>& eqn,
    const label fieldi
)
{
    apply(geometricOneField(), eqn);
}


void Foam::fv::mushyZoneSource::addSup
(
    const volScalarField& rho,
    fvMatrix<scalar>& eqn,
    const label fieldi
)
{
    apply(rho, eqn);
}


void Foam::fv::mushyZoneSource::addSup
(
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    if (debug)
    {
        Info<< type() << ": applying source to " << eqn.psi().name() << endl;
    }

    update();

    vector g = this->g();

    scalarField& Sp = eqn.diag();
    vectorField& Su = eqn.source();
    const scalarField& V = mesh_.V();

    const volScalarField& T = mesh_.lookupObject<volScalarField>("T");

    forAll(cells_, i)
    {
        label celli = cells_[i];

        scalar Vc = V[celli];
        scalar alpha1c = alpha1_[celli];

        if (alpha1c < g_env_)
        {
            scalar S = -Cu_*sqr(1.0 - alpha1c)/(pow3(alpha1c) + q_);
            vector Sc = S*castingVelocity_[celli];
            Sp[celli] += Vc*S;
            Su[celli] += Vc*Sc;
        }

        // vector Sb = rhoRef_*g*beta_*deltaT_[i];
        if (T[celli] > Tsolidus_)
        {
            vector Sb = rhoRef_*g*beta_*(T[celli] - Tsolidus_);
            Su[celli] += Vc*Sb;
        }
    }
}


void Foam::fv::mushyZoneSource::addSup
(
    const volScalarField& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    // Momentum source uses a Boussinesq approximation - redirect
    addSup(eqn, fieldi);
}


// ************************************************************************* //
