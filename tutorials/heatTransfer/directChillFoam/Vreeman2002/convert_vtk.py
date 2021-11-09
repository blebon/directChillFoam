from __future__ import division, print_function

from itertools import chain
import multiprocessing
from os import listdir, makedirs, path, remove, walk
from natsort import natsorted

import vtk

__author__ = "Bruno Lebon"
__copyright__ = "Copyright, 2021, Brunel University London"
__credits__ = ["Bruno Lebon"]
__email__ = "Bruno.Lebon@brunel.ac.uk"
__status__ = "Production"


def vtk2vtp(invtkfile, outvtpfile):
    """Convert vtk to vtp"""
    reader = vtk.vtkPolyDataReader()
    reader.SetFileName(invtkfile)
    reader.Update()
    writer = vtk.vtkXMLPolyDataWriter()
    writer.SetFileName(outvtpfile)
    # writer.SetFileTypeToBinary()
    writer.SetDataModeToBinary()
    writer.SetInputData(reader.GetOutput())
    writer.Update()
    # writer.Write()


def convert_surfaces(vtkFile):
    """Convert vtk to vtp"""
    vtpFile = vtkFile.replace(".vtk", ".vtp")
    if path.exists(vtkFile) and not path.exists(vtpFile):
        print(vtkFile)
        vtk2vtp(vtkFile, vtpFile)
    if path.exists(vtkFile) and path.exists(vtpFile):
        remove(vtkFile)


if __name__ == "__main__":
    with multiprocessing.Pool(8) as pool:
        Walk = walk("postProcessing")
        files_gen = chain.from_iterable(
            (path.join(root, f) for f in files if ".vtk" in f)
            for root, dirs, files in Walk
        )
        # pool.map(print, files_gen)
        pool.map(convert_surfaces, natsorted(files_gen))
