#!/usr/bin/python3

# Copyright (c) 2021 Ben Ashbaugh
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import gen

from mako.template import Template
from mako.exceptions import RichTraceback

from collections import OrderedDict
from collections import namedtuple

import argparse
import sys
import urllib
import xml.etree.ElementTree as etree
import urllib.request

# parse_xml - Helper function to parse the XML file from a URL or local file.
def parse_xml(path):
    file = urllib.request.urlopen(path) if path.startswith("http") else open(path, 'r')
    with file:
        tree = etree.parse(file)
        return tree

# noneStr - returns string argument, or "" if argument is None.
def noneStr(s):
    if s:
        return s
    return ""

if __name__ == "__main__":
    args = gen.parse_args()
    spec = gen.load_spec(args)

    apisigs = gen.get_apisigs(spec)

    extapis = gen.get_extapis(spec, apisigs)

    try:
        mako_template = Template(filename='openclext.cpp.mako')

        print('Generating openclext.cpp...')
        gen = open(args.directory + '/openclext.cpp', 'wb')
        gen.write(
          mako_template.render_unicode(
              genExtensions={
                  'cl_khr_create_command_queue',
                  'cl_khr_spir',
                  'cl_intel_unified_shared_memory'
              },
              includes='#include <CL/cl_ext.h>',
              spec=spec,
              apisigs=apisigs,
              extapis=extapis).
          encode('utf-8', 'replace'))

    except:
        traceback = RichTraceback()
        for (filename, lineno, function, line) in traceback.traceback:
            print('%s(%s) : error in %s' % (filename, lineno, function))
            print('    ', line)
        print('%s: %s' % (str(traceback.error.__class__.__name__), traceback.error))
