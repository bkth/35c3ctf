from distutils.core import setup, Extension
setup(name = 'Collection', version = '1.0', ext_modules = [Extension('Collection', ['module.c', 'utils.c', 'types.c', 'sandbox.c'])])
