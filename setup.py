from distutils.core import setup, Extension

module = Extension('a0',
                   include_dirs = ['/usr/include'],
                   library_dirs = ['/usr/lib'],
                   libraries = ['alephzero'],
                   sources = ['module.cc'],
                   extra_compile_args = ['-std=c++17', '-D_GLIBCXX_USE_CXX11_ABI=0'])

setup(name = 'AlephZero',
      version = '0.1',
      description = 'TODO: description',
      author = 'Leonid Shamis',
      author_email = 'leonid.shamis@gmail.com',
      url = 'https://github.com/alephzero/py',
      long_description = '''TODO: long description''',
      ext_modules = [module],
      py_modules = ['aio_a0'])
