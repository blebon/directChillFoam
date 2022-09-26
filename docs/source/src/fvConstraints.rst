Constraints
===========

This library provides the functionality to limit the solute concentration to be between prescribed minimum and maximum values.

Installation
------------

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.

In the directChillFoam/src/fvConstraints directory, run:

.. code-block:: console
  
  $ wmake libso

Usage
-----

The constraints are entered in the fvConstraints dictionary file in the system folder.  

Example of the limitSolute constraint specification:  

.. code-block:: C
  
  limitC.Cu
  {
      type            limitSolute;

      selectionMode   all;

      solute          C.Cu; // solute field name

      min             0.0;
      max             1.0;
  }
