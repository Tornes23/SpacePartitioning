/*!**************************************************************************
\file    mesh.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 12:39:00 2020

\brief	 This file containsm the implementation of the
Mesh class

The functions included are:
- void mesh::generate(const mesh_data& data);
- mesh::~mesh();
- void mesh::generateBuffers();
- void mesh::bindBuffers();
- void mesh::freeBuffers();
- glm::vec3* mesh::getFirstPos();
- GLuint mesh::getPosVBO();
- GLuint mesh::getVAO();
- GLsizei mesh::getDrawElements();

***************************************************************************/

#pragma once
#include "mesh_data.hpp"
#include "opengl.hpp"

namespace cs350 {
    class mesh
    {
      private:

          std::vector< glm::vec3> positions;
          std::vector< glm::vec2> uvs;
          std::vector< glm::vec3> normals;

          GLuint mVAO;
          GLuint mVBO[3];

      public:
          void generate(const mesh_data& data);
          ~mesh();
          void generateBuffers();
          void bindBuffers();
          void freeBuffers();
          glm::vec3* getFirstPos();
          GLuint getPosVBO();

          GLuint getVAO();
          GLsizei getDrawElements();


    };

}