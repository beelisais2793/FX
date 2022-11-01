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

import bpy

# add modules in the script directory
import sys
import os.path
sys.path.append(os.path.dirname(os.path.realpath(__file__)))


def get_options():
    # get arguments after --
    if '--' in sys.argv:
        custom_argv = sys.argv[sys.argv.index('--')+1: ]
    else:
        custom_argv = []

    import argparse
    parser = argparse.ArgumentParser(
        prog = "artifastring_blender.py",
        description = """Generate a video from an .actions file.
        Options come after -- on the blender command-line.""",
        )

    parser.add_argument('--fps', metavar='N', type=int,
        help="Frames per second", default=25)
    parser.add_argument('--cycle-cameras', metavar='X', type=float,
        help="Cycle cameras every X seconds", default=0.0)
    parser.add_argument('-f', '--filename', metavar='FILENAME',
        help="Filename of .actions", required=True)
    parser.add_argument("-q", "--quality", type=int,
        metavar="N", default=0,
        help="Quality of rending: 0 (terrible) to 2 (best)")

    args = parser.parse_args(custom_argv)
    return args

options = get_options()
    
import violin
import parse_actions
import render_quality

### setup rending variables
render_quality.setup(options.quality, options.fps)

### setup model data
violin = violin.Violin()
#violin.debug()
#violin.bow.debug()

### process actions file
frame_end = parse_actions.load_keyframes(violin, options.filename, options.fps)
bpy.context.scene.frame_end = frame_end

### switch between cameras
if options.cycle_cameras > 0:
    import cameras
    cameras.cycle_cameras(options.cycle_cameras, frame_end, options.fps)


