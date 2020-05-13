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

\*---------------------------------------------------------------------------*/

// #include "specie.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Thermo>
inline Foam::tableTransport<Thermo>::tableTransport
(
    const Thermo& t,
    const scalar mu,
    const scalar kappa
)
:
    Thermo(t),
    mu_(mu),
    kappa_(kappa)
{}


template<class Thermo>
inline Foam::tableTransport<Thermo>::tableTransport
(
    const word& name,
    const tableTransport& ct
)
:
    Thermo(name, ct),
    mu_(ct.mu_),
    kappa_(ct.kappa_)
{}


template<class Thermo>
inline Foam::autoPtr<Foam::tableTransport<Thermo>>
Foam::tableTransport<Thermo>::clone() const
{
    return autoPtr<tableTransport<Thermo>>
    (
        new tableTransport<Thermo>(*this)
    );
}


template<class Thermo>
inline Foam::autoPtr<Foam::tableTransport<Thermo>>
Foam::tableTransport<Thermo>::New
(
    Istream& is
)
{
    return autoPtr<tableTransport<Thermo>>
    (
        new tableTransport<Thermo>(is)
    );
}


template<class Thermo>
inline Foam::autoPtr<Foam::tableTransport<Thermo>>
Foam::tableTransport<Thermo>::New
(
    const dictionary& dict
)
{
    return autoPtr<tableTransport<Thermo>>
    (
        new tableTransport<Thermo>(dict)
    );
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Thermo>
inline Foam::scalar Foam::tableTransport<Thermo>::mu
(
    const scalar p,
    const scalar T
) const
{
    return muTable_(T);
    // return mu_;
}


template<class Thermo>
inline Foam::scalar Foam::tableTransport<Thermo>::kappa
(
    const scalar p,
    const scalar T
) const
{
    return kappaTable_(T);
    // return kappa_;
}


template<class Thermo>
inline Foam::scalar Foam::tableTransport<Thermo>::alphah
(
    const scalar p,
    const scalar T
) const
{
    return kappa(p, T)/this->Cpv(p, T);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Thermo>
inline void Foam::tableTransport<Thermo>::operator=
(
    const tableTransport<Thermo>& ct
)
{
    Thermo::operator=(ct);

    mu_ = ct.mu_;
    kappa_ = ct.kappa_;
}


template<class Thermo>
inline void Foam::tableTransport<Thermo>::operator+=
(
    const tableTransport<Thermo>& st
)
{
    scalar molr1 = this->nMoles();

    Thermo::operator+=(st);

    if (mag(molr1) + mag(st.nMoles()) > SMALL)
    {
        molr1 /= this->nMoles();
        scalar molr2 = st.nMoles()/this->nMoles();

        mu_ = molr1*mu_ + molr2*st.mu_;
        kappa_ = molr1*kappa_ + molr2*st.kappa_;
    }
}


template<class Thermo>
inline void Foam::tableTransport<Thermo>::operator-=
(
    const tableTransport<Thermo>& st
)
{
    scalar molr1 = this->nMoles();

    Thermo::operator-=(st);

    if (mag(molr1) + mag(st.nMoles()) > SMALL)
    {
        molr1 /= this->nMoles();
        scalar molr2 = st.nMoles()/this->nMoles();

        mu_ = molr1*mu_ - molr2*st.mu_;
        kappa_ = molr1*kappa_ - molr2*st.kappa_;
    }
}


template<class Thermo>
inline void Foam::tableTransport<Thermo>::operator*=
(
    const scalar s
)
{
    Thermo::operator*=(s);
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Thermo>
inline Foam::tableTransport<Thermo> Foam::operator+
(
    const tableTransport<Thermo>& ct1,
    const tableTransport<Thermo>& ct2
)
{
    Thermo t
    (
        static_cast<const Thermo&>(ct1) + static_cast<const Thermo&>(ct2)
    );

    if (mag(ct1.nMoles()) + mag(ct2.nMoles()) < SMALL)
    {
        return tableTransport<Thermo>
        (
            t,
            0,
            ct1.kappa_
        );
    }
    else
    {
        scalar molr1 = ct1.nMoles()/t.nMoles();
        scalar molr2 = ct2.nMoles()/t.nMoles();

        return tableTransport<Thermo>
        (
            t,
            molr1*ct1.mu_ + molr2*ct2.mu_,
            molr1*ct1.kappa_ + molr2*ct2.kappa_
        );
    }
}


template<class Thermo>
inline Foam::tableTransport<Thermo> Foam::operator-
(
    const tableTransport<Thermo>& ct1,
    const tableTransport<Thermo>& ct2
)
{
    Thermo t
    (
        static_cast<const Thermo&>(ct1) - static_cast<const Thermo&>(ct2)
    );

    if (mag(ct1.nMoles()) + mag(ct2.nMoles()) < SMALL)
    {
        return tableTransport<Thermo>
        (
            t,
            0,
            ct1.kappa_
        );
    }
    else
    {
        scalar molr1 = ct1.nMoles()/t.nMoles();
        scalar molr2 = ct2.nMoles()/t.nMoles();

        return tableTransport<Thermo>
        (
            t,
            molr1*ct1.mu_ - molr2*ct2.mu_,
            molr1*ct1.kappa_ - molr2*ct2.kappa_
        );
    }
}


template<class Thermo>
inline Foam::tableTransport<Thermo> Foam::operator*
(
    const scalar s,
    const tableTransport<Thermo>& ct
)
{
    return tableTransport<Thermo>
    (
        s*static_cast<const Thermo&>(ct),
        ct.mu_,
        ct.kappa_
    );
}


template<class Thermo>
inline Foam::tableTransport<Thermo> Foam::operator==
(
    const tableTransport<Thermo>& ct1,
    const tableTransport<Thermo>& ct2
)
{
    return ct2 - ct1;
}


// ************************************************************************* //
