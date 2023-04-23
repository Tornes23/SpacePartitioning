/**
* @file	demo_bvh.hpp
* @author Nestor Uriarte ,540000817, nestor.uriarte@digipen.edu
* @date	Fri Oct 23 20:18:42 2020
* @brief	This file contains the demo of the BVH tree and the three algorithms showed in class The camera that will be used on the project.
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#pragma once
#include "window.hpp"
#include "gameobject.hpp"
#include "bvh.hpp"

namespace cs350 {

	enum class TreeMethod
	{
		TopDown,
		BottomUp,
		Incremental
	};

	enum class AXIS
	{
		X,
		Y,
		Z
	};

	class demo_bvh {
	private:
		// For camera update
		float     m_last_frame_time{};
		glm::vec2 m_cursor_pos = {};
	public:
		void create();
		void destroy();
		bool update();
		void render();
		void CreateEnviroment();

		//ImGui
		void Edit();
		void renderGui();
		bool EditBVH();
		void RenderBVH(Node<triangle>* target, bool render);
		void EditNode(Node<triangle>* current, bool render, bool triangles);

		//BVH related
		void RecomputeTree();
		std::vector<triangle>& GetTriangles();
		aabb ComputeAABB(std::vector<triangle>& toBound);
		glm::vec3 ComputeCentre(triangle& triangle);
		AXIS GetPartitionAxis(std::vector<triangle>& container);
		float GetSurfaceArea(std::vector<triangle>& container);

		//Top Down
		void Partition(std::vector<triangle>& container, std::vector<triangle>& right, std::vector<triangle>& left);
		void TopDown(Node<triangle>* current);

		//Bottom Up
		void BottomUp();
		std::vector<triangle> GetClosestNodes(std::vector<Node<triangle>*>& nodes, int count,int* left, int* right);


		//Incremental
		void Incremental();
		void GetBestNode(const Node<triangle>* target, Node<triangle>* current, float& area, Node<triangle>** bestNode);


	private:

		const int MIN_TRIANGLES = 1;

		bool mRenderBVH;
		bool mRenderBVHTrangles;

		BVHTree<triangle> mTopDown;
		BVHTree<triangle> mBottomUp;
		BVHTree<triangle> mIncremental;

		TreeMethod mMethod;

		std::vector<GameObject> mObjs;
		std::vector<triangle> mTriangles;
	};
}