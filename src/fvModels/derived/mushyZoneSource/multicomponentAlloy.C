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

#include "multicomponentAlloy.H"
#include "fvCFD.H"
#include "Time.H"

// * * * * * * * * * * * * * * * Static Member Data  * * * * * * * * * * * * //


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::multicomponentAlloy::multicomponentAlloy
(
    const word& alloyName,
    const volVectorField& U,
    const volScalarField& rho,
    const dimensionedScalar& rho2,
    const surfaceScalarField& phi,
    const fvModels& fvModels,
    const fvConstraints& fvConstraints
)
:
    IOdictionary
    (
        IOobject
        (
            "soluteProperties",
            U.time().constant(),
            U.db(),
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    ),

    name_(alloyName),

    solutes_(lookup("solutes"), soluteModel::iNew(U.mesh())),

    mesh_(U.mesh()),
    U_(U),
    rho_(rho),
    rho2_(rho2),
    phi_(phi),
    fvModels_(fvModels),
    fvConstraints_(fvConstraints),
    alpha_(U.db().lookupObject<volScalarField>("alpha"))
{
    
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::multicomponentAlloy::solve(const volVectorField& Us_,
                                      const fvModels& fvModels_,
                                      const fvConstraints& fvConstraints_ )
{
    PtrList<entry> soluteData(lookup("solutes"));
    label solutei = 0;

    forAllIter(PtrDictionary<soluteModel>, solutes_, iter)
    {       
        volScalarField& C = iter();
        surfaceScalarField phiRel = fvc::interpolate(rho_*(U_-Us_)) & mesh_.Sf();

        iter().correct(soluteData[solutei++].dict());

        C.correctBoundaryConditions();

        volScalarField D = rho_*alpha_*iter().D_l();
        volScalarField CRel = iter().C_Rel();

        fvScalarMatrix CEqn
        (
            fvm::ddt(rho_, C)
          + fvm::div(phi_, C)
          - fvm::laplacian(D, C)
        ==
            fvc::laplacian(D, CRel)
          - fvc::div(phiRel, CRel)
          + fvModels_.source(rho_, C)
        );

        CEqn.relax();
        fvConstraints_.constrain(CEqn);
        CEqn.solve();
        fvConstraints_.constrain(C);
    }
}


bool Foam::multicomponentAlloy::read()
{
    if (regIOobject::read())
    {
        bool readOK = true;

        PtrList<entry> soluteData(lookup("solutes"));
        label solutei = 0;

        forAllIter(PtrDictionary<soluteModel>, solutes_, iter)
        {
            readOK &= iter().read(soluteData[solutei++].dict());
        }

        return readOK;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
