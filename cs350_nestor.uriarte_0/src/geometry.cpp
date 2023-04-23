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


#include "pch.hpp"
#include "geometry.hpp"
#include "renderer.hpp"

namespace cs350 {

	/**************************************************************************
	*!
	\fn     segment::segment

	\brief 
	The constructor for the struct

	\param  glm::vec3 p0
	the start point

	\param  glm::vec3 p1
	the ending point

	*
	**************************************************************************/
	segment::segment(glm::vec3 p0, glm::vec3 p1)
	{
		mP0 = p0;
		mP1 = p1;

		mMesh = renderer::instance().resources().meshes.segment;

	}

	/**************************************************************************
	*!
	\fn     plane::plane

	\brief 
	the constructor for the struct

	\param  glm::vec3 pos
	the position of the plane

	\param  glm::vec3 norm
	the normal of the plane

	*
	**************************************************************************/
	plane::plane(glm::vec3 pos, glm::vec3 norm)
	{
		mPosition = pos;
		mNormal = norm;

		mMesh = renderer::instance().resources().meshes.quad;
	}

	/**************************************************************************
	*!
	\fn     triangle::triangle

	\brief 
	the constructor for the struct

	\param  glm::vec3 v0
	vertex 0

	\param  glm::vec3 v1
	vertex 1

	\param  glm::vec3 v2
	vertex 2

	*
	**************************************************************************/
	triangle::triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
	{
		mV0 = v0;
		mV1 = v1;
		mV2 = v2;

		mMesh = renderer::instance().resources().meshes.triangle;
	}

	/**************************************************************************
	*!
	\fn     aabb::aabb

	\brief 
	the constructor of the struct

	\param  glm::vec3 min
	the min point values on xyz

	\param  glm::vec3 max
	the max point values on xyz

	*
	**************************************************************************/
	aabb::aabb(glm::vec3 min, glm::vec3 max)
	{
		mMin = min;
		mMax = max;

		mMesh = renderer::instance().resources().meshes.cube;
	}

	/**************************************************************************
	*!
	\fn     sphere::sphere

	\brief 
	the constructor of the struct

	\param  glm::vec3 pos
	the position of the sphere

	\param  float radius
	the radius

	*
	**************************************************************************/
	sphere::sphere(glm::vec3 pos, float radius)
	{
		mPosition = pos;
		mRadius = radius;

		mMesh = renderer::instance().resources().meshes.sphere;
	}
}
