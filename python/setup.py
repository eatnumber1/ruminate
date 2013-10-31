from distutils.core import setup
import glob, os

setup(name='RuminateBackend',
	version='0.0.1',
	py_modules=map(lambda s: s[:-3], glob.glob(os.path.join('*.py'))),
	scripts=['ruminate-dbgsvr'],
	packages=['RuminateBackend']
)
