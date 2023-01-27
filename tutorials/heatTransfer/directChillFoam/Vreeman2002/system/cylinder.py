"""Writes a blockMeshDict file for an axisymmetric case.
This script writes a blockMeshDict file for an axisymmetric case in the system directory.

Refer to https://doc.cfd.direct/openfoam/user-guide-v6/blockmesh for the official blockMesh utility documentation.

Example:
    Run python script in case system directory:
        $ cd system
        $ python convert_vtk.py
Todo:
    * None
.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html
   
"""

from __future__ import division, print_function

from numpy import pi, cos, sin

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2021, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def write_vertices(angle=2.5, diameter=155.0, z_points=(-250.0, -50.0, 0.0, 50.0)):
    """Writes the vertices entry

    :param angle: Angle of the axisymmetric slice.
    :type angle: float
    :param diameter: Diameter of the cast billet (mm).
    :type diameter: float
    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :return: string containing the blockMeshDict vertices entry
    :rtype: string

    """
    block = "vertices\n"
    block += "(\n"
    vertex = 0
    radius = diameter / 2
    x = radius * cos(pi * angle / 180)
    y = radius * sin(pi * angle / 180)
    # Write each vertex coordinate and label the vertex number in a comment at the right
    for z in z_points:
        block += f"    ({0:8.4f}  {0:7.4f} {z:6.1f}) // {vertex:2d}\n"
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
    diameter=155.0, z_points=(-250.0, -50.0, 0.0, 50.0), rmesh=1.0, zmesh=1.0
):
    """Writes the blocks entry

    :param diameter: Diameter of the cast billet (mm).
    :type diameter: float
    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :param rmesh: Radial mesh adjustment parameter.
    :type rmesh: float
    :param zmesh: Vertical mesh adjustment parameter.
    :type zmesh: float
    :return: string containing the blockMeshDict blocks entry
    :rtype: string

    """
    block = "blocks\n"
    block += "(\n"
    prism = 0
    N = 3
    rcells = int(diameter / (2 * rmesh))
    for i in range(len(z_points) - 1):
        z_expansion_ratio = 1.0
        cells = int((z_points[i + 1] - z_points[i]) / zmesh)
        if i == 0:
            # Coarser mesh when billet is solid
            z_expansion_ratio = 0.7
            cells //= 5
        block += f"    hex ({0+N*i:2d} {2+N*i:2d} {1+N*i:2d} {0+N*i:2d} {3+N*i:2d} {5+N*i:2d} {4+N*i:2d} {3+N*i:2d}) domain ({rcells:2d} 1 {cells:3d}) simpleGrading (0.5 1 {z_expansion_ratio:.1f}) // {prism:2d}\n"
        prism += 1
    block += ");\n"
    return block


def write_edges(angle=2.5, diameter=155.0, z_points=(-250.0, -50.0, 0.0, 50.0)):
    """Writes the edges entry

    :param angle: Angle of the axisymmetric slice.
    :type angle: float
    :param diameter: Diameter of the cast billet (mm).
    :type diameter: float
    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :return: string containing the blockMeshDict edges entry
    :rtype: string

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


def write_boundary(z_points=(-250.0, -50.0, 0.0, 50.0)):
    """Writes the boundary entry

    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :return: string containing the blockMeshDict boundary entry
    :rtype: string

    """
    block = "boundary\n"
    block += "(\n"
    # face names used in 0 directory
    faces = ["water-film", "water-film", "air-gap", "mould", "hot-top"]
    N = 3
    for i in range(len(z_points) - 1):
        if i != 1:
            block += f"    {faces[i]:s}\n"
            block += "    {\n"
            block += "        type wall;\n"
            block += "        faces\n"
            block += "        (\n"
        block += f"            ({1+N*i:2d} {2+N*i:2d} {5+N*i:2d} {4+N*i:2d})\n"
        if i != 0:
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


def write_blockMeshDict(
    angle=2.5, diameter=155.0, z_points=(-250.0, -50.0, 0.0, 50.0), rmesh=1.0, zmesh=1.0
):
    """Assembles the blockMeshDict file contents

    :param angle: Angle of the axisymmetric slice.
    :type angle: float
    :param diameter: Diameter of the cast billet (mm).
    :type diameter: float
    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :return: string containing the blockMeshDict file contents
    :param rmesh: Radial mesh adjustment parameter.
    :type rmesh: float
    :param zmesh: Vertical mesh adjustment parameter.
    :type zmesh: float
    :rtype: string

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
    version     2.0;
    format      ascii;
    class       dictionary;
    object      blockMeshDict;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

convertToMeters 0.001;\n\n"""
    block += write_vertices(angle=angle, diameter=diameter, z_points=z_points)
    block += "\n"
    block += write_blocks(
        diameter=diameter, z_points=z_points, rmesh=rmesh, zmesh=zmesh
    )
    block += "\n"
    block += write_edges(angle=angle, diameter=diameter, z_points=z_points)
    block += "\n"
    block += write_boundary(z_points=z_points)
    block += "\n\n"
    block += """mergePatchPairs
(
);


// ************************************************************************* //\n\n"""
    return block


if __name__ == "__main__":
    ANGLE = 2.5
    DIAMETER = 450.0
    RMESH = 5.5
    ZMESH = 2.0
    Z_POINTS = (-900.0, -400.0, -70.0, -40.0, -10.0, 0.0)
    with open("blockMeshDict", "w") as f:
        f.write(
            write_blockMeshDict(
                angle=ANGLE,
                diameter=DIAMETER,
                z_points=Z_POINTS,
                rmesh=RMESH,
                zmesh=ZMESH,
            )
        )
