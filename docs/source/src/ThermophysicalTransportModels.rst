Boundary conditions
===================

Robin boundary conditions apply a heat flux condition to temperature on an external mode in one of three modes:  

* fixed power: supply Q
* fixed heat flux: supply q
* fixed heat transfer coefficient: supply h (h1/h2 or htc) and Ta

where 

+----+-------------------------------------+
| Q  | Power [W]                           |
+----+-------------------------------------+
| q  | Heat flux [W/m^2]                   |
+----+-------------------------------------+
| h  | Heat transfer coefficient [W/m^2/K] |
+----+-------------------------------------+
| Ta | Ambient temperature [K]             |
+----+-------------------------------------+

Two boundary conditions are available in this library: the `mould HTC <#mould-heat-transfer-coefficient>`_ and the `water film HTC <#water-film-heat-transfer-coefficient>`_.

For the heat transfer coefficient mode, optional thin thermal layer resistances can be specified through thicknessLayers and kappaLayers entries.

The thermal conductivity kappa can either be retrieved from various possible sources, as detailed in the class temperatureCoupledBase.

The ambient temperature Ta is specified as a Foam::Function1 of time, but uniform is space.

Installation
------------

Pre-requisites:  

* A working installation of `OpenFOAM 9 <https://openfoam.org/release/9/>`_.

In the directChillFoam/src/ThermophysicalTransportModels directory, run:

.. code-block:: console
  
  $ wmake libso

Usage
-----

Mould heat transfer coefficient
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. table:: Properties for mould heat transfer coefficient boundary condition.
  :widths: auto

  +-----------------+--------------------------------------------------+------------------------+---------------+
  | Property        | Description                                      | Required               | Default value |
  +=================+==================================================+========================+===============+
  | mode            | 'power', 'flux' or 'coefficient'                 | yes                    |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | Q               | Power [W]                                        | for mode 'power'       |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | q               | Heat flux [W/m^2]                                | for mode 'flux'        |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | h1              | mould heat transfer coefficient [W/m^2/K]        | yes*                   |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | h2              | air gap heat transfer coefficient [W/m^2/K]      | yes*                   |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | Ta              | Ambient temperature [K]                          | for mode 'coefficient' |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | thicknessLayers | Layer thicknesses [m]                            | no                     |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | kappaLayers     | Layer thermal conductivities [W/m/K]             | no                     |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | relaxation      | Relaxation for the wall temperature              | no                     | 1             |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | emissivity      | Surface emissivity for radiative flux to ambient | no                     | 0             |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | qr              | Name of the radiative field                      | no                     | none          |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | qrRelaxation    | Relaxation factor for radiative field            | no                     | 1             |
  +-----------------+--------------------------------------------------+------------------------+---------------+

Example of the boundary condition specification:  

.. code-block:: c
  
  <patchName>
  {
      type            mouldHTC;

      mode            coefficient;

      Ta              constant 300.0;
      h1              uniform 100.0;
      h2              uniform 10.0;
      thicknessLayers (0.1 0.2 0.3 0.4);
      kappaLayers     (1 2 3 4);

      value           $internalField;
  }

Water film heat transfer coefficient
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. table:: Properties for water film heat transfer coefficient boundary condition.
  :widths: auto

  +-----------------+--------------------------------------------------+------------------------+---------------+
  | Property        | Description                                      | Required               | Default value |
  +=================+==================================================+========================+===============+
  | mode            | 'power', 'flux' or 'coefficient'                 | yes                    |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | Q               | Power [W]                                        | for mode 'power'       |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | q               | Heat flux [W/m^2]                                | for mode 'flux'        |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | htc             | Heat       transfer coefficient [W/m^2/K]        | for mode 'coefficient' |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | Ta              | Ambient temperature [K]                          | for mode 'coefficient' |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | thicknessLayers | Layer thicknesses [m]                            | no                     |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | kappaLayers     | Layer thermal conductivities [W/m/K]             | no                     |               |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | relaxation      | Relaxation for the wall temperature              | no                     | 1             |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | emissivity      | Surface emissivity for radiative flux to ambient | no                     | 0             |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | qr              | Name of the radiative field                      | no                     | none          |
  +-----------------+--------------------------------------------------+------------------------+---------------+
  | qrRelaxation    | Relaxation factor for radiative field            | no                     | 1             |
  +-----------------+--------------------------------------------------+------------------------+---------------+

Example of the boundary condition specification:  

.. code-block:: c
  
  <patchName>
  {
      type            waterFilmHTC;

      mode            coefficient;

      Ta              constant 300.0;
      htc
      {
          type                table;
          format              foam;
          file                "constant/HTC";
          outOfBounds         clamp;
          interpolationScheme linear;
      }
      thicknessLayers (0.1 0.2 0.3 0.4);
      kappaLayers     (1 2 3 4);

      value           $internalField;
  }
