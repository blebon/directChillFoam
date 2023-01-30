# directChillFoam

<!-- badges: start -->
[![status](https://joss.theoj.org/papers/71097ad0617d8fd3da4c5196877ccb73/status.svg)](https://joss.theoj.org/papers/71097ad0617d8fd3da4c5196877ccb73)
[![DOI](https://zenodo.org/badge/263608946.svg)](https://zenodo.org/badge/latestdoi/263608946)
[![docs](https://img.shields.io/readthedocs/d?color=green)](https://blebon.com/directChillFoam)
<!-- badges: end -->

## About OpenFOAM  

  OpenFOAM is a free, open source computational fluid dynamics (CFD) software
  package released by the OpenFOAM Foundation. It has a large user base across
  most areas of engineering and science, from both commercial and academic
  organisations. OpenFOAM has an extensive range of features to solve anything
  from complex fluid flows involving chemical reactions, turbulence and heat
  transfer, to solid dynamics and electromagnetics.

## Copyright  

  OpenFOAM is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.  See the file [LICENSE](LICENSE) in this directory or
  [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/), for a description of the GNU General Public
  License terms under which you can copy the files.

## Contributing to directChillFoam

  Contributions to directChillFoam are welcome. To contribute, create a branch with a [pull request](https://github.com/blebon/directChillFoam/pulls), make sure that your changes tick all the checkboxes in the [pull request template](https://github.com/blebon/directChillFoam/blob/master/.github/PULL_REQUEST_TEMPLATE/pull_request_template.md) and add a clear description of your proposed change.

## Bug reports

  If you discover a bug, open a new [issue](https://github.com/blebon/directChillFoam/issues/new) and complete all the requested sections from the [issue template](https://github.com/blebon/directChillFoam/blob/master/.github/ISSUE_TEMPLATE/bug_report.md).

## Direct-Chill (DC) Casting Application  

  This repository contains developments for modeling direct-chill (DC) casting
  by Bruno Lebon at BCAST, Brunel University London using OpenFOAM.

  This work includes a solver based on the continuum mixture theory taken from:

  W.D. Bennon and F.P. Incropera, "A continuum model for momentum, heat and
  species transport in binary solid-liquid phase change systems - 1. Model
  formulation", IJHMT **30** (1987) 2161-2170.

  as well as test cases from:
  
  1. C.J. Vreeman, J.D. Schloz and M.J.M. Krane, "Direct Chill Casting of Aluminium
  Alloys: Modelling and Experiments on Industrial Scale Ingots", Journal of Heat
  Transfer **124** (2002) 947-953. <https://doi.org/10.1115/1.1482089>
  2. B. Lebon, H.T. Li, J.B. Patel, H. Assadi and Z.Fan, "Numerical modelling of melt-conditioned direct-chill casting", Applied Mathematical Modelling **77** (2020) 1310-1330. <https://doi.org/10.1016/j.apm.2019.08.032>
  3. T. Subroto, B. Lebon, D. Eskin, I. Skalicky, D. Roberts, I. Tzanakis and et al., "Numerical modelling and experimental validation of the effect of ultrasonic melt treatment in a direct-chill cast AA6008 alloy billet", Journal of Materials Research and Technology **12** (2021) 1582-1596. <https://doi.org/10.1016/j.jmrt.2021.03.061>

* [DC Casting solver](applications/solvers/heatTransfer/directChillFoam) - Updated verions of the DC casting solver
* [Vreeman2002](tutorials/heatTransfer/directChillFoam/Vreeman2002) - Tutorial case corresponding to Vreeman *et al.* (2002).
* [Lebon2020](tutorials/heatTransfer/directChillFoam/Lebon2020) - Tutorial case corresponding to Lebon *et al.* (2020).
* [Subroto2021](tutorials/heatTransfer/directChillFoam/Subroto2021) - Tutorial case corresponding to Subroto *et al.* (2021).
* [Documentation](https://blebon.com/directChillFoam/) - Documentation
