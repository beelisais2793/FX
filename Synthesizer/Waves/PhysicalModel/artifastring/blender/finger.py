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
import abstract_object

import math

class Finger(abstract_object.AbstractObject):
    def __init__(self, violin, string_number):
        abstract_object.AbstractObject.__init__(self)
        self.violin = violin
        self.st = string_number

        self.base_size = 0.01*self.violin.string_length

        self.obj = self.make_finger()
        self.set_visible(True)

    def make_finger(self):
        bpy.ops.mesh.primitive_cone_add(radius=self.base_size,
            depth=3*self.base_size)
        obj = bpy.data.objects["Cone"]
        obj.name = "finger_%i" % self.st
        finger_material = bpy.data.materials.new("finger_material")
        finger_material.diffuse_color = (1.0, 0.5, 0.0)
        obj.active_material = finger_material
        # TODO: bad rotation!
        obj.rotation_euler = math.pi * self.violin.towards_frog
        # parent on violin, due to animation ?
        obj.parent = self.violin.obj
        return obj


    def move(self, finger_position, frame):
        # TODO: iffy?  want to disable linear interpolation for
        # the next keyframe.
        if frame > 0:
            self.obj.keyframe_insert("location", frame=frame-1)
        #
        pos = 1.0 - finger_position
        loc = self.violin.string_contact(self.st, pos)
        loc += 1.5*self.base_size * self.violin.away_from_string
        # set properties and animation
        self.obj.location = loc
        self.obj.keyframe_insert("location", frame=frame)

