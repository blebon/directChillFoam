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
    Num_x0mm = read_numerical_dataframe(
        plot_time, probe_position="x0mm", temperature_column_name="tK0mm"
    )
    Num_28mm = read_numerical_dataframe(
        plot_time, probe_position="x28mm", temperature_column_name="tK28mm"
    )
    Num_44mm = read_numerical_dataframe(
        plot_time, probe_position="x44mm", temperature_column_name="tK44mm"
    )
    Num_75mm = read_numerical_dataframe(
        plot_time, probe_position="x75mm", temperature_column_name="tK75mm"
    )

    # Merge dataframes
    Numerical = merge(Num_x0mm, Num_28mm, on="z")
    Numerical = merge(Numerical, Num_44mm, on="z")
    Numerical = merge(Numerical, Num_75mm, on="z")

    # Convert to degrees Celcius
    Numerical["T0mm"] = Numerical["tK0mm"] - 273.15
    Numerical["T28mm"] = Numerical["tK28mm"] - 273.15
    Numerical["T44mm"] = Numerical["tK44mm"] - 273.15
    Numerical["T75mm"] = Numerical["tK75mm"] - 273.15

    # Plot experimental values, masking NaNs
    x0mm_mask = isfinite(Experimental["x0mm"])
    plt.plot(
        -Experimental["x0mm"][x0mm_mask] + offset,
        Experimental["T"][x0mm_mask] - 273.15,
        ".",
        label="x = 0 mm",
    )
    x28mm_mask = isfinite(Experimental["x28mm"])
    plt.plot(
        -Experimental["x28mm"][x28mm_mask] + offset,
        Experimental["T"][x28mm_mask] - 273.15,
        ".",
        label="x = 28 mm",
    )
    x44mm_mask = isfinite(Experimental["x44mm"])
    plt.plot(
        -Experimental["x44mm"][x44mm_mask] + offset,
        Experimental["T"][x44mm_mask] - 273.15,
        ".",
        label="x = 44 mm",
    )
    x75mm_mask = isfinite(Experimental["x75mm"])
    plt.plot(
        -Experimental["x75mm"][x75mm_mask] + offset,
        Experimental["T"][x75mm_mask] - 273.15,
        ".",
        label="x = 75 mm",
    )

    plt.plot(Numerical["z"], Numerical["T0mm"], label="x = 0 mm numerical")
    plt.plot(Numerical["z"], Numerical["T28mm"], label="x = 28 mm numerical")
    plt.plot(Numerical["z"], Numerical["T44mm"], label="x = 44 mm numerical")
    plt.plot(Numerical["z"], Numerical["T75mm"], label="x = 75 mm numerical")

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
        names=["T", "x0mm", "x28mm", "x44mm", "x75mm"],
        skiprows=1,
    )
    plot_line(time, Experimental, ".")
    exit(0)
