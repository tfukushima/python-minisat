from setuptools import setup, find_packages
import sys, os

version = '0.1.0'

setup(name='python-minisat',
      version=version,
      description="MiniSat wrapper module for Python",
      long_description="""\
This package is wrapper module of `MiniSat <http://minisat.se/Main.html>`_.""",
      classifiers=[], # Get strings from http://pypi.python.org/pypi?%3Aaction=list_classifiers
      keywords='SAT MiniSat',
      author='Taku Fukushima',
      author_email='f.tac.mac@gmail.com',
      url='http://www.dcl.info.waseda.ac.jp/~tfukushima/',
      license='MIT License',
      packages=find_packages(exclude=['ez_setup', 'examples', 'tests']),
      include_package_data=True,
      zip_safe=False,
      install_requires=[
          # -*- Extra requirements: -*-
      ],
      entry_points="""
      # -*- Entry points: -*-
      """,
      )
