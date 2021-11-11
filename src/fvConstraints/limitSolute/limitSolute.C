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

\*---------------------------------------------------------------------------*/

#include "limitSolute.H"
#include "soluteModel.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(limitSolute, 0);
    addToRunTimeSelectionTable
    (
        fvConstraint,
        limitSolute,
        dictionary
    );
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::fv::limitSolute::readCoeffs()
{
    Smin_ = coeffs().lookup<scalar>("min");
    Smax_ = coeffs().lookup<scalar>("max");
    SName_ = coeffs().lookupOrDefault<word>("solute", word::null);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::limitSolute::limitSolute
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    fvConstraint(name, modelType, dict, mesh),
    set_(coeffs(), mesh),
    Smin_(-vGreat),
    Smax_(vGreat),
    SName_(word::null)
{
    readCoeffs();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::wordList Foam::fv::limitSolute::constrainedFields() const
{
    return wordList(1, SName_);
}


bool Foam::fv::limitSolute::constrain(volScalarField& C) const
{
    const labelList& cells = set_.cells();
    
    if (C.name() != SName_ ){
        return cells.size();
    }

    if (debug)
    {
        Info<< type() << ": applying constraint to " << SName_ << endl;
    }

    forAll(cells, i)
    {
        const label celli = cells[i];

        C[celli] = Foam::max(Foam::min(C[celli], Smax_), Smin_);
    }

    // Handle boundaries
    volScalarField::Boundary& bf = C.boundaryFieldRef();

    forAll(bf, patchi)
    {
        fvPatchScalarField& Cp = bf[patchi];

        if (!Cp.fixesValue())
        {
            forAll(Cp, facei)
            {
                Cp[facei] =
                    Foam::max(Foam::min(Cp[facei], Smax_), Smin_);
            }
        }
    }

    return cells.size();
}


void Foam::fv::limitSolute::updateMesh(const mapPolyMesh& mpm)
{
    set_.updateMesh(mpm);
}


bool Foam::fv::limitSolute::read(const dictionary& dict)
{
    if (fvConstraint::read(dict))
    {
        set_.read(coeffs());
        readCoeffs();
        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
