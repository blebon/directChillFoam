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
    Foam::soluteModel
SourceFiles
    soluteModel.C
\*---------------------------------------------------------------------------*/

#ifndef soluteModel_H
#define soluteModel_H

#include "fvCFD.H"
#include "dictionary.H"
#include "dictionaryEntry.H"
#include "dimensionedScalar.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "Function1.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class soluteModel Declaration
\*---------------------------------------------------------------------------*/

class soluteModel
:
    public volScalarField
{
    // Private data

        //- Name of solute
        word name_;

        //- soluteProperties dictionary
        dictionary soluteDict_;

        //- Interpolate C_l or use kp
        word interpolate_;

        //- Liquid mass diffusion coefficient
        dimensionedScalar D_l_;

        //- Partition coefficient
        dimensionedScalar kp_;

        //- Liquid concentration field
        volScalarField C_l_;

        //- Relative concentration field
        volScalarField C_Rel_;

        //- Solid concentration field
        volScalarField C_s_;

        //- Initial concentration
        dimensionedScalar C0_;

        //- Eutectic concentration
        dimensionedScalar Ceut_;

        //- Solute expansion coefficient
        dimensionedScalar beta_;

        //- Liquid fraction field
        const volScalarField& alpha_;
public:

    // Constructors

        soluteModel
        (
            const word& soluteName,
            const dictionary& soluteDict,
            const fvMesh& mesh
        );

        //- Return clone
        autoPtr<soluteModel> clone() const;

        //- Return a pointer to a new phase created on freestore
        //  from Istream
        class iNew
        {
            const fvMesh& mesh_;

        public:

            iNew
            (
                const fvMesh& mesh
            )
            :
                mesh_(mesh)
            {}

            autoPtr<soluteModel> operator()(Istream& is) const
            {
                dictionaryEntry ent(dictionary::null, is);
                return autoPtr<soluteModel>
                (
                    new soluteModel(ent.keyword(), ent, mesh_)
                );
            }
        };


    //- Destructor
    virtual ~soluteModel();


    // Member Functions

        const word& name() const
        {
            return name_;
        }

        const word& keyword() const
        {
            return name();
        }

        const word& interpolate() const
        {
            return interpolate_;
        }

        const dimensionedScalar& D_l() const
        {
            return D_l_;
        }

        const dimensionedScalar& kp() const
        {
            return kp_;
        }

        const volScalarField& C_l() const
        {
            return C_l_;
        }

        volScalarField& C_l()
        {
            return C_l_;
        }

        const volScalarField& C_Rel() const
        {
            return C_Rel_;
        }

        volScalarField& C_Rel()
        {
            return C_Rel_;
        }

        const volScalarField& C_s() const
        {
            return C_s_;
        }

        volScalarField& C_s()
        {
            return C_s_;
        }

        const dimensionedScalar& C0() const
        {
            return C0_;
        }

        const dimensionedScalar& Ceut() const
        {
            return Ceut_;
        }

        const dimensionedScalar& beta() const
        {
            return beta_;
        }  

        //- Correct the solute properties
        void correct(const dictionary& soluteDict);

        //- Solve the solute equation
        void solve(const surfaceScalarField& phiRel);

        //-Inherit read from volScalarField
        using volScalarField::read;

        //- Read base transportProperties dictionary
        bool read(const dictionary& soluteDict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
