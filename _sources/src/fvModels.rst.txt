======================
Solidification sources
======================

.. contents:: Contents:
  :backlinks: none

This source is designed to model the effect of solidification and melting processes. The isotherm phase change occurs at the melting temperature, :math:`{T_{sol} = T_{liq}}`. The not isotherm phase change occurs between solidus and liquidus temperature, :math:`{T_{sol} < T_{liq}}` respectively. The presence of the solid phase in the flow field is incorporated into the model as a momentum porosity contribution; the energy associated with the phase change is added as an enthalpy contribution. The model generates the field \<name\>:alpha1 which can be visualised to show the melt distribution as a fraction [0-1].

Installation
============

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.

In the directChillFoam/src/fvModels directory, run:

.. code-block:: console
  
  $ wmake libso

C++ Classes
===========

.. doxygenclass:: Foam::fv::mushyZoneSource
  :members:

Testing
=======

The unit tests for the fvModels libraries require the Boost libraries (>= 1.69.0). The tests are run in the tests/fvModels/case folder:

.. code-block:: console
  
  $ wmake .. # Build the test executable
  $ blockMesh # Create the mesh for the test case
  $ ./test_mushyZoneSource --log_level=all # Run the tests
