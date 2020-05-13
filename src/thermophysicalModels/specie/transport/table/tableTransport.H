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
    Foam::tableTransport

Description
    Table properties Transport package.
    Templated into a given thermodynamics package (needed for thermal
    conductivity).

SourceFiles
    tableTransportI.H
    tableTransport.C

\*---------------------------------------------------------------------------*/

#ifndef tableTransport_H
#define tableTransport_H

#include "interpolationTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of friend functions and operators

template<class Thermo> class tableTransport;

template<class Thermo>
inline tableTransport<Thermo> operator+
(
    const tableTransport<Thermo>&,
    const tableTransport<Thermo>&
);

template<class Thermo>
inline tableTransport<Thermo> operator-
(
    const tableTransport<Thermo>&,
    const tableTransport<Thermo>&
);

template<class Thermo>
inline tableTransport<Thermo> operator*
(
    const scalar,
    const tableTransport<Thermo>&
);

template<class Thermo>
inline tableTransport<Thermo> operator==
(
    const tableTransport<Thermo>&,
    const tableTransport<Thermo>&
);

template<class Thermo>
Ostream& operator<<
(
    Ostream&,
    const tableTransport<Thermo>&
);


/*---------------------------------------------------------------------------*\
                           Class tableTransport Declaration
\*---------------------------------------------------------------------------*/

template<class Thermo>
class tableTransport
:
    public Thermo
{
    // Private data

        //- Constant dynamic viscosity [Pa.s]
        scalar mu_;

        //- Thermal conductivity [Wm/K]
        scalar kappa_;

        //- Viscosity table
        interpolationTable<scalar> muTable_;

        //- Thermal conductivity table
        interpolationTable<scalar> kappaTable_;


    // Private Member Functions

        //- Construct from components
        inline tableTransport
        (
            const Thermo& t,
            const scalar mu,
            const scalar kappa
        );


public:

    // Constructors

        //- Construct as named copy
        inline tableTransport(const word&, const tableTransport&);

        //- Construct from Istream
        tableTransport(Istream&);

        //- Construct from dictionary
        tableTransport(const dictionary& dict);

        //- Construct and return a clone
        inline autoPtr<tableTransport> clone() const;

        // Selector from Istream
        inline static autoPtr<tableTransport> New(Istream& is);

        // Selector from dictionary
        inline static autoPtr<tableTransport> New(const dictionary& dict);


    // Member functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "table<" + Thermo::typeName() + '>';
        }

        //- Dynamic viscosity [kg/ms]
        inline scalar mu(const scalar p, const scalar T) const;

        //- Thermal conductivity [W/mK]
        inline scalar kappa(const scalar p, const scalar T) const;

        //- Thermal diffusivity of enthalpy [kg/ms]
        inline scalar alphah(const scalar p, const scalar T) const;

        // Species diffusivity
        //inline scalar D(const scalar p, const scalar T) const;

        //- Write to Ostream
        void write(Ostream& os) const;


    // Member operators

        inline void operator=(const tableTransport&);

        inline void operator+=(const tableTransport&);

        inline void operator-=(const tableTransport&);

        inline void operator*=(const scalar);


    // Friend operators

        friend tableTransport operator+ <Thermo>
        (
            const tableTransport&,
            const tableTransport&
        );

        friend tableTransport operator- <Thermo>
        (
            const tableTransport&,
            const tableTransport&
        );

        friend tableTransport operator* <Thermo>
        (
            const scalar,
            const tableTransport&
        );

        friend tableTransport operator== <Thermo>
        (
            const tableTransport&,
            const tableTransport&
        );


    // Ostream Operator

        friend Ostream& operator<< <Thermo>
        (
            Ostream&,
            const tableTransport&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "tableTransportI.H"

#ifdef NoRepository
    #include "tableTransport.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
