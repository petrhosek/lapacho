"""Setup file for lapacho."""

import os
import sys
from distutils.core import setup, Extension, Command
from distutils.command.build import build
from distutils import log

exts = ['lapachomodule.c', 'hungarian.c']

class TestCommand(Command):
    """Command for running unittests without install."""

    user_options = [("args=", None, '''The command args string passed to
                                    unittest framework, such as 
                                     --args="-v -f"''')]

    def initialize_options(self):
        self.args = ''
        pass

    def finalize_options(self):
        pass

    def run(self):
        self.run_command('build')
        bld = self.distribution.get_command_obj('build')
        #Add build_lib in to sys.path so that unittest can found DLLs and libs
        sys.path = [os.path.abspath(bld.build_lib)] + sys.path

        import shlex
        import unittest
        test_argv0 = [sys.argv[0] + ' test --args=']
        #For transfering args to unittest, we have to split args
        #by ourself, so that command like:
        #python setup.py test --args="-v -f"
        #can be executed, and the parameter '-v -f' can be
        #transfering to unittest properly.
        test_argv = test_argv0 + shlex.split(self.args)
        unittest.main(module=None, defaultTest='test.LapachoTestCase', argv=test_argv)

cmdclass = {'test': TestCommand}

classifiers = [
    "Development Status :: 4 - Beta",
    "Intended Audience :: Developers",
    "License :: OSI Approved :: BSD License",
    "Natural Language :: English",
    "Operating System :: OS Independent",
    "Programming Language :: Python",
    "Topic :: Scientific/Engineering :: Mathematics"],

with open('README.rst') as readme:
    long_description = readme.read()

setup(name='lapacho',
      version='0.1.0',
      platforms=['any'],
      description=("Algorithms for linear assignment problem"),
      url='http://github.com/petrh/lapacho',
      maintainer='Petr Hosek',
      maintainer_email='petrhosek@gmail.com',
      long_description=long_description,
      classifiers=classifiers,
      license='BSD',
      packages=['lapacho'],
      ext_modules=[Extension('_lapacho', exts)],
      cmdclass=cmdclass)
