try:
    from setuptools import setup, find_packages, Extension
except ImportError:
    from distutils.core import setup, find_packages, Extension

import sys, os

version = '0.1.0'
library_dirs = ['/usr/local/lib']

MINISAT_ROOT = os.path.join(os.path.dirname(__file__), 'minisat/src')
MINISAT_SOURCE_ROOT = os.path.join(os.path.dirname(__file__), 'minisat/src/minisat')
include_dirs = []
include_dirs += ['/usr/include', '/usr/local/include']
include_dirs.append(MINISAT_ROOT)
include_dirs.append(MINISAT_SOURCE_ROOT)
include_dirs += filter(os.path.isdir,
                       [ os.path.join(MINISAT_SOURCE_ROOT, d)
                         for d in os.listdir(MINISAT_SOURCE_ROOT) ])

setup(name='python-minisat',
      version=version,
      description="MiniSat wrapper module for Python",
      long_description="""\
This package is wrapper module of `MiniSat <http://minisat.se/Main.html>`_.""",
      classifiers=[
        "Programming Language :: C",
        "Programming Language :: C++",
        "Programming Language :: Python",
        "Topic :: Software Development :: Libraries :: Python Modules"
      ], # Get strings from http://pypi.python.org/pypi?%3Aaction=list_classifiers
      keywords='SAT MiniSat',
      author='Taku Fukushima',
      author_email='f.tac.mac@gmail.com',
      url='http://www.dcl.info.waseda.ac.jp/~tfukushima/',
      license='MIT License',
      packages=find_packages(exclude=['ez_setup', 'examples', 'tests']),
      include_package_data=True,
      zip_safe=False,
      ext_modules = [Extension("minisat",
                               sources=["minisat/src/minisat/core/Solver.cc",
                                        "minisat/MiniSatWrapper.cc",
                                        "minisat/minisat.c",
                                        ],
                               define_macros = [('__STDC_LIMIT_MACROS', None),
                                                ('__STDC_FORMAT_MACROS', None)],
                               #extra_compile_args = ["-Wall", "-Wno-parenthesis",
                               #                      "-Wextra"],
                               extra_compile_args = [],
                               #extra_link_args = ["-Wall", "-lz"],
                               extra_link_args = ["-lz"],
                               include_dirs=include_dirs,
                               library_dirs=library_dirs,
                               libraries = ["stdc++"],
                               )],
      install_requires=[
          # -*- Extra requirements: -*-
      ],
      entry_points="""
      # -*- Entry points: -*-
      """,
      )
