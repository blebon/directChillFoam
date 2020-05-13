for (int Ecorr=0; Ecorr<nEnergyCorrectors; Ecorr++)
{
    volScalarField alpha0(melt1_alpha1);

    #include "EEqn.H"

    rho = thermo.rho();
    mu = thermo.mu();
    
    alpha = melt1_alpha1;

    scalarField residualField
    (
        mag(alpha.internalField() - alpha0.internalField())
    );

    scalar residual(gMax(residualField));

    Info<< "Energy iteration " << (Ecorr + 1)
        << " melt1_alpha1 residual: " << residual << endl;
    
    if (residual < 1e-8)
    {
        break;
    }      
}