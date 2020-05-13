/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
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

#include "tableTransport.H"
#include "IOstreams.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Thermo>
Foam::tableTransport<Thermo>::tableTransport(Istream& is)
:
    Thermo(is),
    mu_(readScalar(is)),
    kappa_(readScalar(is))
{
    is.check("tableTransport::tableTransport(Istream& is)");
}


template<class Thermo>
Foam::tableTransport<Thermo>::tableTransport(const dictionary& dict)
:
    Thermo(dict),
    mu_(readScalar(dict.subDict("transport").lookup("mu"))),
    kappa_(readScalar(dict.subDict("transport").lookup("kappa"))),
    muTable_ ("constant/mu"),
    kappaTable_ ("constant/kappa")
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Thermo>
void Foam::tableTransport<Thermo>::tableTransport::write(Ostream& os) const
{
    os  << this->name() << endl;
    os  << token::BEGIN_BLOCK  << incrIndent << nl;

    Thermo::write(os);

    dictionary dict("transport");
    dict.add("mu", mu_);
    dict.add("kappa", kappa_);
    os  << indent << dict.dictName() << dict;

    os  << decrIndent << token::END_BLOCK << nl;
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

template<class Thermo>
Foam::Ostream& Foam::operator<<(Ostream& os, const tableTransport<Thermo>& ct)
{
    operator<<(os, static_cast<const Thermo&>(ct));
    os << tab << ct.mu_ << tab << ct.kappa_;

    os.check("Ostream& operator<<(Ostream&, const tableTransport&)");

    return os;
}


// ************************************************************************* //
