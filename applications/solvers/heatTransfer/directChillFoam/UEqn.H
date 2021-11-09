    // Solve the Momentum equation
    alpha = melt1_alpha1;

    MRF.correctBoundaryVelocity(U);

    // In preparation for macrosegregation handling
    volScalarField gl("gl", alpha);
    forAll(gl, i) {
        if (alpha[i] < g_env.value()) {
            gl[i] = 0.0;
        }
        else {
            gl[i] = rho2.value()/(rho1.value()*((1.0/alpha[i]) - 1.0) + rho2.value());
        }
    }

    Us = U + gl * (rho1 - rho2) * DAS*DAS * g / (18 * mu);

    tmp<fvVectorMatrix> tUEqn
    (
        fvm::ddt(rho, U) + fvm::div(phi, U)
      + MRF.DDt(rho, U)
      + turbulence->divDevTau(U)
     ==
        fvModels.source(rho, U)
    );
    fvVectorMatrix& UEqn = tUEqn.ref();

    UEqn.relax();

    fvConstraints.constrain(UEqn);

    if (pimple.momentumPredictor())
    {
        solve
        (
            UEqn
         ==
            fvc::reconstruct
            (
                (
                  - ghf*fvc::snGrad(rho)
                  - fvc::snGrad(p_rgh)
                )*mesh.magSf()
            )
        );

        fvConstraints.constrain(U);
        K = 0.5*magSqr(U);
    }
