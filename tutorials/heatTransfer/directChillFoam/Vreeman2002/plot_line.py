#!/usr/bin/env python3
"""Plots the temperature profiles at prescribed locations.

This script plots the comparison between numerical and experimental
data. Experimental data in system/validation.dat. Numerical data in
postProcessing directory.

Example:
    Run python script in case root directory:

        $ python3 plot_line.py

Todo:
    * None

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

from __future__ import division, print_function

from os import listdir

# from time import sleep

import matplotlib.pyplot as plt
from natsort import natsorted
from numpy import isfinite
from pandas import read_csv, merge

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2022, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def read_numerical_dataframe(
    plot_time,
    probe_position="centreline",
    temperature_column_name="tcK",
):
    """Read temperature at plot_time and return as dataframe."""
    _df = read_csv(
        f"postProcessing/{probe_position}/{plot_time:s}/line_T.xy",
        header=None,
        delimiter=r"\s+",
    )
    _df[0] *= -1  # z inverted in experiment
    _df.columns = ["z", temperature_column_name]
    return _df


def plot_line(plot_time, Experimental, Image_Path="."):
    """Plots temperature graphs at plot_time.

    :param plot_time: Time at which to plot the temperatures.
    :type plot_time: str
    :param Experimental: Dataframe containing experimental data.
    :type Experimental: pandas.Dataframe
    :param Image_Path: Absolute path of line plot.
    :type Image_Path: str
    :return: 0 if successful.
    :rtype: int

    """
    _plot_time_float = float(plot_time)
    print(f"{_plot_time_float:4.1f}")

    # Get numerical temperatures
    Numc = read_numerical_dataframe(plot_time, probe_position="centreline", temperature_column_name="tcK")
    Numm = read_numerical_dataframe(plot_time, probe_position="midradius", temperature_column_name="tmK")
    Nums = read_numerical_dataframe(plot_time, probe_position="subsurface", temperature_column_name="tsK")
    
    # Merge dataframes
    Numerical = merge(Numc, Numm, on="z")
    Numerical = merge(Numerical, Nums, on="z")

    # Convert to degrees Celcius
    Numerical["tcC"] = Numerical["tcK"] - 273.15
    Numerical["tmC"] = Numerical["tmK"] - 273.15
    Numerical["tsC"] = Numerical["tsK"] - 273.15

    # Plot experimental values, masking NaNs
    centre_mask = isfinite(Experimental["tcC"])
    plt.plot(
        Experimental["z"][centre_mask],
        Experimental["tcC"][centre_mask],
        ".",
        label="Centre measurement",
    )
    mid_mask = isfinite(Experimental["tmC"])
    plt.plot(
        Experimental["z"][mid_mask],
        Experimental["tmC"][mid_mask],
        ".",
        label="Midradius measurement",
    )
    side_mask = isfinite(Experimental["tsC"])
    plt.plot(
        Experimental["z"][side_mask],
        Experimental["tsC"][side_mask],
        ".",
        label="Side measurement",
    )

    plt.plot(Numerical["z"], Numerical["tcC"], label="Centre numerical")
    plt.plot(Numerical["z"], Numerical["tmC"], label="Midradius numerical")
    plt.plot(Numerical["z"], Numerical["tsC"], label="Side numerical")

    plt.title(f"Time = {_plot_time_float:4.1f} s")
    plt.xlabel("z (m)")
    plt.ylabel("T (C)")
    plt.xlim((0, 0.5))
    plt.legend(loc="best")
    plt.savefig(f"{Image_Path:s}/{_plot_time_float:.1f}.png")
    plt.clf()

    return 0


def get_last_time():
    """Get the last time for the current case."""
    return natsorted(listdir("postProcessing/centreline"))[-1]


if __name__ == "__main__":
    time = get_last_time()
    Experimental = read_csv(
        "system/temperature.csv", names=["z", "tcC", "tmC", "tsC"], skiprows=1
    )
    plot_line(time, Experimental, ".")
    exit(0)
