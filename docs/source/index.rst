Welcome to directChillFoam's documentation!
===========================================

directChillFoam is an OpenFOAM implementation of the `continuum mixture model <https://doi.org/10.1016/0017-9310(87)90094-9>`_ for solving direct chill (DC) casting problems using the finite volume method.

You will find the application source code on `Github <https://github.com/blebon/directChillFoam/blob/master/applications/solvers/heatTransfer/directChillFoam>`_. Associated libraries are also available on the repository.

A tutorial case corresponding to an experiment from C.J. Vreeman, J.D. Schloz and M.J.M. Krane, "Direct Chill Casting of Aluminium Alloys: Modelling and Experiments on Industrial Scale Ingots", *Journal of Heat Transfer* **124** (2002) 947-953 `doi:10.1115/1.1482089 <https://doi.org/10.1115/1.1482089>`_ is found `here <https://github.com/blebon/directChillFoam/blob/master/tutorials/heatTransfer/directChillFoam/Vreeman2002>`_ and documented on :doc:`tutorials/Vreeman2002`.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   solver/directChillFoam
   src/soluteModel
   src/multicomponentAlloy
   src/ThermophysicalTransportModels
   src/fvConstraints
   src/fvModels
   tutorials/Vreeman2002
   tutorials/Lebon2020
   tutorials/Subroto2021
   tutorials/modules
    

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
