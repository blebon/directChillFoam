/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::tableThermo

Description
    Table properties thermodynamics package
    templated into the EquationOfState.

SourceFiles
    tableThermoI.H
    tableThermo.C

\*---------------------------------------------------------------------------*/

#ifndef tableThermo_H
#define tableThermo_H

#include "interpolationTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

template<class EquationOfState> class tableThermo;

template<class EquationOfState>
Ostream& operator<<
(
    Ostream&,
    const tableThermo<EquationOfState>&
);


/*---------------------------------------------------------------------------*\
                           Class tableThermo Declaration
\*---------------------------------------------------------------------------*/

template<class EquationOfState>
class tableThermo
:
    public EquationOfState
{
    // Private data

        scalar Hf_;
    
    //- Specific heat table
    interpolationTable<scalar> CpTable_;

    //- Enthalpy table
    interpolationTable<scalar> HsTable_;

    // Private Member Functions

        //- Construct from components
        inline tableThermo
        (
            const EquationOfState& st,
            const scalar hf
        );


public:

    // Constructors

        //- Construct from dictionary
        tableThermo(const dictionary& dict);

        //- Construct as named copy
        inline tableThermo(const word&, const tableThermo&);

        //- Construct and return a clone
        inline autoPtr<tableThermo> clone() const;

        //- Selector from dictionary
        inline static autoPtr<tableThermo> New(const dictionary& dict);


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "table<" + EquationOfState::typeName() + '>';
        }

        //- Limit the temperature to be in the range Tlow_ to Thigh_
        inline scalar limit(const scalar T) const;


        // Fundamental properties

            //- Heat capacity at constant pressure [J/(kg K)]
            inline scalar Cp(const scalar p, const scalar T) const;

            //- Absolute Enthalpy [J/kg]
            inline scalar Ha(const scalar p, const scalar T) const;

            //- Sensible enthalpy [J/kg]
            inline scalar Hs(const scalar p, const scalar T) const;

            //- Chemical enthalpy [J/kg]
            inline scalar Hc() const;

            //- Entropy [J/(kg K)]
            inline scalar S(const scalar p, const scalar T) const;

            #include "HtoEthermo.H"

        // I-O

            //- Write to Ostream
            void write(Ostream& os) const;


    // IOstream Operators

        friend Ostream& operator<< <EquationOfState>
        (
            Ostream&,
            const tableThermo&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "tableThermoI.H"

#ifdef NoRepository
    #include "tableThermo.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
