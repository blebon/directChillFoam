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
__copyright__ = "Copyright, 2023, Brunel University London"
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


def plot_line(plot_time, Experimental, Image_Path=".", offset=-0.01):
    """Plots temperature graphs at plot_time.

    :param plot_time: Time at which to plot the temperatures.
    :type plot_time: str
    :param Experimental: Dataframe containing experimental data.
    :type Experimental: pandas.Dataframe
    :param Image_Path: Absolute path of line plot.
    :type Image_Path: str
    :param offset: Offset position of mould bottom in experiment.
    :type offset: float
    :return: 0 if successful.
    :rtype: int

    """
    _plot_time_float = float(plot_time)
    print(f"{_plot_time_float:4.1f}")

    # Get numerical temperatures
    Numc = read_numerical_dataframe(
        plot_time, probe_position="x0mm", temperature_column_name="tK0mm"
    )
    Numm = read_numerical_dataframe(
        plot_time, probe_position="x325mm", temperature_column_name="tK325mm"
    )
    Nums = read_numerical_dataframe(
        plot_time, probe_position="x62mm", temperature_column_name="tK62mm"
    )

    # Merge dataframes
    Numerical = merge(Numc, Numm, on="z")
    Numerical = merge(Numerical, Nums, on="z")

    # Convert to degrees Celcius
    Numerical["T0mm"] = Numerical["tK0mm"] - 273.15
    Numerical["T325mm"] = Numerical["tK325mm"] - 273.15
    Numerical["T62mm"] = Numerical["tK62mm"] - 273.15

    # Plot experimental values, masking NaNs
    centre_mask = isfinite(Experimental["x0mm"])
    plt.plot(
        -Experimental["x0mm"][centre_mask] + offset,
        Experimental["T"][centre_mask],
        ".",
        label="x = 0 mm",
    )
    mid_mask = isfinite(Experimental["x325mm_1"])
    plt.plot(
        -Experimental["x325mm_1"][mid_mask] + offset,
        Experimental["T"][mid_mask],
        ".",
        label="x = 32.5 mm (1)",
    )
    mid_mask = isfinite(Experimental["x325mm_2"])
    plt.plot(
        -Experimental["x325mm_2"][mid_mask] + offset,
        Experimental["T"][mid_mask],
        ".",
        label="x = 32.5 mm (2)",
    )
    side_mask = isfinite(Experimental["x62mm_1"])
    plt.plot(
        -Experimental["x62mm_1"][side_mask] + offset,
        Experimental["T"][side_mask],
        ".",
        label="x = 62 mm (1)",
    )
    side_mask = isfinite(Experimental["x62mm_2"])
    plt.plot(
        -Experimental["x62mm_2"][side_mask] + offset,
        Experimental["T"][side_mask],
        ".",
        label="x = 62 mm (2)",
    )

    plt.plot(Numerical["z"], Numerical["T0mm"], label="x = 0 mm numerical")
    plt.plot(Numerical["z"], Numerical["T325mm"], label="x = 32.5 mm numerical")
    plt.plot(Numerical["z"], Numerical["T62mm"], label="x = 62 mm numerical")

    plt.title(f"Time = {_plot_time_float:4.1f} s")
    plt.xlabel("z (m)")
    plt.ylabel("T (C)")
    plt.xlim((-0.01, 0.3))
    plt.legend(loc="best")
    plt.savefig(f"{Image_Path:s}/{_plot_time_float:.1f}.png")
    plt.clf()

    return 0


def get_last_time():
    """Get the last time for the current case."""
    return natsorted(listdir("postProcessing/x0mm"))[-1]


if __name__ == "__main__":
    time = get_last_time()
    Experimental = read_csv(
        "system/experimental.csv",
        names=["T", "x0mm", "x325mm_1", "x325mm_2", "x62mm_1", "x62mm_2"],
        skiprows=1,
    )
    plot_line(time, Experimental, ".")
    exit(0)
