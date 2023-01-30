================================
Direct-chill (DC) casting solver
================================

.. contents:: Contents:
  :backlinks: none

directChillFoam is based on `buoyantPimpleFoam <https://github.com/OpenFOAM/OpenFOAM-9/tree/master/applications/solvers/heatTransfer/buoyantPimpleFoam>`_, a transient solver for buoyant, turbulent flow of compressible fluids and heat-transfer. directChillFoam adds the following capabilities to the solver:  

* Solid motion prescribed at a casting speed (SI units).
* Handling solidification including mushy and slurry zones in the transition region.
* Robin boundary condition to prescribe the direct-chill (water cooling upon billet exit from mould).
* A solute transport solver

Definition in file directChillFoam.C

Installation
============

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.
* All the included libraries: 
  
  * :doc:`../src/soluteModel`
  * :doc:`../src/multicomponentAlloy`
  * :doc:`../src/ThermophysicalTransportModels`
  * :doc:`../src/fvConstraints`
  * :doc:`../src/fvModels`

In the directChillFoam/applications/solver/heatTransfer/directChillFoam directory, run:

.. code-block:: console
  
  $ wmake

Running the application
=======================

In the case directory, run:

.. code-block:: console
  
  $ directChillFoam

Theory
======

Full details of the theory behind the DC casting solver implementation can be found in G.S. Bruno Lebon, Georges Salloum-Abou-Jaoude, Dmitry Eskin, Iakovos Tzanakis, Koulis Pericleous, Philippe Jarry, "Numerical modelling of acoustic streaming during the ultrasonic melt treatment of direct-chill (DC) casting", *Ultrasonics Sonochemistry* **54** (2019) 171-182 `doi:10.1016/j.ultsonch.2019.02.002 <https://doi.org/10.1016/j.ultsonch.2019.02.002>`_.

Nomenclature
============

.. table:: Variables used in direct chill casting simulations.
  :widths: auto

  +----------+------------------------------------+
  | Variable | Description                        |
  +==========+====================================+
  | g_env    | Coherency fraction                 |
  +----------+------------------------------------+
  | rho1     | Solid density [kg/m^3]             |
  +----------+------------------------------------+
  | rho2     | Liquid density [kg/m^3]            |
  +----------+------------------------------------+
  | mu2      | Dynamic viscosity of liquid [Pa s] |
  +----------+------------------------------------+
  | DAS      | Dendrite arm spacing [m]           |
  +----------+------------------------------------+
  