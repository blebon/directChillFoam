====================================================================
Tutorial: Subroto *et al.* (2021) - DC casting of a ternary Al alloy
====================================================================

This tutorial describes how to pre-process, run and post-process a direct chill (DC) casting case with directChillFoam.

.. figure:: ../images/DC_Schematic.png
  :width: 80%
  :alt: DC casting schematic

  The DC casting process: the melt in contact with the mould solidifies and the billet is pulled downwards by gravity as the ram is lowered. Water is sprayed at the sides of the billet providing the direct chill.

The execution time of this tutorial case is around 12 minutes when run in serial on a node with a 2.10 GHz processor.

Pre-processing
==============

The schematic for the DC casting setup of this case is as follows:

.. Figure:: ../images/Subroto2021_BC.png
  :width: 80%
  :alt: Schematic of the Subroto *et al.* (2021) case

  Boundary patch labels and dimensions of the casting mould and billet. The image is not drawn to scale.

Change to the case directory.

.. code-block:: console

  $ cd directChillFoam/tutorials/heatTransfer/directChillFoam/Subroto2021

Mesh generation
---------------

Move into the system directory.

.. code-block:: console

  $ cd system

Generate the blockMeshDict file using the helper Python script.

.. code-block:: console

  $ python3 cylinder.py

Return to the case directory and generate the mesh.

.. code-block:: console

  $ cd ..
  $ blockMesh

Boundary and initial conditions
-------------------------------

Initialize the fields in the case directory:

.. code-block:: console

  $ setFields

Solute
^^^^^^

Solute fields are dimensionless: 

.. code-block:: C

  dimensions      [0 0 0 0 0 0 0];

The fields are initialized with constant values. Use 0.6 for silicon and 0.48 for magnesium.

.. code-block:: C

  internalField   uniform 0.6; // C.Si

.. code-block:: C

  internalField   uniform 0.48; // C.Mg

The inlet boundary condition is a fixed value:

.. code-block:: C 
  
  free-surface
  {
      type            fixedValue;
      value           $internalField;
  }

All other boundaries are treated with a zero gradient condition.

Temperature
^^^^^^^^^^^

The mould uses the `mould HTC <../src/ThermophysicalTransportModels.html#mould-heat-transfer-coefficient>`_ boundary condition:

.. code-block:: C
  
  "(mould|air-gap)"
  {
      type            mouldHTC;
      mode            coefficient;
      h1              uniform 1250.0;
      h2              uniform 40.0;
      Ta              constant $waterTemp;
      kappaMethod     fluidThermo;
      relaxation      1.0;
      value           uniform $waterTemp;
  }

The surface of the billet that is sprayed with water jets uses the `water film HTC <../src/ThermophysicalTransportModels.html#water-film-heat-transfer-coefficient>`_ boundary condition.
Use the ``Subroto2021.write_htc.write_HTC_T()`` function to generate the Foam temperature-htc table.

.. autofunction:: Subroto2021.write_htc.write_HTC_T
  :noindex:

.. code-block:: C
  
  water-film
  {
      type            waterFilmHTC;
      mode            coefficient;
      htc
      {
          type                table;
          format              foam;
          file                "constant/HTC_Tw";
          outOfBounds         clamp;
          interpolationScheme linear;
      }
      Ta              constant $waterTemp;
      kappaMethod     fluidThermo;
      relaxation      0.3;
      value           uniform $waterTemp;
  }

Velocity
^^^^^^^^

The casting velocity :math:`{U}` is prescribed at the mould and water-cooled surface using a coded function:

.. code-block:: C 

  "(graphite|mould|air-gap|water-film)"
  {
      type            codedFixedValue;
      value           $internalField;
      redirectType    movingShell;
      name            movingShell;

      code
      #{
          // Initialize velocity with zeroes
          vectorField U(patch().size(), vector(0, 0, 0));
          
          // Find liquid fraction
          const scalarField& fL = this->patch().lookupPatchField<volScalarField, scalar>("melt1_alpha1");
          
          // Go through all face centres
          const vectorField& centre(patch().Cf());
          forAll(centre, i)
          {
              U[i].z() = -0.00233*(1-fL[i]);
          }
          
          // Apply the calculated velocities
           operator==(U);
      #};
      
      codeInclude
      #{
          // #include "fvCFD.H";
      #};
    }

Solid velocity
^^^^^^^^^^^^^^

The movingShell coded function from the liquid velocity boundary condition is reused for the solid velocity :math:`{U_s}` boundary prescription:

.. code-block:: C 

  "(graphite|mould|air-gap|water-film)"
  {
      type            movingShell;
      value           $internalField;
  }

Physical properties
-------------------

Transport properties
^^^^^^^^^^^^^^^^^^^^

The `transport properties <../solver/directChillFoam.html#nomenclature>`_ are prescribed in the constant/transportProperties dictionary file.

Example usage:

.. code-block:: C 

  g_env           [ 0 0 0 0 0 0 0 ] 0.7;
  rho1            [ 1 -3 0 0 0 0 0 ] 2378.65;
  rho2            [ 1 -3 0 0 0 0 0 ] 2602.04;
  mu1             [ 1 -1 -1 0 0 0 0 ] 0.0010155;
  mu2             [ 1 -1 -1 0 0 0 0 ] 0.0010155;
  DAS             [ 0 1 0 0 0 0 0 ] 0.000185;

Thermophysical properties
^^^^^^^^^^^^^^^^^^^^^^^^^

The thermophysical properties are prescribed in the constant/thermophysicalProperties dictionary file.

Solute properties
^^^^^^^^^^^^^^^^^

The `solute properties <../src/soluteModel.html#nomenclature>`_ are prescribed in the constant/soluteProperties dictionary file.

Example usage:

.. code-block:: C 
  
  solutes
  (
      Si
      {
          D_l   3e-9;
          kp    0.102068;
          C0    0.6;
          Ceut  0.32;
          beta  -3.7e-4;
      }
      
      Mg
      {
          D_l   3e-9;
          kp    0.316119;
          C0    0.48;
          Ceut  0.32;
          beta  1.3e-4;
      }
  );

Solidification properties
^^^^^^^^^^^^^^^^^^^^^^^^^

The solidification properties are prescribed in the constant/fvModels dictionary file.

Example usage: 

.. code-block:: C 
    
  melt1
  {
      type            mushyZoneSource;
      active          yes;
    
      mushyZoneSourceCoeffs
      {
          selectionMode   all;
          
          Tliq            928.236;
          Tsol            805.503;
          L               351540.0;
          g_env           0.7;
          relax           0.1;
          castingVelocity (0 0 -0.00233);
          
          tStar
          {
              type                table;
              format              foam;
              file                "constant/tStar";
              outOfBounds         clamp;
              interpolationScheme linear;
          }
          
          thermoMode      thermo;
          rhoRef          2602.044;
          beta            23e-6;
          phi             phi;
          Cu              1.522e+07;
          q               1e-03;
      }
  }

Control
-------

The solidification and other associated libraries must be included in the system/controlDict dictionary file.

.. code-block:: C 
  
  libs (
      "libmyFvModels.so"
      "libmyFvConstraints.so"
      "libmythermophysicalTransportModels.so"
  );

Discretization and solver settings
----------------------------------

Discretization schemes are entered in the system/fvSchemes dictionary file. Upwinding is recommended for the solute equations.

.. code-block:: C 
  
  divSchemes
  {
      ...
      div(phi,C.Si)   bounded Gauss upwind;
      div(phi,C.Mg)   bounded Gauss upwind;
  }

The number of energy corrector loops is prescribed in the PIMPLE entry of the system/fvSolution dictionary file.

.. code-block:: C 
  
  PIMPLE
  {
      ...
      nEnergyCorrectors 3;
  }

Run the application
===================

In the case directory, run:

.. code-block:: console

  $ directChillFoam

Post-processing
===============

Contour plots
-------------

The sump profile can be plotted from the VTK files that are saved in the 
postProcessing directory using the ``Subroto2021.plot_sump.plot_sump()`` function.

.. autofunction:: Subroto2021.plot_sump.plot_sump
  :noindex:

.. Figure:: ../images/Subroto2021_Sump.png
  :width: 80%
  :alt: Predicted sump profile

  Predicted sump profile.

Validation
----------

The simulation can be verified by comparing the predicted temperatures at the
billet centre line, mid-radius and surface with experimental measurements. Use
the ``Subroto2021.plot_line.plot_line()`` function:

.. autofunction:: Subroto2021.plot_line.plot_line
  :noindex:

.. Figure:: ../images/Subroto2021_100.0.png
  :width: 80%
  :alt: Temperature line plots

  Comparing the predicted temperatures (solid lines) with experimental data (markers).
