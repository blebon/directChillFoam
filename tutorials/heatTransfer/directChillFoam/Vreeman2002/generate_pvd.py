#!/usr/bin/env python2
"""Generate .pvd XML file to post-process surfaces function
object

This script assembles the VTK files in separate time directories.

Example:
    Run python script in case root directory:

        $ python generate_pvd.py

Todo:
    * None

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

from __future__ import division, print_function

import os
import xml.etree.cElementTree as ET

from natsort import natsorted

def create_xml(variable="U", surface="yNormal", file_format="vtp"):
    """Create <variable>.pvd file with existing VTK files"""
    root = ET.Element(
        "VTKFile",
        type="Collection",
        version="0.1",
        byte_order="LittleEndian",
        compressor="vtkZLibDataCompressor",
    )
    root.text = "\n    "
    collection = ET.SubElement(root, "Collection")
    collection.text = "\n        "
    collection.tail = "\n"
    for time in natsorted(os.listdir(f"postProcessing/{surface}")):
        # print(time)
        try:
            dataset.tail = "\n        "
        except:
            pass
        dataset = ET.SubElement(
            collection,
            "DataSet",
            timestep="{0:s}".format(time),
            group="",
            part="0",
            file=f"{surface}/{time}/{variable}_cutPlane.{file_format}",
        )
    dataset.tail = "\n    "
    tree = ET.ElementTree(root)
    tree.write(
        "postProcessing/{0:s}_{1:s}.pvd".format(variable, surface), xml_declaration=True
    )


if __name__ == "__main__":
    variables = ["grad(T)", "melt1_alpha1", "T", "U"]
    surfaces = ["yNormal"]
    for variable in variables:
        for surface in surfaces:
            create_xml(variable, surface)
