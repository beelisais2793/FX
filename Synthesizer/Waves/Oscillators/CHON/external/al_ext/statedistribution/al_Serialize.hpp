#ifndef AL_SERIALIZE_HPP
#define AL_SERIALIZE_HPP

/*	Allolib --
        Multimedia / virtual environment application class library

    Copyright (C) 2009. AlloSphere Research Group, Media Arts & Technology,
    UCSB. Copyright (C) 2012-2020. The Regents of the University of California.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the distribution.

    Neither the name of the University of California nor the names
    of its contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    File description:
    Dynamic Scene Manager

    File author(s):
    Andrés Cabrera mantaraya36@gmail.com

*/

#include "al/graphics/al_Mesh.hpp"

#include <cstring> // for memcpy in serialize() and deserialize()

namespace al {
namespace ser {

/**
 * @brief serialize mesh data
 * @param mesh
 * @param meshData
 * @param numVertices
 * @param numIndices
 * @param numColors
 *
 * You should check before calling this function that there
 * The binary format for serialization is bound to change, so don't rely on
 * it.
 */
static void serializeMesh(Mesh &mesh, char *meshData, size_t &numVertices,
                          size_t &numIndices, size_t &numColors,
                          size_t meshDataSize) {
  size_t numBytes = (mesh.vertices().size() * 3 * sizeof(float)) +
                    (mesh.indices().size() * sizeof(unsigned int)) +
                    (mesh.colors().size() * 4 * sizeof(float));
  if (numBytes > meshDataSize) {
    numVertices = numIndices = numColors = 0;
    return;
  }
  numVertices = mesh.vertices().size();
  for (auto vertex : mesh.vertices()) {
    memcpy(meshData, vertex.elems(), 3 * sizeof(float));
    meshData += 3 * sizeof(float);
  }
  numIndices = mesh.indices().size();
  for (auto index : mesh.indices()) {
    memcpy(meshData, &index, sizeof(unsigned int));
    *meshData++ = sizeof(unsigned int);
  }
  numColors = mesh.colors().size();
  for (auto color : mesh.colors()) {

    memcpy(meshData, color.components, 4 * sizeof(float));
    meshData += 4 * sizeof(float);
  }
}

static void deserializeMesh(Mesh &mesh, char *meshData, size_t numVertices,
                            size_t numIndices, size_t numColors) {
  mesh.vertices().resize(numVertices); // Allocate upfront if needed
  for (auto &vertex : mesh.vertices()) {
    memcpy(vertex.elems(), meshData, 3 * sizeof(float));
    meshData += 3 * sizeof(float);
  }
  mesh.indices().resize(numIndices); // Allocate upfront if needed
  for (auto &index : mesh.indices()) {
    memcpy(&index, meshData, sizeof(unsigned int));
    *meshData++ = sizeof(unsigned int);
  }
  mesh.colors().resize(numColors); // Allocate upfront if needed
  for (auto &color : mesh.colors()) {
    memcpy(color.components, meshData, 3 * sizeof(float));
    meshData += 4 * sizeof(float);
  }
}

} // namespace ser
} // namespace al

#endif // AL_SERIALIZE_HPP
