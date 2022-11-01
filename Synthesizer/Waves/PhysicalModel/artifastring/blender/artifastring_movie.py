#!/usr/bin/env python

##
# Copyright 2010--2013 Graham Percival
# This file is part of Artifastring.
#
# Artifastring is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# Artifastring is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with Artifastring.  If not, see
# <http://www.gnu.org/licenses/>.
##

import optparse
import subprocess
import sys
import os
import glob

def get_options():
    parser = optparse.OptionParser(
        description="Combines a .wav and *.tga into a movie for Artifastring.")
    parser.add_option("-o", "--output",
        default="artifastring-movie.avi",
        help="Output filename")
    parser.add_option("-i", "--images-dir",
        default="/tmp/artifastring/images/",
        help="Directory of images")
    parser.add_option("-l", "--logfile",
        default="artifastring-movie.log",
        help="Log filename (relative to output dir)")
    parser.add_option("--fps",
        metavar="N", default="25",
        help="Frames per second")
    (options, args) = parser.parse_args()
    try:
        options.audio_filename = args[0]
    except IndexError:
        print "Error: must have an audio filename"
        parser.print_help()
        return None
    return options.__dict__

MENCODER_H264 = """mencoder \
"mf://%(images_dir)s/*.%(filename_extension)s" \
-mf fps=%(fps)s:type=%(filename_extension)s \
\
-ovc lavc \
-lavcopts \
vcodec=mpeg4 \
\
-oac mp3lame \
-audiofile %(audio_filename)s \
-o %(output)s
"""

def check_extension(images_dir, extension):
    filenames = glob.glob(os.path.join(images_dir, "*.%s" % extension))
    if len(filenames) > 0:
        return True
    return False

def main():
    options = get_options()
    if not options:
        exit()

    if check_extension(options["images_dir"], "tga"):
        options["filename_extension"] = "tga"
    else:
        if check_extension(options["images_dir"], "png"):
            options["filename_extension"] = "png"
        else:
            print "Error: cannot find images in directory"
            sys.exit(1)

    cmd = MENCODER_H264 % options

    log_filename = options['logfile']
    logfile = open(log_filename, 'w')
    #print cmd
    p = subprocess.Popen(cmd, shell=True, stdout=logfile, stderr=logfile)
    p.wait()
    logfile.close()

main()

