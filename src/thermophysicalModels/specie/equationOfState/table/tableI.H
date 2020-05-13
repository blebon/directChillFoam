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

\*---------------------------------------------------------------------------*/

#include "table.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Specie>
inline Foam::table<Specie>::table
(
    const Specie& sp,
    const scalar rho
)
:
    Specie(sp),
    rho_(rho)
{}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Specie>
inline Foam::table<Specie>::table
(
    const word& name,
    const table<Specie>& ico
)
:
    Specie(name, ico),
    rho_(ico.rho_)
{}


template<class Specie>
inline Foam::autoPtr<Foam::table<Specie>>
Foam::table<Specie>::clone() const
{
    return autoPtr<table<Specie>>(new table<Specie>(*this));
}


template<class Specie>
inline Foam::autoPtr<Foam::table<Specie>>
Foam::table<Specie>::New(Istream& is)
{
    return autoPtr<table<Specie>>(new table<Specie>(is));
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::scalar Foam::table<Specie>::rho(scalar p, scalar T) const
{
    return rhoTable_(T);
    // return rho_;
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::H(scalar p, scalar T) const
{
    // return 0;
    return p/this->rho(p, T);
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::Cp(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::E(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::Cv(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::S(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::psi(scalar p, scalar T) const
{
    return this->rho(p, T)/p;
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::Z(scalar p, scalar T) const
{
    return 1.0;
}


template<class Specie>
inline Foam::scalar Foam::table<Specie>::CpMCv(scalar p, scalar T) const
{
    return 0;
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Specie>
inline void Foam::table<Specie>::operator+=(const table<Specie>& ico)
{
    scalar Y1 = this->Y();
    Specie::operator+=(ico);

    if (mag(this->Y()) > small)
    {
        Y1 /= this->Y();
        const scalar Y2 = ico.Y()/this->Y();

        rho_ = Y1*rho_ + Y2*ico.rho_;
    }
}


template<class Specie>
inline void Foam::table<Specie>::operator-=(const table<Specie>& ico)
{
    scalar Y1 = this->Y();
    Specie::operator-=(ico);

    if (mag(this->Y()) > small)
    {
        Y1 /= this->Y();
        const scalar Y2 = ico.Y()/this->Y();

        rho_ = Y1*rho_ - Y2*ico.rho_;
    }
}


template<class Specie>
inline void Foam::table<Specie>::operator*=(const scalar s)
{
    Specie::operator*=(s);
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::table<Specie> Foam::operator+
(
    const table<Specie>& ico1,
    const table<Specie>& ico2
)
{
    Specie sp
    (
        static_cast<const Specie&>(ico1)
      + static_cast<const Specie&>(ico2)
    );

    if (mag(sp.Y()) < small)
    {
        return table<Specie>
        (
            sp,
            ico1.rho_
        );
    }
    else
    {
        const scalar Y1 = ico1.Y()/sp.Y();
        const scalar Y2 = ico2.Y()/sp.Y();

        return table<Specie>
        (
            sp,
            Y1*ico1.rho_ + Y2*ico2.rho_
        );
    }
}


template<class Specie>
inline Foam::table<Specie> Foam::operator-
(
    const table<Specie>& ico1,
    const table<Specie>& ico2
)
{
    Specie sp
    (
        static_cast<const Specie&>(ico1)
      - static_cast<const Specie&>(ico2)
    );

    if (mag(sp.Y()) < small)
    {
        return table<Specie>
        (
            sp,
            ico1.rho_
        );
    }
    else
    {
        const scalar Y1 = ico1.Y()/sp.Y();
        const scalar Y2 = ico2.Y()/sp.Y();

        return table<Specie>
        (
            sp,
            Y1*ico1.rho_ - Y2*ico2.rho_
        );
    }
}


template<class Specie>
inline Foam::table<Specie> Foam::operator*
(
    const scalar s,
    const table<Specie>& ico
)
{
    return table<Specie>(s*static_cast<const Specie&>(ico), ico.rho_);
}


template<class Specie>
inline Foam::table<Specie> Foam::operator==
(
    const table<Specie>& ico1,
    const table<Specie>& ico2
)
{
    Specie sp
    (
        static_cast<const Specie&>(ico1)
     == static_cast<const Specie&>(ico2)
    );

    const scalar Y1 = ico1.Y()/sp.Y();
    const scalar Y2 = ico2.Y()/sp.Y();

    return table<Specie>
    (
        sp,
        Y2*ico2.rho_ - Y1*ico1.rho_
    );
}


// ************************************************************************* //
