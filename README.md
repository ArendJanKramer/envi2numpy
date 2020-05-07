# envi2numpy - Converting ENVI shots to numpy

Python module for converting hyperspectral ENVI captures to numpy arrays. 
It can also do white/dark balancing.

To install:

```bash
python3 ./setup.py build
python3 ./setup.py install
```

This module can also be installed using pip

```bash
pip3 install envi2numpy
```

## Example
Suppose the following files:

 - DARKREF_EXAMPLE_2020-01-01-10-10-10.hdr
 - DARKREF_EXAMPLE_2020-01-01-10-10-10.log
 - DARKREF_EXAMPLE_2020-01-01-10-10-10.raw
 - EXAMPLE_2020-01-01-10-10-10.hdr
 - EXAMPLE_2020-01-01-10-10-10.log
 - EXAMPLE_2020-01-01-10-10-10.raw
 - WHITEREF_EXAMPLE_2020-01-01-10-10-10.hdr
 - WHITEREF_EXAMPLE_2020-01-01-10-10-10.log
 - WHITEREF_EXAMPLE_2020-01-01-10-10-10.raw

```python
import envi2numpy

# Settings
do_normalize = False # Normalize pixel values between 0.0 and 1.0
do_log_derive = False # Take the logarithmic derivative between the bands

# Parse meta info
hdr_dict = envi2numpy.readHDRfile(cube_hdr_path="EXAMPLE_2020-01-01-10-10-10.hdr")
width = hdr_dict['width']
num_bands = hdr_dict['bands']

numpy_array = envi2numpy.convert(cube_path="EXAMPLE_2020-01-01-10-10-10.raw",
                                  dark_ref_path="DARKREF_EXAMPLE_2020-01-01-10-10-10.raw",
                                  white_ref_path="WHITEREF_EXAMPLE_2020-01-01-10-10-10.raw",
                                  width=width,
                                  num_bands=num_bands,
                                  normalize=do_normalize,
                                  log_derive=do_log_derive

```
