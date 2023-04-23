/*!**************************************************************************
\file    mesh_data.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 12:44:05 2020

\brief	 This file contains the implementation of the
parser functions

The functions included are:
- mesh_data load_obj(const char* filename);
- std::vector<mesh_data> load_objs(const char* filename);

***************************************************************************/

#pragma once
#include "geometry.hpp" // triangle

namespace cs350 {
    struct mesh_data
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec<3, glm::ivec3>> faces;
    };

    mesh_data load_obj(const char* filename);
    std::vector<mesh_data> load_objs(const char* filename);
}