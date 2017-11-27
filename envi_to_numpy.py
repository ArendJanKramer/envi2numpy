import envi2numpy
import os
import numpy as np


def processCapture(cube, dark, white, cubehdr, npyfile, npyfilenorm):
    print("Processing ", os.path.basename(cube))
    hdr_dict = envi2numpy.readHDRfile(cubehdr)
    print(hdr_dict)

    width = hdr_dict['width']
    numbands = hdr_dict['bands']

    if True:
        if envi2numpy.convertCapture(cube, dark, white, npyfile, width, numbands):
            print("Converted!!")
        else:
            print("Failed to convert")

    if True:
        if envi2numpy.convertAndNormalizeCapture(cube, dark, white, npyfilenorm, width, numbands):
            print("Converted normalized!!")
        else:
            print("Failed to convert to normalized")


def folderContainsFile(filenames, root, prefix, suffix):
    for file in filenames:
        if file.startswith(prefix) and file.endswith(suffix):
            return os.path.join(root, file)
    return False


workDir = os.getcwd()
#workDir = '/media/public_data/datasets/Tomatoes/'

for root, subFolders, files in os.walk(os.path.join(workDir)): #, "Scans van plastics/Zwarte achtergrond/")):
    cubefile = folderContainsFile(files, root, 'NHL_', '.raw')
    dark = folderContainsFile(files, root, 'DARKREF_', '.raw')
    white = folderContainsFile(files, root, 'WHITEREF_', '.raw')
    hdr = folderContainsFile(files, root, 'NHL_', '.hdr')

    if cubefile and dark and white and hdr:
        npyfile = os.path.abspath(os.path.join(root, os.pardir, "cube.npy"))
        npyfilenorm = os.path.abspath(os.path.join(root, os.pardir, "cube-norm.npy"))
    #print(npyfile)
        processCapture(cube=cubefile, dark=dark, white=white, cubehdr=hdr, npyfile=npyfile, npyfilenorm=npyfilenorm)
