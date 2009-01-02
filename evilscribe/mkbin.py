#!python
# python mkbin.py py2exe -c

from distutils.core import setup
import py2exe

setup(name='EvilScribe',
    version='0.3-pre',
    windows=['main.py'])