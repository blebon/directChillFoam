======================================================================
Tutorial: Vreeman *et al.* (2002) - DC casting of a binary Al-Cu alloy
======================================================================

.. contents:: Contents:
  :backlinks: none

This tutorial describes the steps to pre-process, run and post-process an axisymmetryc direct chill (DC) casting case that corresponds to the binary Al-Cu alloy casting experiment described in `Vreeman et al. (2002) <https://doi.org/10.1115/1.1482089>`_ with directChillFoam.

.. figure:: ../images/DC_Schematic.png
  :width: 80%
  :alt: DC casting schematic

  The DC casting process: the melt in contact with the mould solidifies and the billet is pulled downwards by gravity as the ram is lowered. Water is sprayed at the sides of the billet providing the direct chill.

The execution time of this tutorial case is around 15 minutes when run in serial on a node with a 2.10 GHz processor.

Pre-processing
==============

The schematic for the DC casting setup of this case is as follows:

.. _Vreeman2002_BC:

.. Figure:: ../images/Vreeman2002_BC.png
  :width: 80%
  :alt: Schematic of the Vreeman *et al.* (2002) case

  Boundary patch labels and dimensions of the casting mould and billet. The image is not drawn to scale.

The first step is to generate the mesh for the case and setup the boundary conditions.

To run the pre-processing steps, change to the case directory.

.. code-block:: console

  $ cd directChillFoam/tutorials/heatTransfer/directChillFoam/Vreeman2002

Mesh generation
---------------

The dictionary file that describes the mesh is located in the system directory.

.. code-block:: console

  $ cd system

Generate the blockMeshDict file using the ``Vreeman2002.system.cylinder.write_blockMeshDict()`` function from the `system/cylinder.py script <modules.html#module-Vreeman2002.system.cylinder>`_.

.. code-block:: console

  $ python3 cylinder.py

The axisymmetryc mesh consists of a slice of a cylinder that matches the billet dimensions. The z_points tuple list the vertices z coordinates as defined in :numref:`vreeman2002_bc`. The patch names are entered in the faces list in the ``Vreeman2002.system.cylinder.write_boundary()`` function.

Return to the case directory and generate the mesh using `blockMesh <https://doc.cfd.direct/openfoam/user-guide-v9/blockmesh>`_.

.. code-block:: console

  $ cd ..
  $ blockMesh

Boundary and initial conditions
-------------------------------

To speed up the simulation, initialize the fields with rough estimates of temperatures (and corresponding liquid fractions) in the case directory:

.. code-block:: console

  $ setFields

.. warning::
  The liquid fraction values must match the temperatures for the alloy according to the constant/tStar Foam table. Otherwise, the case can diverge.

Solute
^^^^^^

The solute field is dimensionless: 

.. code-block:: C

  dimensions      [0 0 0 0 0 0 0];

The field is initialized with a constant value:

.. code-block:: C

  internalField   uniform 0.06;

The inlet boundary condition is a fixed value (stored in internalField):

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
Use the ``Vreeman2002.write_htc.write_HTC_T()`` function to generate the Foam temperature-htc table.

.. autofunction:: Vreeman2002.write_htc.write_HTC_T
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
              U[i].z() = -0.001*(1-fL[i]);
          }
          
          // Apply the calculated velocities
           operator==(U);
      #};
      
      codeInclude
      #{
          // #include "fvCFD.H";
      #};
    }

The values are initialized at the casting speed in m s\ :sup:`-1`.

Solid velocity
^^^^^^^^^^^^^^

The movingShell coded function from the liquid velocity boundary condition is reused for the solid velocity :math:`{U_s}` boundary prescription:

.. code-block:: C 

  "(graphite|mould|air-gap|water-film)"
  {
      type            movingShell;
      value           $internalField;
  }

The values are also initialized at the casting speed in m s\ :sup:`-1`.

Physical properties
-------------------

Transport properties
^^^^^^^^^^^^^^^^^^^^

The `transport properties <../solver/directChillFoam.html#nomenclature>`_ are prescribed in the constant/transportProperties dictionary file.  All values are in SI units. These values are available for the alloy that is being studied either in engineering tables or by calculating them using a CALPHAD software package.

Example usage:

.. code-block:: C 

  g_env [ 0  0  0 0 0 0 0 ] 0.7;     // Coherency fraction
  rho1  [ 1 -3  0 0 0 0 0 ] 1750;    // Solid density
  rho2  [ 1 -3  0 0 0 0 0 ] 2490;    // Liquid density
  mu1   [ 1 -1 -1 0 0 0 0 ] 1.28e-3; // Not required, but implementation requires a non-zero value
  mu2   [ 1 -1 -1 0 0 0 0 ] 1.28e-3; // Dynamic viscosity (of liquid)
  DAS   [ 0  1  0 0 0 0 0 ] 1.85e-4; // Dendrite arm spacing

Thermophysical properties
^^^^^^^^^^^^^^^^^^^^^^^^^

The thermophysical properties are prescribed in the `constant/thermophysicalProperties dictionary file <https://doc.cfd.direct/openfoam/user-guide-v9/thermophysical>`_.

Solute properties
^^^^^^^^^^^^^^^^^

The `solute properties <../src/soluteModel.html#nomenclature>`_ are prescribed in the constant/soluteProperties dictionary file.

Example usage:

.. code-block:: C 
  
  solutes
  (
      Cu
      {
          D_l   5.66e-09; // Liquid mass diffusion coefficient
          kp    0.171;    // Partition coefficient
          C0    0.06;     // Initial solute concentration
          Ceut  0.32;     // Eutectic concentration
          beta  -7.3e-3;  // Solute expansion coefficient
      }
  );

.. note::

  These values are also obtained from engineering tables or by calculating them using a CALPHAD software package.

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
          
          Tliq            913.13;         // Liquidus temperature [K]
          Tsol            820.98;         // Solidus temperature [K]
          L               392000.0;       // Latent heat of fusion [J/kg]
          g_env           0.7;            // Coherency fraction
          relax           0.2;            // Under-relaxation factor [0-1] - keep this value low if simulation is unstable
          castingVelocity (0 0 -0.001);   // Casting velocity [m/s]
          
          tStar
          // liquid fraction-temperature table
          {
              type                table;
              format              foam;
              file                "constant/tStar";
              outOfBounds         clamp;
              interpolationScheme linear;
          }
          
          thermoMode      thermo;
          rhoRef          2573.;    // Reference (solid) density [kg/m^3]
          beta            2.25e-5;  // Thermal expansion coefficient [1/K]
          phi             phi;      // Name of flux field
          Cu              1.0e+05;  // Mushy region momentum sink coefficient [1/s]
          q               1.0e-06;  // Carman-Kozeny model coefficient
      }
  }

.. note::

  These values are also obtained from engineering tables or by calculating them using a CALPHAD software package.

  The tStar table can be obtained using a CALPHAD calculation using an appropriate diffusion model e.g. the lever rule or the Scheil equation. When using a non-linear model, use small (liquid fraction) steps of the order of 0.001 if the energy solver diverges.

Control
-------

The solidification and other associated libraries must be included in the `system/controlDict dictionary file <https://doc.cfd.direct/openfoam/user-guide-v9/global-settings>`_.

.. code-block:: C 
  
  libs (
      "libmyFvModels.so"
      "libmyFvConstraints.so"
      "libmythermophysicalTransportModels.so"
  );

Discretization and solver settings
----------------------------------

Discretization schemes are entered in the `system/fvSchemes dictionary file <https://doc.cfd.direct/openfoam/user-guide-v9/fvschemes>`_. Upwinding is recommended for the solute equations.

.. code-block:: C 
  
  divSchemes
  {
      ...
      div(phi,C.Cu)    bounded Gauss upwind;
  }

The number of energy corrector loops is prescribed in the PIMPLE entry of the `system/fvSolution dictionary file <https://doc.cfd.direct/openfoam/user-guide-v9/fvsolution>`_.

.. code-block:: C 
  
  PIMPLE
  {
      ...
      nEnergyCorrectors 3;
  }

Run the application
===================

Once the case has been setup, run `directChillFoam` in the case directory:

.. code-block:: console

  $ cd directChillFoam/tutorials/heatTransfer/directChillFoam/Vreeman2002
  $ directChillFoam

Post-processing
===============

Contour plots
-------------

The sump profile (:numref:`vreeman2002_sump`) can be plotted from the VTK files that are saved in the 
postProcessing directory using the ``Vreeman2002.plot_sump.plot_sump()`` function.

.. autofunction:: Vreeman2002.plot_sump.plot_sump
  :noindex:

.. _Vreeman2002_Sump:

.. Figure:: ../images/Vreeman2002_Sump.png
  :width: 80%
  :alt: Predicted sump profile

  Predicted sump profile.

Validation
----------

The simulation can be verified by comparing the predicted temperatures at the
billet centre line, mid-radius and surface with experimental measurements. Use
the ``Vreeman2002.plot_line.plot_line()`` function to plot :numref:`vreeman2002_800`:

.. autofunction:: Vreeman2002.plot_line.plot_line
  :noindex:

.. _Vreeman2002_800:

.. Figure:: ../images/Vreeman2002_800.0.png
  :width: 80%
  :alt: Temperature line plots

  Comparing the predicted temperatures (solid lines) with experimental data (markers).
