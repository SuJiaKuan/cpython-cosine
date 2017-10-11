from distutils.core import setup, Extension
import numpy as np

# define the extension module
cos_module = Extension('cos_module', sources=['cos_module.c'],
                        include_dirs=[np.get_include()])

# run the setup
setup(ext_modules=[cos_module])
