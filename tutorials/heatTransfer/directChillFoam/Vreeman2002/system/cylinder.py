from __future__ import division, print_function

from numpy import sqrt, pi, cos, sin

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2020, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"

def write_vertices(angle=2.5,
                   diameter=155.0,
                   z_points=(-250.0, -50.0, 0.0, 50.0)):
    """
        Vertices
    """
    block = "vertices\n"
    block += "(\n"
    vertex = 0
    radius = diameter/2
    x = radius * cos(pi*angle/180)
    y = radius * sin(pi*angle/180)
    for z in z_points:
        block += "    ({0:7.4f}  {1:7.4f} {2:6.1f}) // {3:2d}\n".format(0, 0, z, vertex)
        vertex += 1
        block += "    ({0:7.4f}  {1:7.4f} {2:6.1f}) // {3:2d}\n".format(x, y, z, vertex)
        vertex += 1
        block += "    ({0:7.4f}  {1:7.4f} {2:6.1f}) // {3:2d}\n".format(x, -y, z, vertex)
        vertex += 1
        if z != z_points[-1]:
            block += "    \n"
    block += ");\n"
    return block

def write_blocks(z_points=(-250.0, -50.0, 0.0, 50.0), diameter=80, mesh=1.0):
    """
        Blocks
    """
    block = "blocks\n"
    block += "(\n"
    prism = 0
    N = 3
    rcells = int(diameter/(2*mesh))
    for i in range(len(z_points)-1):
        cells = int((z_points[i+1]-z_points[i])/mesh)
        block += "    hex ({0:2d} {1:2d} {2:2d} {3:2d} {4:2d} {5:2d} {6:2d} {7:2d}) domain ({8:2d} 1 {9:3d}) simpleGrading (0.5 1 1) // {10:2d}\n".format(0+N*i, 2+N*i, 1+N*i, 0+N*i, 3+N*i, 5+N*i, 4+N*i, 3+N*i, rcells, cells, prism)
        prism += 1
    block += ");\n"
    return block

def write_edges(angle=2.5, diameter=80,
                z_points=(-250.0, -50.0, 0.0, 50.0)):
    """
        Edges
    """
    radius = diameter/2
    x =  radius * cos(pi*angle/(2*180))
    y = -radius * sin(pi*angle/(2*180))
    block = "edges\n"
    block += "(\n"
    N = 3
    for i in range(len(z_points)):
        z = z_points[i]
        block += "    arc {0:2d} {1:2d} ({2:7.4f} {3:7.4f} {4:6.1f})\n".format(1+N*i, 2+N*i, radius, 0, z)
    block += ");\n"
    return block

def write_boundary(z_points=(-250.0, -50.0, 0.0, 50.0)):
    """
        Boundary
    """
    block = "boundary\n"
    block += "(\n"
    faces = ["water-film", "water-film", "air-gap", "mould", "hot-top"]
    N = 3
    for i in range(len(z_points)-1):
        if i != 1:
            block += "    {:s}\n".format(faces[i])
            block += "    {\n"
            block += "        type wall;\n"
            block += "        faces\n"
            block += "        (\n"
        block += "            ({0:2d} {1:2d} {2:2d} {3:2d})\n".format(1+N*i, 2+N*i, 5+N*i, 4+N*i)
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
    block += "            ({0:2d} {1:2d} {2:2d} {3:2d})\n".format(0+N*i, 2+N*i, 1+N*i, 0+N*i)
    block += "        );\n"
    block += "    }\n"
    block += "\n"

    i = len(z_points)-1
    block += "    free-surface\n"
    block += "    {\n"
    block += "        type patch;\n"
    block += "        faces\n"
    block += "        (\n"
    block += "            ({0:2d} {1:2d} {2:2d} {3:2d})\n".format(0+N*i, 2+N*i, 1+N*i, 0+N*i)
    block += "        );\n"
    block += "    }\n"
    block += "\n"

    block += "    axis\n"
    block += "    {\n"
    block += "        type empty;\n"
    block += "        faces\n"
    block += "        (\n"
    for i in range(len(z_points)-1):
        block += "            ({0:2d} {1:2d} {2:2d} {3:2d})\n".format(3+N*i, 0+N*i, 0+N*i, 3+N*i)
    block += "        );\n"
    block += "    }\n"
    block += "\n"

    block += "    symmetry_planes\n"
    block += "    {\n"
    block += "        type symmetry;\n"
    block += "        faces\n"
    block += "        (\n"
    for i in range(len(z_points)-1):
        block += "            ({0:2d} {1:2d} {2:2d} {3:2d})\n".format(0+N*i, 1+N*i, 4+N*i, 3+N*i)
        block += "            ({0:2d} {1:2d} {2:2d} {3:2d})\n".format(5+N*i, 2+N*i, 0+N*i, 3+N*i)
    block += "        );\n"
    block += "    }\n"

    block += ");"

    return block

def write_blockMeshDict(z_points=(-250.0, -50.0, 0.0, 50.0)):
    """
        blockMeshDict
    """
    block = '''/*--------------------------------*- C++ -*----------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Version:  7
     \\/     M anipulation  |
\*---------------------------------------------------------------------------*/
FoamFile
{
    version     2.0;
    format      ascii;
    class       dictionary;
    object      blockMeshDict;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

convertToMeters 0.001;\n\n'''
    block += write_vertices(angle=2.5, diameter=450., z_points=Z_POINTS)
    block += "\n"
    block += write_blocks(z_points=Z_POINTS, mesh=2.0)
    block += "\n"
    block += write_edges(angle=2.5, diameter=450., z_points=Z_POINTS)
    block += '\n'
    block += write_boundary(z_points=Z_POINTS)
    block += '\n\n'
    block += '''mergePatchPairs
(
);


// ************************************************************************* //\n\n'''
    return block

if __name__ == "__main__":
    Z_POINTS = (-900.0, -400.0, -70.0, -60.0, -10.0, 0.0)
    with open('blockMeshDict', 'w') as f:
        f.write(write_blockMeshDict(z_points=Z_POINTS))
