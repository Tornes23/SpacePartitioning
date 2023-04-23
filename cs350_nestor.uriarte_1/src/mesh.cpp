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
- GLuint mesh::getVAO();
- GLsizei mesh::getDrawElements();

***************************************************************************/


#include "pch.hpp"
#include "mesh.hpp"
#include "opengl.hpp"

namespace cs350 {

    /**************************************************************************
    *!
    \fn     mesh::generate

    \brief 
    Function to generate a mesh from a mesh_data structure

    \param  const mesh_data& data
    the data to use to create

    *
    **************************************************************************/
    void mesh::generate(const mesh_data& data)
    {
        //for each face on the mesh data
        for (unsigned i = 0; i < data.faces.size(); i++)
        {
            glm::vec3 triVertex[3];

            //tranform from index based to array based
            for (int j = 0; j < 3; j++)
            {
                //geting the index for the position, uv and normal for this face
                int posIndex = data.faces[i][j].x;
                int uvIndex  = data.faces[i][j].y;
                int norIndex = data.faces[i][j].z;

                //if is a valid index add it to the vector
                if (posIndex >= 0)
                    positions.push_back(data.positions[posIndex]);

                //if is a valid index add it to the vector
                if (uvIndex >= 0)
                    uvs.push_back(data.uvs[uvIndex]);

                //if is a valid index add it to the vector
                if (norIndex >= 0)
                    normals.push_back(data.normals[norIndex]);

                triVertex[j] = data.positions[posIndex];

            }

            triangles.push_back(triangle(triVertex[0], triVertex[1], triVertex[2]));
        }

        //generate the buffers and bind them
        generateBuffers();
        bindBuffers();

    }

    /**************************************************************************
    *!
    \fn     mesh::~mesh

    \brief 
    the destructor of the class

    *
    **************************************************************************/
    mesh::~mesh() { freeBuffers(); }

    /**************************************************************************
    *!
    \fn     mesh::generateBuffers

    \brief 
    generates the VAO and VBO buffers

    *
    **************************************************************************/
    void mesh::generateBuffers()
    {
        //creating the VAO and VBOs
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(3, mVBO);
    }

    /**************************************************************************
    *!
    \fn     mesh::bindBuffers

    \brief 
    Binds the buffers with the data that it stores

    *
    **************************************************************************/
    void mesh::bindBuffers()
    {
        //binding the VAO
        glBindVertexArray(mVAO);

        //addign pos
        glBindBuffer(GL_ARRAY_BUFFER, mVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0].x, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        //adding textureCoords if were given
        if (uvs.size() != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVBO[1]);
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0].x, GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        }
        
        //adding normals if were given
        if (normals.size() != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, mVBO[2]);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0].x, GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        }
    
        //binding the default array again
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(mVAO);

    }

    /**************************************************************************
    *!
    \fn     mesh::freeBuffers

    \brief 
    frees the buffers

    *
    **************************************************************************/
    void mesh::freeBuffers()
    {
        //freeing the memory
        glDeleteBuffers(3, mVBO);
        glDeleteVertexArrays(1, &mVAO);

        //clearing the vectors
        positions.clear();
        normals.clear();
        uvs.clear();
    }

    /**************************************************************************
    *!
    \fn     mesh::getVAO

    \brief 
    Getter for the handle of the VAO

    \return GLuint
    the handle

    *
    **************************************************************************/
    GLuint mesh::getVAO()
    {
        return mVAO;
    }

    /**************************************************************************
    *!
    \fn     mesh::getDrawElements

    \brief 
    Utility function to know the number of vertices to draw

    \return GLsizei
    the vertex count

    *
    **************************************************************************/
    GLsizei mesh::getDrawElements()
    {
        return static_cast<GLsizei>(positions.size());
    }
    std::vector<triangle> mesh::getTriangles() const
    {
        return triangles;
    }
}
