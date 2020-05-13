/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2012-2016 OpenFOAM Foundation
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
    Foam::table

Description
    table (rho = const) of state.

SourceFiles
    tableI.H
    table.C

\*---------------------------------------------------------------------------*/

#ifndef table_H
#define table_H

#include "autoPtr.H"
#include "interpolationTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

template<class Specie> class table;

template<class Specie>
inline table<Specie> operator+
(
    const table<Specie>&,
    const table<Specie>&
);

template<class Specie>
inline table<Specie> operator-
(
    const table<Specie>&,
    const table<Specie>&
);

template<class Specie>
inline table<Specie> operator*
(
    const scalar,
    const table<Specie>&
);

template<class Specie>
inline table<Specie> operator==
(
    const table<Specie>&,
    const table<Specie>&
);

template<class Specie>
Ostream& operator<<
(
    Ostream&,
    const table<Specie>&
);


/*---------------------------------------------------------------------------*\
                        Class table Declaration
\*---------------------------------------------------------------------------*/

template<class Specie>
class table
:
    public Specie
{
    // Private data

        //- Density
        scalar rho_;

        //- Density table
        interpolationTable<scalar> rhoTable_;


public:

    // Constructors

        //- Construct from components
        inline table(const Specie& sp, const scalar rho);

        //- Construct from Istream
        table(Istream&);

        //- Construct from dictionary
        table(const dictionary& dict);

        //- Construct as named copy
        inline table(const word& name, const table&);

        //- Construct and return a clone
        inline autoPtr<table> clone() const;

        // Selector from Istream
        inline static autoPtr<table> New(Istream& is);


    // Member functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "table<" + word(Specie::typeName_()) + '>';
        }


        // Fundamental properties

            //- Is the equation of state is incompressible i.e. rho != f(p)
            static const bool incompressible = true;

            //- Is the equation of state is isochoric i.e. rho = const
            static const bool isochoric = true;

            //- Return density [kg/m^3]
            inline scalar rho(scalar p, scalar T) const;

            //- Return enthalpy departure [J/kg]
            inline scalar H(const scalar p, const scalar T) const;

            //- Return Cp departure [J/(kg K)]
            inline scalar Cp(scalar p, scalar T) const;

            //- Return internal energy departure [J/kg]
            inline scalar E(const scalar p, const scalar T) const;

            //- Return Cv departure [J/(kg K]
            inline scalar Cv(scalar p, scalar T) const;

            //- Return entropy [J/(kg K)]
            inline scalar S(const scalar p, const scalar T) const;

            //- Return compressibility rho/p [s^2/m^2]
            inline scalar psi(scalar p, scalar T) const;

            //- Return compression factor []
            inline scalar Z(scalar p, scalar T) const;

            //- Return (Cp - Cv) [J/(kg K)]
            inline scalar CpMCv(scalar p, scalar T) const;


        // IO

            //- Write to Ostream
            void write(Ostream& os) const;


    // Member operators

        inline void operator+=(const table&);
        inline void operator-=(const table&);

        inline void operator*=(const scalar);


    // Friend operators

        friend table operator+ <Specie>
        (
            const table&,
            const table&
        );

        friend table operator- <Specie>
        (
            const table&,
            const table&
        );

        friend table operator* <Specie>
        (
            const scalar s,
            const table&
        );

        friend table operator== <Specie>
        (
            const table&,
            const table&
        );


    // Ostream Operator

        friend Ostream& operator<< <Specie>
        (
            Ostream&,
            const table&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "tableI.H"

#ifdef NoRepository
    #include "table.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
