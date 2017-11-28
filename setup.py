from setuptools import setup
from setuptools.extension import Extension

import os

module1 = Extension('envi2numpy',
                    sources = ['src/pythonwrapper.cpp', 'src/INIReader.cpp','src/ini.c','src/cnpy.cpp','src/envi_parser.cpp'],
                    include_dirs = [],
		extra_compile_args=['-std=c++11','-O2'])

setup (name = 'envi2numpy',
       version = '1.1',
       description = 'Package used for reading hyperspectral captures',
       ext_modules = [module1],

    url='https://github.com/ArendJanKramer/envi2numpy',

    # Author details
    author='Arend Jan Kramer',
    author_email='github@arendjan.eu',

    # Choose your license
    license='MIT'
)

