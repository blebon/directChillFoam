Solute model
============

The implementation of the solute model as an OpenFOAM class. The liquid concentration field is updated by the correct() function using either the Scheil model or an interpolation table.

Installation
------------

Pre-requisite:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.

In the directChillFoam/applications/solver/heatTransfer/directChillFoam/multicomponentAlloy/soluteModel directory, run:

.. code-block:: console
  
  $ wmake libso

Nomenclature
------------

.. table:: Variables used by the solute model class.
  :widths: auto

  +-----------------+--------------------------------------------------+
  | Variable        | Description                                      |
  +=================+==================================================+
  | D_l             | Liquid mass diffusion coefficient                |
  +-----------------+--------------------------------------------------+
  | k_p             | Partition coefficient                            |
  +-----------------+--------------------------------------------------+
  | C0              | Initial concentration                            |
  +-----------------+--------------------------------------------------+
  | Ceut            | Eutectic concentration                           |
  +-----------------+--------------------------------------------------+
  | beta            | Solutal expansion coefficient                    |
  +-----------------+--------------------------------------------------+

C++ Classes
-----------

.. doxygenclass:: Foam::soluteModel
  :members:
