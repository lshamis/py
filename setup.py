import glob
from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension

sources = ['module.cc']
for directory in ['./alephzero/include', './alephzero/include/a0', './alephzero/src']:
    for extension in ['c', 'cpp', 'h', 'hpp']:
        sources += glob.glob(f'{directory}/*.{extension}')

module = Pybind11Extension('alephzero_bindings',
                           sources=sources,
                           include_dirs=['./alephzero/include/'],
                           extra_compile_args=['-std=c++17', '-O3'])

setup(name='alephzero',
      version='0.2.3',
      description='TODO: description',
      author='Leonid Shamis',
      author_email='leonid.shamis@gmail.com',
      url='https://github.com/alephzero/py',
      long_description='''TODO: long description''',
      ext_modules=[module],
      py_modules=['a0'])
