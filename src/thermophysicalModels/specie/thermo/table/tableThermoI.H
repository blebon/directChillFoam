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

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class EquationOfState>
inline Foam::tableThermo<EquationOfState>::tableThermo
(
    const EquationOfState& st,
    const scalar hf
)
:
    EquationOfState(st),
    Hf_(hf)
{}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class EquationOfState>
inline Foam::tableThermo<EquationOfState>::tableThermo
(
    const word& name,
    const tableThermo& ct
)
:
    EquationOfState(name, ct),
    Hf_(ct.Hf_)
{}


template<class EquationOfState>
inline Foam::autoPtr<Foam::tableThermo<EquationOfState>>
Foam::tableThermo<EquationOfState>::clone() const
{
    return autoPtr<tableThermo<EquationOfState>>
    (
        new tableThermo<EquationOfState>(*this)
    );
}


template<class EquationOfState>
inline Foam::autoPtr<Foam::tableThermo<EquationOfState>>
Foam::tableThermo<EquationOfState>::New(const dictionary& dict)
{
    return autoPtr<tableThermo<EquationOfState>>
    (
        new tableThermo<EquationOfState>(dict)
    );
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class EquationOfState>
inline Foam::scalar Foam::tableThermo<EquationOfState>::limit
(
    const scalar T
) const
{
    return T;
}


template<class EquationOfState>
inline Foam::scalar Foam::tableThermo<EquationOfState>::Cp
(
    const scalar p,
    const scalar T
) const
{
    return CpTable_(T) + EquationOfState::Cp(p, T);
}


template<class EquationOfState>
inline Foam::scalar Foam::tableThermo<EquationOfState>::Ha
(
    const scalar p, const scalar T
) const
{
    return this->Cp(p, T)*T + Hf_ + EquationOfState::H(p, T);
    // return hsTable_(T) + Hf_;
}


template<class EquationOfState>
inline Foam::scalar Foam::tableThermo<EquationOfState>::Hs
(
    const scalar p, const scalar T
) const
{
    return Ha(p, T) - Hc();
    // return hsTable_(T);
}


template<class EquationOfState>
inline Foam::scalar Foam::tableThermo<EquationOfState>::Hc() const
{
    return Hf_;
}


template<class EquationOfState>
inline Foam::scalar Foam::tableThermo<EquationOfState>::S
(
    const scalar p, const scalar T
) const
{
    return 0;
    // return this->Cp(p, T)*log(T/Tstd) + EquationOfState::S(p, T);
}


// ************************************************************************* //
