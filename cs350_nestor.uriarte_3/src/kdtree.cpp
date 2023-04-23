/**
* @file		 kdtree.cpp      
* @author	 Nestor Uriarte,  nestor.uriarte@digipen.edu    
* @date		 Thu Nov 26 03:37:29 2020          
* @brief	 This file contains the implementation of the KDTree class     
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include <cassert>
#include <cmath>
#include <iomanip>
#include <functional>
#include <sstream>
#include "kdtree.hpp"
#include "scene_data.hpp"

namespace cs350 {

/**
* @brief	sets the variables for a leaf node
* @param	int first_primitive_index
* @param	int primitive_count
* @return		void
**/
	void kdtree::node::set_leaf(int first_primitive_index, int primitive_count)
	{
		//setting the variables
		m_start_primitive = first_primitive_index;
		m_count = primitive_count;

		m_count <<= 2;
		m_count |= 0b11;
	}

/**
* @brief	sets the variable for a internal node
* @param	int axis
* @param	float split_point
* @param	int subnode_index
* @return		void
**/
	void kdtree::node::set_internal(int axis, float split_point, int subnode_index)
	{
		//setting the variables
		m_split = split_point;

		m_subnode_index = subnode_index;

		m_subnode_index <<= 2;
		m_subnode_index |= axis;
	}

/**
* @brief	to know if is a leaf
* @return		bool
**/
	bool kdtree::node::is_leaf() const noexcept
	{
		//check if last 2 bits on count are 11
		int axis = m_count & 0b11;

		if (axis == 3)
			return true;

		return false;
	}

/**
* @brief	to know if is a internal
* @return		bool
**/
	bool kdtree::node::is_internal() const noexcept
	{
		//check if last 2 bits on count are not 11 (00 -> x, 01 -> y 10 -> z)
		int axis = m_count & 0b11;

		if (axis != 3)
			return true;

		return false;
	}

/**
* @brief	returns the triangle count
* @return		int
**/
	int kdtree::node::primitive_count() const noexcept
	{
		//remove the last 2 bits and return count
		unsigned int count = m_count >> 2;
		return count;
	}

/**
* @brief	returns the index of the first triangle in the node
* @return		int
**/
	int kdtree::node::primitive_start() const noexcept { return m_start_primitive; }

/**
* @brief	gets the right node of a internal one
* @return		int
**/
	int kdtree::node::next_child() const noexcept
	{
		//remove the last 2 bits and return count
		int rightNode = m_subnode_index >> 2;
		return rightNode;
	}

/**
* @brief	gets the splitting point
* @return		float
**/
	float kdtree::node::split() const noexcept { return m_split; }

/**
* @brief	gets the splitting axis
* @return		int
**/
	int kdtree::node::axis() const noexcept
	{
		//get last 2 bits on the subnode index
		int axis = m_subnode_index & 0b11;

		return axis;
	}

/**
* @brief	wrapper to call the recursive build
* @param	triangle_container const& all_triangles
* @param	const config& cfg
* @return		void
**/
	void kdtree::build(triangle_container const& all_triangles, const config& cfg)
	{
		//setting the config
		m_cfg = cfg;

		//pushing back the triangles
		for (unsigned i = 0; i < all_triangles.size(); i++)
			m_triangles.push_back({ all_triangles[i].geometry, i });

		//calling to build the tree
		build_tree(m_triangles, 0);
	}

/**
* @brief	recursive function that builds the KDTree
* @param	std::vector<triangle_wrapper> const& triangles
* @param	int depth
* @return		void
**/
	void kdtree::build_tree(std::vector<triangle_wrapper> const& triangles, int depth)
	{
		//creating the new node
		node new_node;

		//adding to the vector the node and its bv
		m_nodes.push_back(new_node);
		m_aabbs.push_back(computeBV(triangles));

		//getting the nodes index
		unsigned currIndex = m_nodes.size() - 1;

		//if it does not have a valid depth create a leaf and return
		if (depth >= m_cfg.max_depth)
		{
			m_nodes[currIndex].set_leaf(m_indices.size(), triangles.size());

			for (unsigned i = 0; i < triangles.size(); i++)
				m_indices.push_back(triangles[i].original_index);

			return;
		}

		//getting the partition axis
		int axis = depth % 3;
		float cost = 0.0F;

		std::vector<triangle_wrapper> left;
		std::vector<triangle_wrapper> right;
		//getting the splitting point
		float splitPoint = get_split(triangles, axis, &cost);

		//performing the splitting on the optimal point
		split(triangles, left, right, axis, splitPoint);

		//if the left or the right nodes are empty, or the cost of making it a leaf is lower create a leaf
		if (cost_leaf(triangles) <= cost || left.empty() || right.empty())
		{
			//setting it as a leaf
			m_nodes[currIndex].set_leaf(m_indices.size(), triangles.size());

			//adding thew indices
			for (unsigned i = 0; i < triangles.size(); i++)
				m_indices.push_back(triangles[i].original_index);

		}
		else
		{
			//calling to build the left node
			build_tree(left, depth + 1);

			//setting the node as internal
			m_nodes[currIndex].set_internal(axis, splitPoint, m_nodes.size());

			//calling to build the right node
			build_tree(right, depth + 1);
		}
	}

/**
* @brief	gets the splitting point based on heuristics
* @param	std::vector<triangle_wrapper> const& triangles
* @param	int axis
* @param	float* min_cost
* @param	std::vector<triangle_wrapper>& left
* @param	std::vector<triangle_wrapper>& right
* @return		float
**/
	float kdtree::get_split(std::vector<triangle_wrapper> const& triangles, int axis, float* min_cost)
	{
		float min = std::numeric_limits<float>::max();
		float splitPoint = 0.0F;

		aabb parentBV= computeBV(triangles);
		float parentSurface = compute_surface(parentBV);
		
		const unsigned samples = 10;
		const float step = (parentBV.mMax[axis] - parentBV.mMin[axis]) / static_cast<float>(samples);

		//for each triangle
		for (unsigned i = 0; i < samples; i++) 
		{
			float tempSplit = parentBV.mMin[axis] + (i * step);

			int leftCount = 0;
			int rightCount = 0;

			//splitting the triangles
			split(triangles, &leftCount, &rightCount, axis, tempSplit);

			//computing the aabb to have the surface and be able to use the heuristic formula
			glm::vec3 limitLeft = parentBV.mMax;
			glm::vec3 limitRight = parentBV.mMin;

			limitLeft[axis] = tempSplit;
			limitRight[axis] = tempSplit;

			aabb leftBV(parentBV.mMin, limitLeft);
			aabb rightBV(limitRight, parentBV.mMax);

			//computing the probability based on surface areas
			float surfaceLeft = compute_surface(leftBV) / parentSurface;
			float surfaceRight = compute_surface(rightBV) / parentSurface;

			//getting the cost
			float cost = cost_intersect(surfaceLeft, leftCount, surfaceRight, rightCount);

			//if the cost is lower update the values
			if (cost < min)
			{
				min = cost;
				splitPoint = tempSplit;
				*min_cost = cost;
			}
		}

		//returning the splitting point
		return splitPoint;
	}

/**
* @brief	splits the tiangles into left and right based on their position
* @param	std::vector<triangle_wrapper> const& triangles
* @param	int* left
* @param	int* right
* @param	int axis
* @param	float splitPoint
* @return		void
**/
	void kdtree::split(std::vector<triangle_wrapper> const& triangles, int* left, int* right, int axis, float splitPoint)
	{
		//creating the plane
		glm::vec3 planePos{};
		glm::vec3 planeNormal{};

		planePos[axis] = splitPoint;
		planeNormal[axis] = 1.0F;

		//the burning plane at a thousand degrees� that will be used to split
		plane bpatd(planePos, planeNormal);

		for (auto& it : triangles)
		{
			//getting the classification based on the plane
			classification_t result = classify_plane_triangle(bpatd, it.tri, cEpsilon);

			//if is inside increment left
			if (result == classification_t::inside)
				*left += 1;
			else if (result == classification_t::outside)//if is out increment right
				*right += 1;
			else//if overlaps increment both
			{
				*left += 1;
				*right += 1;
			}
		}
	}

/**
* @brief	computes the cost of making it a leaf node
* @param	std::vector<triangle_wrapper> const& triangles
* @return		int
**/
	int kdtree::cost_leaf(std::vector<triangle_wrapper> const& triangles)
	{
		//returning the result of the heuristics formula
		return m_cfg.cost_intersection * triangles.size();
	}

/**
* @brief	splits the tiangles into left and right based on their position
* @param	std::vector<triangle_wrapper> const& triangles
* @param	std::vector<triangle_wrapper>& left
* @param	std::vector<triangle_wrapper>& right
* @param	int axis
* @param	float splitPoint
* @return		void
**/
	void kdtree::split(std::vector<triangle_wrapper> const& triangles, std::vector<triangle_wrapper>& left, std::vector<triangle_wrapper>& right, int axis, float splitPoint)
	{
		//creating the plane
		glm::vec3 planePos{};
		glm::vec3 planeNormal{};
		
		planePos[axis] = splitPoint;
		planeNormal[axis] = 1.0F;

		//the burning plane at a thousand degrees� that will be used to split
		plane bpatd(planePos, planeNormal);

		//for each triangle
		for (auto& it : triangles)
		{
			//getting the classification based on the plane
			classification_t result = classify_plane_triangle(bpatd, it.tri, cEpsilon);

			//if is inside push to the left
			if (result == classification_t::inside)
			{
				left.push_back(it);
			}
			else if (result == classification_t::outside)//if is out push to right
			{
				right.push_back(it);
			}
			else//if overlaps push to both
			{
				right.push_back(it);
				left.push_back(it);
			}
		}
	}

/**
* @brief	Computes the bv of a triangle
* @param	triangle_wrapper const& triangle
* @return		aabb
**/
	aabb kdtree::computeBV(triangle_wrapper const& triangle)
	{
		//aabb which will contain the values
		aabb bounding;

		//floats to store the min and max values
		float mins[3] = { std::numeric_limits<float>::max(),
						  std::numeric_limits<float>::max(),
						  std::numeric_limits<float>::max() };

		float maxs[3] = { -std::numeric_limits<float>::max(),
						  -std::numeric_limits<float>::max(),
						  -std::numeric_limits<float>::max() };

		//go throught the vector and get the lowest and highest x, y, z values
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (triangle[i][j] <= mins[j])
					mins[j] = triangle[i][j];

				if (triangle[i][j] >= maxs[j])
					maxs[j] = triangle[i][j];
			}
		}

		//setting the points
		bounding.mMin = glm::vec3(mins[0], mins[1], mins[2]);
		bounding.mMax = glm::vec3(maxs[0], maxs[1], maxs[2]);

		return bounding;
	}

/**
* @brief	Computes the bv of a vector of triangles
* @param	std::vector<triangle_wrapper> const& triangles
* @return		aabb
**/
	aabb kdtree::computeBV(std::vector<triangle_wrapper> const& triangles)
	{
		//aabb which will contain the values
		aabb bounding;

		//floats to store the min and max values
		float mins[3] = { std::numeric_limits<float>::max(),
						  std::numeric_limits<float>::max(),
						  std::numeric_limits<float>::max() };

		float maxs[3] = { -std::numeric_limits<float>::max(),
						  -std::numeric_limits<float>::max(),
						  -std::numeric_limits<float>::max() };

		//go throught the vector and get the lowest and highest x, y, z values
		for (unsigned i = 0; i < triangles.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (triangles[i][j][k] <= mins[k])
						mins[k] = triangles[i][j][k];

					if (triangles[i][j][k] >= maxs[k])
						maxs[k] = triangles[i][j][k];
				}
			}

		}

		//setting the points
		bounding.mMin = glm::vec3(mins[0], mins[1], mins[2]);
		bounding.mMax = glm::vec3(maxs[0], maxs[1], maxs[2]);

		return bounding;
	}

/**
* @brief	computes the surface area of a bv
* @param	const aabb& bv
* @return		float
**/
	float kdtree::compute_surface(const aabb& bv)
	{
		//the scale on x y and z
		float scaleX = bv.mMax.x - bv.mMin.x;
		float scaleY = bv.mMax.y - bv.mMin.y;
		float scaleZ = bv.mMax.z - bv.mMin.z;

		//compute the area of 3 faces
		float area1 = scaleX * scaleY;
		float area2 = scaleZ * scaleY;
		float area3 = scaleX * scaleZ;

		//compute the total area
		float area = (area1 + area2 + area3) * 2.0F;

		//returning tne total area
		return area;
	}

/**
* @brief	computes the cost of intersection for a node
* @param	float surfaceA
* @param	int countA
* @param	float surfaceB
* @param	int countB
* @return		int
**/
	int kdtree::cost_intersect(float surfaceA, int countA, float surfaceB, int countB)
	{
		//returning the result of the heuristics formula
		int cost = m_cfg.cost_traversal + m_cfg.cost_intersection * (surfaceA * countA + surfaceB * countB);
		return cost;
	}
	
/**
* @brief	wrapper to get the closest triangle
* @param	ray const r
* @param	debug_stats* stats
* @return		kdtree::intersection
**/
	kdtree::intersection kdtree::get_closest(ray const r, debug_stats* stats) const
	{
		//intersection value
		intersection t{ 0, -1.0F };

		//if does not intersect with the root return straight away
		if (intersection_ray_aabb(r, m_aabbs[0]) < 0.0F)
			return t;

		//getting the minimum intersection time
		get_min(r, 0, t);

		//returing the intersecion object
		return t;
	}

/**
* @brief	recursive fuction that gets the triangle with the mionimum intersection time
* @param	ray const& r
* @param	int currNode
* @param	intersection& minT
* @return		kdtree::intersection
**/
	kdtree::intersection kdtree::get_min(ray const& r, int currNode, intersection& minT) const
	{
		//cheching is a valid node
		if (currNode < 0.0F)
			return minT;

		//if does not intersect with the aabb of this node return an error intersection code
		if (intersection_ray_aabb(r, m_aabbs[currNode]) < 0.0F)
		{
			intersection error{ 0, -1.0F };
			return error;
		}
		
		//if the node is a leaf
		if (m_nodes[currNode].is_leaf())
		{
			unsigned size = m_nodes[currNode].primitive_count();

			//checking with every triangle in the node
			for (unsigned i = 0; i < size; i++)
			{
				//getting the starting index
				unsigned index = m_nodes[currNode].primitive_start();

				//getting the intersection time for the triangle
				float time = intersection_ray_triangle(r, m_triangles[m_indices[index + i]].tri);

				//if does not intersect skip it
				if (time < 0.0F)
					continue;

				//if the minimum time is negative or the result is lower than the stored one update it
				if (time < minT.t || minT.t < 0.0F)
				{
					minT.triangle_index = m_indices[index + i];
					minT.t = time;
				}
			}
		}

		//if the node is a intermediate
		if(m_nodes[currNode].is_internal())
		{
			//getting the partition axis and the splitting point
			int axis = m_nodes[currNode].axis();
			float splitPoint = m_nodes[currNode].split();

			//if the rays starting pos is over or under the splitting point get the left or right node
			int leftIndex = -1;
			if (r.mP[axis] <= splitPoint)
				leftIndex = currNode + 1;
			else
				leftIndex = m_nodes[currNode].next_child();

			int rightIndex = -1.0F;
			if (r.mP[axis] > splitPoint)
				rightIndex = currNode + 1;
			else
				rightIndex = m_nodes[currNode].next_child();

			//recursive call to the child nodes
			get_min(r, leftIndex, minT);
			get_min(r, rightIndex, minT);
		}

		//returing the minumum intersection time
		return minT;
	}

/**
* @brief	gets the depth of the tree
* @return		int
**/
	int kdtree::get_depth() const
	{
		//returning the macximum depth of the tree
		return m_cfg.max_depth;
	}
}