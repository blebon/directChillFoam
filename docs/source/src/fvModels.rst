Solidification sources
======================

This source is designed to model the effect of solidification and melting processes. The isotherm phase change occurs at the melting temperature, :math:`{T_{sol} = T_{liq}}`. The not isotherm phase change occurs between solidus and liquidus temperature, :math:`{T_{sol} < T_{liq}}` respectively. The presence of the solid phase in the flow field is incorporated into the model as a momentum porosity contribution; the energy associated with the phase change is added as an enthalpy contribution. The model generates the field \<name\>:alpha1 which can be visualised to show the melt distribution as a fraction [0-1].

Installation
------------

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.

In the directChillFoam/src/fvModels directory, run:

.. code-block:: console
  
  $ wmake libso

Usage
-----

Example usage:  

.. code-block:: C
  
  melt1
  {
      type            mushyZoneSource;
      active          yes;
    
      mushyZoneSourceCoeffs
      {
          selectionMode   all;
          
          Tliq            913.13;
          Tsol            820.98;
          L               392000.0;
          g_env           0.7;
          relax           0.2;
          castingVelocity (0 0 -0.001);
          
          tStar
          {
              type                table;
              format              foam;
              file                "constant/tStar";
              outOfBounds         clamp;
              interpolationScheme linear;
          }
          
          thermoMode      thermo;
          
          rhoRef          2573.;
          beta            2.25e-5;
          
          phi             phi;
          
          Cu              1.0e+05;
          q               1.0e-06;
      }
  }

.. table:: Variables used in the mushy zone source.
  :widths: auto
  
  +-----------------+-------------------------------------+----------+---------------+
  | Property        | Description                         | Required | Default value |
  +=================+=====================================+==========+===============+
  | Tsol            | Solidus temperature [K]             | yes      |               |
  +-----------------+-------------------------------------+----------+---------------+
  | Tliq            | Liquidus temperature [K]            | no       | Tsol          |
  +-----------------+-------------------------------------+----------+---------------+
  | L               | Latent heat of fusion [J/kg]        | yes      |               |
  +-----------------+-------------------------------------+----------+---------------+
  | g_env           | Packing fraction                    | no       | 0.7           |
  +-----------------+-------------------------------------+----------+---------------+
  | relax           | Relaxation coefficient [0-1]        | no       | 0.9           |
  +-----------------+-------------------------------------+----------+---------------+
  | castingVelocity | Casting velocity [m/s]              | yes      |               |
  +-----------------+-------------------------------------+----------+---------------+
  | tStar           | Reverse liquid fraction table       | yes      |               |
  +-----------------+-------------------------------------+----------+---------------+
  | thermoMode      | Thermo mode [thermo\|lookup]        | yes      |               |
  +-----------------+-------------------------------------+----------+---------------+
  | rhoRef          | Reference (solid) density [kg/m^3]  | yes      |               |
  +-----------------+-------------------------------------+----------+---------------+
  | rho             | Name of density field               | no       | rho           |
  +-----------------+-------------------------------------+----------+---------------+
  | T               | Name of temperature field           | no       | T             |
  +-----------------+-------------------------------------+----------+---------------+
  | Cp              | Name of specific heat field         | no       | Cp            |
  +-----------------+-------------------------------------+----------+---------------+
  | U               | Name of velocity field              | no       | U             |
  +-----------------+-------------------------------------+----------+---------------+
  | phi             | Name of flux field                  | no       | phi           |
  +-----------------+-------------------------------------+----------+---------------+
  | Cu              | Model coefficient [1/s]             | no       | 100000        |
  +-----------------+-------------------------------------+----------+---------------+
  | q               | Model coefficient                   | no       | 0.001         |
  +-----------------+-------------------------------------+----------+---------------+
  | beta            | Thermal expansion coefficient [1/K] | yes      |               |
  +-----------------+-------------------------------------+----------+---------------+
  | g               | Accelerartion due to gravity        | no       |               |
  +-----------------+-------------------------------------+----------+---------------+

References
^^^^^^^^^^  

* V.R. Voller, C. Prakash, (1987). "A fixed grid numerical modelling methodology for convection-diffusion mushy region phase-change problems", *International Journal of Heat and Mass Transfer*, **30**\(8), 1709-1719. `doi:10.1016/0017-9310(87)90317-6 <https://doi.org/10.1016/0017-9310(87)90317-6>`_.
* C.R. Swaminathan, V.R. Voller (1992). "A general enthalpy method for modeling solidification processes", *Metallurgical transactions B*, **23**\(5), 651-664. `doi:10.1007/BF02649725 <https://doi.org/10.1007/BF02649725>`_.
* G.S. Bruno Lebon, Georges Salloum-Abou-Jaoude, Dmitry Eskin, Iakovos Tzanakis, Koulis Pericleous, Philippe Jarry, "Numerical modelling of acoustic streaming during the ultrasonic melt treatment of direct-chill (DC) casting", *Ultrasonics Sonochemistry* **54** (2019) 171-182 `doi:10.1016/j.ultsonch.2019.02.002 <https://doi.org/10.1016/j.ultsonch.2019.02.002>`_.
