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
Class
    Foam::multicomponentAlloy
Description
    Multicomponent alloy class for solving species equation.
SourceFiles
    multicomponentAlloy.C
\*---------------------------------------------------------------------------*/

#ifndef multicomponentAlloy_H
#define multicomponentAlloy_H

#include "IOdictionary.H"
#include "soluteModel.H"
#include "PtrDictionary.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "fvModels.H"
#include "fvConstraints.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class multicomponentAlloy Declaration
\*---------------------------------------------------------------------------*/

class multicomponentAlloy
:
    public IOdictionary
{

private:

    // Private data

        //- Name of alloy
        word name_;

        //- Dictionary of solutes
        PtrDictionary<soluteModel> solutes_;

        const fvMesh& mesh_;
        const volVectorField& U_;
        const volScalarField& rho_;
        const dimensionedScalar& rho2_;
        const surfaceScalarField& phi_;
        const fvModels& fvModels_;
        const fvConstraints& fvConstraints_;
	const volScalarField& alpha_;

public:

    // Constructors

        //- Construct from components
        multicomponentAlloy
        (
            const word& alloyName,
            const volVectorField& U,
            const volScalarField& rho,
            const dimensionedScalar& rho2,
            const surfaceScalarField& phi,
            const fvModels& fvModels,
            const fvConstraints& fvConstraints
        );


    //- Destructor
    virtual ~multicomponentAlloy()
    {}


    // Member Functions

        const word& name() const
        {
            return name_;
        }

        const word& keyword() const
        {
            return name();
        }

        //- Return the solutes
        const PtrDictionary<soluteModel>& solutes() const
        {
            return solutes_;
        }

        //- Return the solutes
        PtrDictionary<soluteModel>& solutes()
        {
            return solutes_;
        }

        //- Solve each solute equation
        void solve(const volVectorField& Us_,
                   const fvModels& fvModels_,
                   const fvConstraints& fvConstraints_);

        //- Read base transportProperties dictionary
        bool read();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
