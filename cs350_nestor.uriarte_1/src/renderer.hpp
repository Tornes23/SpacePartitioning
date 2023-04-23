/*!**************************************************************************
\file    renderer.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 13:03:11 2020

\brief	 This file containsm the implementation of the
renderer class

The functions included are:
- void renderer::create(unsigned w, unsigned h, const char* title, bool hidden);
- void renderer::destroy();
- void renderer::load_resources();
- camera& renderer::camera();
- window& renderer::window();
- shaderProgram& renderer::getShader();
- resources& renderer::resources();

***************************************************************************/

#pragma once

#include "math.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "geometry.hpp"
#include "shader.hpp"

namespace cs350{

	enum class meshType
	{
		bunny,
		cube,
		cylinder,
		gourd,
		icosahedron,
		line,
		octohedron,
		quad,
		segment,
		sphere,
		sponza,
		suzzane,
		triangle
	};

	/**
	 * Contains all the necessary information for the assignment. It's a Singleton
	 *
	 * Keeps track of the needed systems.
	 */

	//structure containing the shared pointers
	struct loadedMeshes
	{
		std::shared_ptr<mesh> bunny;
		std::shared_ptr<mesh> cube;
		std::shared_ptr<mesh> cylinder;
		std::shared_ptr<mesh> gourd;
		std::shared_ptr<mesh> icosahedron;
		std::shared_ptr<mesh> line;
		std::shared_ptr<mesh> octohedron;
		std::shared_ptr<mesh> quad;
		std::shared_ptr<mesh> segment;
		std::shared_ptr<mesh> sphere;
		std::shared_ptr<mesh> sponza;
		std::shared_ptr<mesh> suzanne;
		std::shared_ptr<mesh> triangle;

	};

	//structure storing the resources
	struct resources
	{
		loadedMeshes meshes;
	};

	class renderer {
	private:		
		camera mCamera{};
		window mWindow{};
		shaderProgram mShader{};

		resources mResources;

	public:
		static renderer& instance()
		{
			static renderer inst;
			return inst;
		}
		~renderer() {destroy();}

		// Core features
		void create(unsigned w, unsigned h, const char* title, bool hidden);
		void destroy();
		void load_resources();
		cs350::camera& camera();
		cs350::window& window();
		shaderProgram& getShader();
		cs350::resources& resources();
		std::shared_ptr<mesh>& getMesh(meshType mesh);

	private:
		renderer() = default;
		renderer(const renderer& rhs) = delete;
		renderer& operator=(const renderer& rhs) = delete;

	};
}
