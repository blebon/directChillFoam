#!/usr/bin/env python3
"""Create axisymmetric block mesh dict file

Example:
    Run python script in system directory:

        $ python3 cylinder.py

Todo:
    * None

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

from __future__ import division, print_function

from numpy import pi, cos, sin

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2023, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def write_vertices(
    angle=2.5, diameter=155.0, z_points=(-300.0, -57.0, -40.0, 0.0, 170.0)
):
    """
    Vertices
    """
    block = "vertices\n"
    block += "(\n"
    vertex = 0
    radius = diameter / 2
    x = radius * cos(pi * angle / 180)
    y = radius * sin(pi * angle / 180)
    for z in z_points:
        block += f"    ({0:7.4f}  {0:7.4f} {z:6.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    ({x:7.4f}  {y:7.4f} {z:6.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    ({x:7.4f}  {-y:7.4f} {z:6.1f}) // {vertex:2d}\n"
        vertex += 1
        if z != z_points[-1]:
            block += "    \n"
    block += ");\n"
    return block


def write_blocks(
    z_points=(-331.0, -71.0, -57.0, -40.0, 0.0, 50.0), diameter=155, mesh=1.0
):
    """
    Blocks
    """
    block = "blocks\n"
    block += "(\n"
    prism = 0
    N = 3
    rcells = int(diameter / (2 * mesh))
    for i in range(len(z_points) - 1):
        cells = int((z_points[i + 1] - z_points[i]) / mesh)
        block += f"    hex ({0+N*i:2d} {2+N*i:2d} {1+N*i:2d} {0+N*i:2d} {3+N*i:2d} {5+N*i:2d} {4+N*i:2d} {3+N*i:2d}) domain ({rcells:2d} 1 {cells:3d}) simpleGrading (0.5 1 1) // {prism:2d}\n"
        prism += 1
    block += ");\n"
    return block


def write_edges(
    angle=2.5, diameter=155, z_points=(-331.0, -71.0, -57.0, -40.0, 0.0, 50.0)
):
    """
    Edges
    """
    radius = diameter / 2
    x = radius * cos(pi * angle / (2 * 180))
    y = -radius * sin(pi * angle / (2 * 180))
    block = "edges\n"
    block += "(\n"
    N = 3
    for i in range(len(z_points)):
        z = z_points[i]
        block += f"    arc {1+N*i:2d} {2+N*i:2d} ({radius:7.4f} {0:7.4f} {z:6.1f})\n"
    block += ");\n"
    return block


def write_boundary(z_points=(-331.0, -71.0, -57.0, -40.0, 0.0, 50.0)):
    """
    Boundary
    """
    block = "boundary\n"
    block += "(\n"
    faces = ["water-film", "air-gap", "mould", "graphite", "hot-top"]
    N = 3
    for i in range(len(z_points) - 1):
        block += f"    {faces[i]:s}\n"
        block += "    {\n"
        block += "        type wall;\n"
        block += "        faces\n"
        block += "        (\n"
        block += f"            ({1+N*i:2d} {2+N*i:2d} {5+N*i:2d} {4+N*i:2d})\n"
        block += "        );\n"
        block += "    }\n"
        block += "\n"

    i = 0
    block += "    ram\n"
    block += "    {\n"
    block += "        type patch;\n"
    block += "        faces\n"
    block += "        (\n"
    block += f"            ({0+N*i:2d} {2+N*i:2d} {1+N*i:2d} {0+N*i:2d})\n"
    block += "        );\n"
    block += "    }\n"
    block += "\n"

    i = len(z_points) - 1
    block += "    free-surface\n"
    block += "    {\n"
    block += "        type patch;\n"
    block += "        faces\n"
    block += "        (\n"
    block += f"            ({0+N*i:2d} {2+N*i:2d} {1+N*i:2d} {0+N*i:2d})\n"
    block += "        );\n"
    block += "    }\n"
    block += "\n"

    block += "    axis\n"
    block += "    {\n"
    block += "        type empty;\n"
    block += "        faces\n"
    block += "        (\n"
    for i in range(len(z_points) - 1):
        block += f"            ({3+N*i:2d} {0+N*i:2d} {0+N*i:2d} {3+N*i:2d})\n"
    block += "        );\n"
    block += "    }\n"
    block += "\n"

    block += "    symmetry_planes\n"
    block += "    {\n"
    block += "        type symmetry;\n"
    block += "        faces\n"
    block += "        (\n"
    for i in range(len(z_points) - 1):
        block += f"            ({0+N*i:2d} {1+N*i:2d} {4+N*i:2d} {3+N*i:2d})\n"
        block += f"            ({5+N*i:2d} {2+N*i:2d} {0+N*i:2d} {3+N*i:2d})\n"
    block += "        );\n"
    block += "    }\n"

    block += ");"

    return block


def write_blockMeshDict(z_points=(-331.0, -71.0, -57.0, -40.0, 0.0, 50.0)):
    """
    blockMeshDict
    """
    block = """/*--------------------------------*- C++ -*----------------------------------*\\
  =========                 |
  \\\\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\\\    /   O peration     | Website:  https://openfoam.org
    \\\\  /    A nd           | Version:  9
     \\\\/     M anipulation  |
\*---------------------------------------------------------------------------*/
FoamFile
{
    format          ascii;
    class           dictionary;
    location        "system";
    object          blockMeshDict;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

convertToMeters 0.001;\n\n"""
    block += write_vertices(angle=2.5, diameter=155.0, z_points=Z_POINTS)
    block += "\n"
    block += write_blocks(z_points=Z_POINTS, mesh=2.0)
    block += "\n"
    block += write_edges(angle=2.5, diameter=155.0, z_points=Z_POINTS)
    block += "\n"
    block += write_boundary(z_points=Z_POINTS)
    block += "\n\n"
    block += """mergePatchPairs
(
);


// ************************************************************************* //\n\n"""
    return block


if __name__ == "__main__":
    Z_POINTS = (-300.0, -57.0, -50, -40.0, 0.0, 170.0)
    with open("blockMeshDict", "w") as f:
        f.write(write_blockMeshDict(z_points=Z_POINTS))
