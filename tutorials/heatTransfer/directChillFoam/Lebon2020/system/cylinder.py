"""Writes a blockMeshDict file for a three-dimensional case.
This script writes a blockMeshDict file for a three-dimensional case in the system directory.

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


from numpy import pi, cos

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2023, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def write_vertices(
    square=16, diameter=155.0, z_points=(-300.0, -57.0, -40.0, 0.0, 170.0)
):
    """Writes the vertices entry

    :param square: Length of inner square section of mesh.
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
    curve = (diameter / 2) * cos(45.0 * pi / 180.0)
    # Write each vertex coordinate and label the vertex number in a comment at the right
    for z in z_points:
        block += f"    ( {square:16.13f} -{square:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    (-{square:16.13f} -{square:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    (-{square:16.13f}  {square:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    ( {square:16.13f}  {square:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1

        block += "    \n"

        block += f"    ( {curve:16.13f} -{curve:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    (-{curve:16.13f} -{curve:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    (-{curve:16.13f}  {curve:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1
        block += f"    ( {curve:16.13f}  {curve:16.13f} {z:5.1f}) // {vertex:2d}\n"
        vertex += 1

        if z != z_points[-1]:
            block += "    \n"
    block += ");\n"
    return block


def write_blocks(z_points=(-300.0, -57.0, -40.0, 0.0, 170.0), zmesh=1.0):
    """Writes the blocks entry

    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :param zmesh: Vertical mesh adjustment parameter.
    :type zmesh: float
    :return: string containing the blockMeshDict blocks entry
    :rtype: string

    """
    block = "blocks\n"
    block += "(\n"
    vertex = 0
    for i in range(len(z_points) - 1):
        cells = int((z_points[i + 1] - z_points[i]) / zmesh)
        block += "    hex ({:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d}) domain (20 20 {:2d}) simpleGrading (1 1 1) // {:2d}\n".format(
            1 + 8 * i,
            0 + 8 * i,
            3 + 8 * i,
            2 + 8 * i,
            9 + 8 * i,
            8 + 8 * i,
            11 + 8 * i,
            10 + 8 * i,
            cells,
            vertex,
        )
        vertex += 1
        block += "    hex ({:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d}) domain (10 20 {:2d}) simpleGrading (1 1 1) // {:2d}\n".format(
            0 + 8 * i,
            4 + 8 * i,
            7 + 8 * i,
            3 + 8 * i,
            8 + 8 * i,
            12 + 8 * i,
            15 + 8 * i,
            11 + 8 * i,
            cells,
            vertex,
        )
        vertex += 1
        block += "    hex ({:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d}) domain (10 20 {:2d}) simpleGrading (1 1 1) // {:2d}\n".format(
            3 + 8 * i,
            7 + 8 * i,
            6 + 8 * i,
            2 + 8 * i,
            11 + 8 * i,
            15 + 8 * i,
            14 + 8 * i,
            10 + 8 * i,
            cells,
            vertex,
        )
        vertex += 1
        block += "    hex ({:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d}) domain (10 20 {:2d}) simpleGrading (1 1 1) // {:2d}\n".format(
            2 + 8 * i,
            6 + 8 * i,
            5 + 8 * i,
            1 + 8 * i,
            10 + 8 * i,
            14 + 8 * i,
            13 + 8 * i,
            9 + 8 * i,
            cells,
            vertex,
        )
        vertex += 1
        block += "    hex ({:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d} {:2d}) domain (10 20 {:2d}) simpleGrading (1 1 1) // {:2d}\n".format(
            1 + 8 * i,
            5 + 8 * i,
            4 + 8 * i,
            0 + 8 * i,
            9 + 8 * i,
            13 + 8 * i,
            12 + 8 * i,
            8 + 8 * i,
            cells,
            vertex,
        )
        vertex += 1

        if i < len(z_points) - 2:
            block += "    \n"
    block += ");\n"
    return block


def write_edges(
    square_rad=20.0, diameter=155.0, z_points=(-300.0, -57.0, -40.0, 0.0, 170.0)
):
    """Writes the edges entry

    :param square_rad: Radius of inner section curvature.
    :type square_rad: float
    :param diameter: Diameter of the cast billet (mm).
    :type diameter: float
    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :return: string containing the blockMeshDict edges entry
    :rtype: string

    """
    mould_rad = diameter / 2
    zero = 0.0
    block = "edges\n"
    block += "(\n"
    for i in range(len(z_points)):
        z = z_points[i]
        block += "    arc {:2d} {:2d} ({:22.18f} {:22.18f} {:5.1f})\n".format(
            7 + 8 * i, 4 + 8 * i, mould_rad, zero, z
        )
        block += "    arc {:2d} {:2d} ({:22.14e} {:22.18f} {:5.1f})\n".format(
            4 + 8 * i, 5 + 8 * i, zero, -mould_rad, z
        )
        block += "    arc {:2d} {:2d} ({:22.18f} {:22.14e} {:5.1f})\n".format(
            5 + 8 * i, 6 + 8 * i, -mould_rad, zero, z
        )
        block += "    arc {:2d} {:2d} ({:22.18f} {:22.18f} {:5.1f})\n".format(
            6 + 8 * i, 7 + 8 * i, zero, mould_rad, z
        )
        block += "    \n"

    for i in range(len(z_points)):
        z = z_points[i]
        block += "    arc {:2d} {:2d} ({:22.18f} {:22.18f} {:5.1f})\n".format(
            3 + 8 * i, 0 + 8 * i, square_rad, zero, z
        )
        block += "    arc {:2d} {:2d} ({:22.14e} {:22.18f} {:5.1f})\n".format(
            0 + 8 * i, 1 + 8 * i, zero, -square_rad, z
        )
        block += "    arc {:2d} {:2d} ({:22.18f} {:22.14e} {:5.1f})\n".format(
            1 + 8 * i, 2 + 8 * i, -square_rad, zero, z
        )
        block += "    arc {:2d} {:2d} ({:22.18f} {:22.18f} {:5.1f})\n".format(
            2 + 8 * i, 3 + 8 * i, zero, square_rad, z
        )

        if i < len(z_points) - 1:
            block += "    \n"
    block += ");\n"
    return block


def write_boundary(z_points=(-300.0, -57.0, -40.0, 0.0, 170.0)):
    """Writes the boundary entry

    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :return: string containing the blockMeshDict boundary entry
    :rtype: string

    """
    block = "boundary\n"
    block += "(\n"
    faces = ["water-film", "mould", "graphite", "hot-top"]
    for i in range(len(z_points) - 1):
        block += "    {:s}\n".format(faces[i])
        block += "    {\n"
        block += "        type wall;\n"
        block += "        faces\n"
        block += "        (\n"
        block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
            4 + 8 * i, 7 + 8 * i, 15 + 8 * i, 12 + 8 * i
        )
        block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
            5 + 8 * i, 4 + 8 * i, 12 + 8 * i, 13 + 8 * i
        )
        block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
            6 + 8 * i, 5 + 8 * i, 13 + 8 * i, 14 + 8 * i
        )
        block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
            7 + 8 * i, 6 + 8 * i, 14 + 8 * i, 15 + 8 * i
        )
        block += "        );\n"
        block += "    }\n"
        block += "\n"

    i = 0
    block += "    ram\n"
    block += "    {\n"
    block += "        type patch;\n"
    block += "        faces\n"
    block += "        (\n"
    block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
        3 + 8 * i, 0 + 8 * i, 1 + 8 * i, 2 + 8 * i
    )
    block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
        3 + 8 * i, 7 + 8 * i, 4 + 8 * i, 0 + 8 * i
    )
    block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
        2 + 8 * i, 6 + 8 * i, 7 + 8 * i, 3 + 8 * i
    )
    block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
        1 + 8 * i, 5 + 8 * i, 6 + 8 * i, 2 + 8 * i
    )
    block += "            ({:2d} {:2d} {:2d} {:2d})\n".format(
        0 + 8 * i, 4 + 8 * i, 5 + 8 * i, 1 + 8 * i
    )
    block += "        );\n"
    block += "    }\n"
    block += "\n"

    i = len(z_points) - 1
    block += "    free-surface\n"
    block += "    {\n"
    block += "        type patch;\n"
    block += "        faces\n"
    block += "        (\n"
    block += "            ({0:2d} {3:2d} {2:2d} {1:2d})\n".format(
        3 + 8 * i, 2 + 8 * i, 1 + 8 * i, 0 + 8 * i
    )
    block += "            ({0:2d} {3:2d} {2:2d} {1:2d})\n".format(
        3 + 8 * i, 0 + 8 * i, 4 + 8 * i, 7 + 8 * i
    )
    block += "            ({0:2d} {3:2d} {2:2d} {1:2d})\n".format(
        2 + 8 * i, 3 + 8 * i, 7 + 8 * i, 6 + 8 * i
    )
    block += "            ({0:2d} {3:2d} {2:2d} {1:2d})\n".format(
        1 + 8 * i, 2 + 8 * i, 6 + 8 * i, 5 + 8 * i
    )
    block += "            ({0:2d} {3:2d} {2:2d} {1:2d})\n".format(
        0 + 8 * i, 1 + 8 * i, 5 + 8 * i, 4 + 8 * i
    )
    block += "        );\n"
    block += "    }\n"
    block += ");"

    return block


def write_blockMeshDict(
    diameter=155.0,
    z_points=(-300.0, -57.0, -40.0, 0.0, 170.0),
    zmesh=1.0,
):
    """Assembles the blockMeshDict file contents

    :param diameter: Diameter of the cast billet (mm).
    :type diameter: float
    :param z_points: z coordinates of the boundaries of the billet and mould sections.
    :type z_points: tuple
    :return: string containing the blockMeshDict file contents
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
\\*---------------------------------------------------------------------------*/
FoamFile
{
    format          ascii;
    class           dictionary;
    location        "system";
    object          blockMeshDict;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

convertToMeters 0.001;\n\n"""
    block += write_vertices(diameter=diameter, z_points=z_points)
    block += "\n"
    block += write_blocks(z_points=z_points, zmesh=zmesh)
    block += "\n"
    block += write_edges(diameter=diameter, z_points=z_points)
    block += "\n"
    block += write_boundary(z_points=z_points)
    block += "\n\n"
    block += """mergePatchPairs
(
);


// ************************************************************************* //\n"""
    return block


if __name__ == "__main__":
    DIAMETER = 155.0
    ZMESH = 2.0
    Z_POINTS = (-300.0, -57.0, -40.0, 0.0, 170.0)
    with open("blockMeshDict", "w") as f:
        f.write(
            write_blockMeshDict(
                diameter=DIAMETER,
                z_points=Z_POINTS,
                zmesh=ZMESH,
            )
        )
