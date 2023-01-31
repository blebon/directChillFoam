===================
Boundary conditions
===================

.. contents:: Contents:
  :backlinks: none

Robin boundary conditions apply a heat flux condition to temperature on an external wall.

Two boundary conditions are available in this library: the `mould HTC <#mould-heat-transfer-coefficient>`_ and the `water film HTC <#water-film-heat-transfer-coefficient>`_.

Installation
============

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.

In the directChillFoam/src/ThermophysicalTransportModels directory, run:

.. code-block:: console
  
  $ wmake libso

Mould heat transfer coefficient
===============================

.. doxygenclass:: Foam::mouldHTCFvPatchScalarField
  :members:

Water film heat transfer coefficient
===============================

.. doxygenclass:: Foam::waterFilmHTCFvPatchScalarField
  :members:

Testing
=======

The unit tests for the ThermophysicalTransportModels libraries require the Boost libraries (>= 1.69.0). The tests are run in the tests/ThermophysicalTransportModels/<boundaryType>/case folder:

.. code-block:: console
  
  $ wmake .. # Build the test executable
  $ blockMesh # Create the mesh for the test case
  $ ./test_mouldHTC --log_level=all # Run the tests. Use ./test_waterFilmHTC for the waterFilmHTC boundary condition.
