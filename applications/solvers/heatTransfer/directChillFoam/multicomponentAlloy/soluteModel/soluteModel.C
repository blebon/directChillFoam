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

#include "soluteModel.H"
#include "fixedValueFvPatchFields.H"
#include "surfaceInterpolate.H"
#include "fvcFlux.H"

// * * * * * * * * * * * * * * * Static Member Data  * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::soluteModel::soluteModel
(
    const word& soluteName,
    const dictionary& soluteDict,
    const fvMesh& mesh
)
:
    volScalarField
    (
        IOobject
        (
            IOobject::groupName("C", soluteName),
            mesh.time().timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh
    ),
    name_(soluteName),
    soluteDict_(soluteDict),
    interpolate_(soluteDict.lookupOrDefault<word>("interpolate", "no")),
    D_l_
    (
        "D_l",
        dimArea/dimTime,
        soluteDict_
    ),
    kp_
    (
        "kp",
        dimless,
        soluteDict_
    ),
    C_l_
    (
        IOobject
        (
            IOobject::groupName("C_l", soluteName),
            mesh.time().timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("0", dimless, 0)
    ),
    C_Rel_
    (
        IOobject
        (
            IOobject::groupName("C_Rel", soluteName),
            mesh.time().timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("0", dimless, 0)
    ),
    C_s_
    (
        IOobject
        (
            IOobject::groupName("C_s", soluteName),
            mesh.time().timeName(),
            mesh
        ),
        mesh,
        dimensionedScalar("0", dimless, 0)
    ),
    C0_
    (
        "C0",
        dimless,
        soluteDict_
    ),
    beta_
    (
        "beta",
        dimless,
        soluteDict_
    ),
    alpha_(mesh.lookupObject<volScalarField>("alpha"))
{

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::soluteModel::~soluteModel()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::autoPtr<Foam::soluteModel> Foam::soluteModel::clone() const
{
    NotImplemented;
    return autoPtr<soluteModel>(nullptr);
}


void Foam::soluteModel::correct(const dictionary& soluteDict)
{
    soluteDict_ = soluteDict;
    const volScalarField& C_ = *this;

    if (interpolate_ == "yes") {
        autoPtr<Function1<scalar>> cl_curve(Function1<scalar>::New(groupName("C_l", this->name()), soluteDict_));
        forAll(C_l_, i)
        {
            C_l_[i] = cl_curve->value(alpha_[i]);
        }
    }
    else {
        C_l_ = C_/(1.0 + (1.0 - alpha_)*(this->kp() - 1.0));
    }

    // forAll(C_l_, i)
    // {
    //     C_l_[i] = Foam::min(C_l_[i], this->Ceut().value());
    // }
    
    C_Rel_ = C_l_ - C_;
    C_s_ = this->kp()*C_l_;
}


void Foam::soluteModel::solve(const surfaceScalarField& phiRel)
{
    NotImplemented;
}


bool Foam::soluteModel::read(const dictionary& soluteDict)
{
    soluteDict_ = soluteDict;

    soluteDict_.lookup("D_l") >> D_l_.value();
    soluteDict_.lookup("kp") >> kp_.value();
    soluteDict_.lookup("Ceut") >> Ceut_.value();

    return true;
}

// ************************************************************************* //
