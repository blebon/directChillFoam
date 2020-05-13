#!/usr/bin/env python
"""Plot temperature profiles at centre and side

This script plots the comparison between numerical and experimental
data. Experimental data in system/validation.dat. Numerical data in
postProcessing directory.

Example:
    Run python script in case root directory:

        $ python3 system/plot_line.py

Todo:
    * None

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

from __future__ import division, print_function

import multiprocessing as mp
from functools import partial
from os import listdir, path, makedirs
# from time import sleep

import matplotlib.pyplot as plt
import seaborn as sns
from mpi4py import MPI
from natsort import natsorted
from numpy import genfromtxt, isfinite
from pandas import read_csv, merge

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2020, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def plot_single_image(plot_time, Experimental, Image_Path='.'):
    """Plot single image at plot_time"""
    _plot_time_float = float(plot_time)
    print("{0:4.1f}".format(_plot_time_float))

    # Get numerical temperature
    Numc = read_csv("postProcessing/centreline/{:s}/line_T.xy".format(plot_time), header=None, delimiter=r"\s+")
    Numc[0] *= -1                 # z inverted in experiment
    Numc.columns = ["z", "tcK"]
    Numm = read_csv("postProcessing/midradius/{:s}/line_T.xy".format(plot_time), header=None, delimiter=r"\s+")
    Numm[0] *= -1
    Numm.columns = ["z", "tmK"]
    Nums = read_csv("postProcessing/subsurface/{:s}/line_T.xy".format(plot_time), header=None, delimiter=r"\s+")
    Nums[0] *= -1
    Nums.columns = ["z", "tsK"]
    Numerical = merge(Numc, Numm, on="z")
    Numerical = merge(Numerical, Nums, on="z")

    Numerical["tcC"] = Numerical["tcK"] -273.15
    Numerical["tmC"] = Numerical["tmK"] -273.15
    Numerical["tsC"] = Numerical["tsK"] -273.15
    
    centre_mask = isfinite(Experimental["tcC"])
    plt.plot(Experimental["z"][centre_mask], Experimental["tcC"][centre_mask], '.', label="Centre measurement")
    mid_mask = isfinite(Experimental["tmC"])
    plt.plot(Experimental["z"][mid_mask], Experimental["tmC"][mid_mask], '.', label="Midradius measurement")
    side_mask = isfinite(Experimental["tsC"])
    plt.plot(Experimental["z"][side_mask], Experimental["tsC"][side_mask], '.', label="Side measurement")
    
    plt.plot(Numerical["z"], Numerical["tcC"]-27, label="Centre numerical")
    plt.plot(Numerical["z"], Numerical["tmC"], label="Midradius numerical")
    plt.plot(Numerical["z"], Numerical["tsC"], label="Side numerical")

    plt.title("Time = {0:4.1f} s".format(_plot_time_float))
    plt.xlabel("z (m)")
    plt.ylabel("T (C)")
    plt.xlim((0, 0.5))
    plt.legend(loc="best")
    plt.savefig("{0:s}/{1:.1f}.png".format(Image_Path, _plot_time_float))
    plt.clf()

    return 0


def get_last_time():
    """Get last time"""
    return natsorted(listdir("postProcessing/centreline"))[-1]

if __name__ == "__main__":
    time = get_last_time()
    Experimental = read_csv('system/temperature.csv', names=['z', 'tcC', 'tmC', 'tsC'], skiprows=1)
    plot_single_image(time, Experimental, '.')
    exit(0)
