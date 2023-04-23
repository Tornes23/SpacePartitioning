/*!**************************************************************************
\file    geometry.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 12:16:27 2020

\brief	 This file containsm the implementation of the
Basic Geometric Structures

The functions included are:
- segment::segment(glm::vec3 p0, glm::vec3 p1);
- plane::plane(glm::vec3 pos, glm::vec3 norm);
- triangle::triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);
- aabb::aabb(glm::vec3 min, glm::vec3 max);
- sphere::sphere(glm::vec3 pos, float radius);

***************************************************************************/
#pragma once
#include "math.hpp"
#include "mesh.hpp"

namespace cs350 {

    struct ray
    {

    };

    struct segment
    {
        //constructor
        segment(glm::vec3 p0, glm::vec3 p1);

        std::shared_ptr<mesh> mMesh;

        glm::vec3 mP0;
        glm::vec3 mP1;
    };

    struct plane
    {
        //constructor
        plane(glm::vec3 pos, glm::vec3 norm);

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //necessary data
        glm::vec3 mPosition;
        glm::vec3 mNormal;
        glm::vec4 mColor;

    };

    struct triangle
    {
        //constructor
        triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //necessary data
        glm::vec3 mV0;
        glm::vec3 mV1;
        glm::vec3 mV2;
    };

    struct aabb
    {
        //constructor
        aabb(glm::vec3 min, glm::vec3 max);

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //necessary data
        glm::vec3 mMin;
        glm::vec3 mMax;
    };

    struct sphere
    {
        //constructor
        sphere(glm::vec3 pos, float radius);

        //the mesh
        std::shared_ptr<mesh> mMesh;

        //necessary data
        glm::vec3 mPosition;
        float mRadius;


    };

    struct frustum
    {
        //the mesh
        std::shared_ptr<mesh> mMesh;


    };
}