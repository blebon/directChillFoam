Solute equation solver
======================

The multicomponent alloy class for solving the solute transport equation.

Installation
------------

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.
* The :doc:`../src/soluteModel` library.

In the directChillFoam/applications/solver/heatTransfer/directChillFoam/multicomponentAlloy directory, run:

.. code-block:: console
  
  $ wmake libso

Usage
-----

Create the alloy in the CreateFields.H file:

.. code-block:: C

  multicomponentAlloy alloy("alloy", U, rho, rho2, phi, fvModels, fvConstraints);

Solve the solute transport equations in the energy corrector loop:

.. code-block:: C

  alloy.solve(Us, fvModels, fvConstraints);
