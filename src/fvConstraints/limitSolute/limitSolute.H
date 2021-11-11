/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2012-2021 OpenFOAM Foundation
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
    Foam::fv::limitSolute

Description
    Limits the solute concentration to be between minimum and maximum
    values.

Usage
    Example usage:
    \verbatim
    limitS
    {
        type            limitSolute;

        selectionMode   all;

        solute          C.Cu; // solute field name

        min             0.0;
        max             1.0;
    }
    \endverbatim

SourceFiles
    limitSolute.C

\*---------------------------------------------------------------------------*/

#ifndef limitSolute_H
#define limitSolute_H

#include "fvConstraint.H"
#include "fvCellSet.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
                      Class limitSolute Declaration
\*---------------------------------------------------------------------------*/

class limitSolute
:
    public fvConstraint
{
    // Private data

        //- The set of cells the fvConstraint applies to
        fvCellSet set_;

        //- Minimum Solute limit [K]
        scalar Smin_;

        //- Maximum Solute limit [K]
        scalar Smax_;

        //- Name of solute field
        word SName_;


    // Private Member Functions

        //- Non-virtual read
        void readCoeffs();


public:

    //- Runtime type information
    TypeName("limitSolute");


    // Constructors

        //- Construct from components
        limitSolute
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );

        //- Disallow default bitwise copy construction
        limitSolute(const limitSolute&) = delete;


    //- Destructor
    virtual ~limitSolute()
    {}


    // Member Functions

        //- Return the list of fields constrained by the fvConstraint
        virtual wordList constrainedFields() const;

        //- Constrain the solute field
        virtual bool constrain(volScalarField& C) const;

        //- Update for mesh changes
        virtual void updateMesh(const mapPolyMesh&);

        //- Read dictionary
        virtual bool read(const dictionary& dict);


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const limitSolute&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
