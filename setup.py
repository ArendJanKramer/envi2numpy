import os

import numpy
from setuptools import setup
from setuptools.extension import Extension

module1 = Extension('envi2numpy',
                    sources=['src/pythonwrapper.cpp', 'src/INIReader.cpp', 'src/ini.c', 'src/cnpy.cpp',
                             'src/envi_parser.cpp', 'src/numpy_helpers.cpp'],
                    include_dirs=['src',
                                  numpy.get_include()],
                    extra_compile_args=['-std=c++11', '-O2'])

this_directory = os.path.abspath(os.path.dirname(__file__))
with open(os.path.join(this_directory, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

setup(name='envi2numpy',
      version='1.3',
      description='Package used for reading and converting hyperspectral ENVI captures to numpy',
      long_description=long_description,
      long_description_content_type='text/markdown',
      ext_modules=[module1],

      url='https://github.com/ArendJanKramer/envi2numpy',

      # Author details
      author='Arend Jan Kramer',
      author_email='github@arendjan.eu',

      # Choose your license
      license='gpl-2.0'
      )
