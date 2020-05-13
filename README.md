# directChillFoam

* About OpenFOAM
  OpenFOAM is a free, open source computational fluid dynamics (CFD) software
  package released by the OpenFOAM Foundation. It has a large user base across
  most areas of engineering and science, from both commercial and academic
  organisations. OpenFOAM has an extensive range of features to solve anything
  from complex fluid flows involving chemical reactions, turbulence and heat
  transfer, to solid dynamics and electromagnetics.

* Copyright
  OpenFOAM is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.  See the file [LICENSE](LICENSE) in this directory or
  [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/), for a description of the GNU General Public
  License terms under which you can copy the files.

* Direct-Chill (DC) Casting Application
  This repository contains developments for modeling direct-chill (DC) casting
  by Bruno Lebon at BCAST, Brunel University London using OpenFOAM.

  This work includes a solver based on the continuum mixture theory taken from:

  W.D. Bennon and F.P. Incropera, "A continuum model for momentum, heat and
  species transport in binary solid-liquid phase change systems - 1. Model
  formulation", IJHMT **30** (1987) 2161-2170.

  as well as a test case from:
  C.J. Vreeman, J.D. Schloz and M.J.M. Krane, "Direct Chill Casting of Aluminium
  Alloys: Modelling and Experiments on Industrial Scale Ingots", Journal of Heat
  Transfer **124** (2002) 947-953.

  * [DC Casting solver](applications/solvers/heatTransfer/directChillFoam) - Updated verions of the DC casting solver
  * [Test case](tutorials/heatTransfer/directChillFoam/Vreeman2002) - Tutorial case corresponding to the above reference 
