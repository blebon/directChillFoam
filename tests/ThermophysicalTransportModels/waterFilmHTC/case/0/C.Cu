/*--------------------------------*- C++ -*----------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Version:  9
     \\/     M anipulation  |
\*---------------------------------------------------------------------------*/
FoamFile
{
    format      ascii;
    class       volScalarField;
    location    "0";
    object      C.Cu;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

dimensions      [0 0 0 0 0 0 0];

internalField   uniform 0.06;

boundaryField
{
    "(water-film|mould|air-gap|ceramic|hot-top)"
    {
        type            zeroGradient;
    }

    free-surface
    {
        type            fixedValue;
        value           $internalField;
    }

    ram
    {
        type            zeroGradient;
    }

    symmetry_planes
    {
        type            symmetry;
    }
}


// ************************************************************************* //
