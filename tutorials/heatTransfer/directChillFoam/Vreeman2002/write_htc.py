#!/usr/bin/env python3
"""Writes HTC table file in foam format.

This script computes the HTC for the water film, including the
effect of nucleate boiling. Outputs the HTC table in foam
format in constant/HTC_T.

Example:
    Run python script in case root directory:

        $ python3 write_htc.py

Todo:
    * None

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

from __future__ import print_function, division

import re

from numpy import array, isnan, abs, linspace, e, power, pi, sqrt
from scipy.interpolate import UnivariateSpline, interp1d

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2022, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def rohsenhow(T=930, n=1.0, g0=1.0, q_water=90.0, D=0.255, T_water=372.8, T_amb=328.5):
    """Rohsenhow correlation.

    :param T: Billet temperature [K].
    :type T: float
    :param n: Experimental constant equal to 1 for water and 1.7 for other fluids.
    :type n: float
    :param g0: Force conversion factor [kg m/N s^2].
    :type g0: float
    :param q_water: Water flow rate [L/min].
    :type q_water: float
    :param D: Diameter [m].
    :type D: float
    :param T_water: Temperature of water jet [K].
    :type T_water: float
    :param T_amb: Temperature of the lab [K].
    :type T_amb: float
    :return: 0 if successful.
    :rtype: int
    """
    mu = 2.829e-4
    k = 0.6790
    rho = 958.6
    g = 9.81
    Cp = 4215.0
    flow_rate = q_water * 0.001 / 60.0
    D = 0.255
    Gamma = flow_rate / (pi * D)
    Hfg = 2.257e6
    sigma = 5.9e-2
    Pr = Cp * mu / k
    C_f = 0.011
    A = power(mu**2 / (k**3 * rho**2 * g), -1 / 3)
    B = power(Pr, 1 / 3)
    C = power(4 * Gamma / mu, 1 / 3)
    h_conv = 0.01 * B * C * A
    q_conv = h_conv * (T - T_amb)
    h = h_conv
    if T > T_water:
        q_inc = 3910 * (T - T_water) ** 2.16
        if q_conv < q_inc:
            q_A = (
                mu
                * Hfg
                * sqrt(g * (rho - 0.5903) / (g0 * sigma))
                * power(Cp * (T - T_water) / (C_f * Hfg * Pr**n), 3)
            )
            h = (q_A + q_conv) / (T - T_amb)
    return h


def weckmann_niessen(T, q_water=90.0, D=0.255, T_water=372.8):
    """Weckmann-Niessen correlation.

    :param T: Billet temperature [K].
    :type T: float
    :param q_water: Water flow rate [L/min].
    :type q_water: float
    :param D: Diameter [m].
    :type D: float
    :param T_water: Temperature of water jet [K].
    :type T_water: float
    :return: 0 if successful.
    :rtype: int
    """
    A = -167000.0
    B = 352
    C = 20.8
    flow_rate = q_water * 0.001 / 60.0
    Gamma = flow_rate / (pi * D)
    T_sat = 373.15
    h = (A + B * (T + T_water)) * power(Gamma, 1.0 / 3) + C * power(T - T_sat, 3) / (
        T - T_water
    )
    return h


def write_HTC_T(htc_file="constant/HTC_T", func=rohsenhow, T_water=372.8):
    """Writes the temperature-HTC foam file using the prescribed formula.

    :param htc_file: Relative path of HTC foam file.
    :type htc_file: str
    :param func: HTC function (Rohsenhow or Weckmann-Niessen).
    :type func: function
    :param T_water: Temperature of water jet [K].
    :type T_water: float
    :return: 0 if successful.
    :rtype: int

    """
    s = "(\n"
    for T in linspace(273, 1053, 200):
        _htc = func(T, T_water=T_water)
        s += f"    ({T:8.4f} {_htc:10.5f})\n"
    s += ")"
    with open(f"{htc_file}", "w") as f:
        f.write(s)
    return 0


if __name__ == "__main__":
    # write_HTC_T(htc_file="constant/HTC_T", func=rohsenhow)
    write_HTC_T(htc_file="constant/HTC_Tw", func=weckmann_niessen, T_water=358.0)
    exit(0)
