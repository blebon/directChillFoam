const volScalarField& psi = thermo.psi();

// Coherency fraction
dimensionedScalar g_env
(
    "g_env",
    dimless,
    transportProperties.lookup("g_env")
);

// Solid density
dimensionedScalar rho1
(
    "rho1",
    dimDensity,
    transportProperties.lookup("rho1")
);


// Liquid density
dimensionedScalar rho2
(
    "rho2",
    dimDensity,
    transportProperties.lookup("rho2")
);


// Not required but makes implementation easier
dimensionedScalar mu1
(
    "mu1",
    dimDynamicViscosity,
    transportProperties.lookup("mu1")
);


// Dynamic viscosity
dimensionedScalar mu2
(
    "mu2",
    dimDynamicViscosity,
    transportProperties.lookup("mu2")
);

// Dendrite arm spacing
dimensionedScalar DAS
(
    "DAS",
    dimLength,
    transportProperties.lookup("DAS")
);