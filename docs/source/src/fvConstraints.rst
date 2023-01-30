===========
Constraints
===========

.. contents:: Contents:
  :backlinks: none

This library provides the functionality to limit the solute concentration to be between prescribed minimum and maximum values.

Installation
============

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.

In the directChillFoam/src/fvConstraints directory, run:

.. code-block:: console
  
  $ wmake libso

C++ Classes
===========

.. doxygenclass:: Foam::fv::limitSolute
  :members:
