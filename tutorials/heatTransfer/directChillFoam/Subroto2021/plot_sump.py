#!/usr/bin/env python3
"""Plots the sump profile.

This script plots the sump profile using the output in the
postProcessing directory.

Example:
    Run python script in case root directory:

        $ python3 plot_sump.py

Todo:
    * None

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

from __future__ import division, print_function

from os import listdir, path, sep
from natsort import natsorted, ns

import vtk
from vtk.numpy_interface import dataset_adapter as dsa
from vtk.util.numpy_support import vtk_to_numpy

import matplotlib.tri as tr
import matplotlib.pyplot as plt

from natsort import natsorted, ns
from numpy import (
    linspace,
    sqrt,
    around,
    where,
)

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2022, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def load_slice(filename):
    """Read field from .vtp file.

    :param filename: Filename of .vtp file, including path.
    :type filename: str
    :return: A tuple containing points, triangulation and scalar field.
    :rtype: tuple[numpy.array]

    """
    if not path.exists(filename):
        return None
    reader = vtk.vtkXMLPolyDataReader()
    reader.SetFileName(filename)
    reader.Update()

    data = reader.GetOutput()
    arrays = dsa.WrapDataObject(data)

    # Extracting triangular information
    cells = arrays.GetPolys()
    triangles = vtk_to_numpy(cells.GetData())

    ntri = cells.GetNumberOfCells()
    tris = [None] * ntri
    tri = [None] * ntri

    j = 0
    offset = 0.1

    for _ in range(ntri):
        points = triangles[j]
        tris[_] = list(triangles[j : j + points + 1])

        tri[_ + offset] = list([triangles[j + 1], triangles[j + 2], triangles[j + 3]])

        if points == 4:
            offset += 1
            tri.insert(
                _ + offset, list([triangles[j + 3], triangles[j + 4], triangles[j + 1]])
            )

        j += points + 1

    # Extracting points
    points = vtk_to_numpy(arrays.GetPoints())
    x, y, z = points.T

    # Extract data
    sc = vtk_to_numpy(arrays.GetPointData().GetArray(0))

    if any([i in filename for i in ["U", "grad(T)"]]):
        return (x, y, z, tri, sc.T)
    else:
        return (x, y, z, tri, sc)


Ucast = 0.00233  # Casting speed in m/s
offset = 0  # Set to mould position

T_sol = 805.503  # Solidus temperature in K
T_liq = 928.236  # Liquidus temperature in K


def plot_sump(image_name="sump.png", cmap="jet"):
    """Plots sump contour.

    :param image_name: Filename of image, including path.
    :type image_name: str
    :param cmap: Color map for plotting contour.
    :type cmap: str or matplotlib.colormaps
    :return: 0 if successful.
    :rtype: int

    """
    plt.clf()
    # plt.rc('text', usetex=True)
    fig, ax = plt.subplots(nrows=1, ncols=1)

    casedir = f"postProcessing{sep}yNormal"
    times = natsorted(listdir(casedir), alg=ns.FLOAT)
    time = times[-1]
    suffix = "cutPlane"

    # fl
    vtkFile = f"{casedir}/{time:d}/melt1_alpha1_{suffix}.vtp"
    x, y, z, tri, fL = load_slice(vtkFile)
    levels = linspace(0, 1, 11)
    levels = around(levels, decimals=1)
    lhs_triang = tr.Triangulation(-x, z, triangles=tri)
    rhs_triang = tr.Triangulation(x, z, triangles=tri)
    ax.tricontourf(lhs_triang, fL, cmap="gray", levels=levels)
    cs = ax.tricontourf(rhs_triang, fL, cmap="gray", levels=levels)

    # T
    vtkFile = f"{casedir}/{time}/T_{suffix}.vtp"
    x, y, z, tri, scalar = load_slice(vtkFile)
    scalar -= 273.13
    levels = linspace(T_sol, T_liq, 10)
    levels -= 273.13
    levels = around(levels, decimals=1)
    ax.tricontourf(lhs_triang, scalar, cmap=cmap, levels=levels)
    cs = ax.tricontourf(rhs_triang, scalar, cmap=cmap, levels=levels)
    handles, labels = cs.legend_elements(variable_name="T")
    labels[0] = "$T \\leq {0:s}$".format(labels[1].split(" ")[0].replace("$", ""))
    labels[-1] = "$T > {0:s}$".format(labels[-2].split(" ")[-1].replace("$", ""))
    lgd1 = ax.legend(
        handles,
        labels,
        title=r"$T$ $^{\circ}$C",
        loc="center",
        bbox_to_anchor=(1.25, 0.5),
    )

    # U
    vtkFile = f"{casedir}/{time}/U_{suffix}.vtp"
    x, y, z, tri, (Ux, Uy, Uz) = load_slice(vtkFile)
    scalar = sqrt(Ux**2 + Uy**2 + Uz**2)
    Uz += Ucast

    umask = where(fL > 0.3, True, False)

    SKIP = 15
    quiveropts = dict(pivot="middle", scale=0.5, units="xy", angles="xy")
    ax.quiver(
        -x[umask][::SKIP],
        z[umask][::SKIP],
        -Ux[umask][::SKIP],
        Uz[umask][::SKIP],
        **quiveropts,
    )
    q = ax.quiver(
        x[umask][::SKIP],
        z[umask][::SKIP],
        Ux[umask][::SKIP],
        Uz[umask][::SKIP],
        **quiveropts,
    )
    p = ax.quiverkey(
        q,
        0.15,
        -0.30,
        1e-2,
        "DC: 1 cm s$^{-1}$",
        coordinates="data",
        labelpos="N",
        labelsep=0.1,
        labelcolor="w",
        color="w",
    )

    ratio = 0.9
    ax.set_ylim([-0.13, 0.01])
    ax.set_aspect(1.0 * ratio)

    ax.tick_params(direction="out", which="both")
    ax.minorticks_on()

    fig.set_size_inches(6, 4)
    plt.savefig(
        image_name,
        transparent=False,
        dpi=1000,
        bbox_extra_artists=(lgd1, p),
        bbox_inches="tight",
    )

    return 0


if __name__ == "__main__":
    plot_sump(image_name="Sump.png", cmap="jet")
