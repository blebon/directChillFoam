/*--------------------------------*- C++ -*----------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Version:  9
     \\/     M anipulation  |
\*---------------------------------------------------------------------------*/
FoamFile
{
    format          ascii;
    class           volScalarField;
    location        "0";
    object          C.Mg;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

dimensions      [ 0 0 0 0 0 0 0 ];

internalField   uniform 0.48;

boundaryField
{
    "(hot-top|graphite|mould|air-gap|water-film)"
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
