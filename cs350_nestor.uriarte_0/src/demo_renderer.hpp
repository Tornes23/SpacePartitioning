/*!**************************************************************************
\file    ../demo_renderer.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 13:26:26 2020

\brief	 This file containsm the implementation of the
demo for the renderer

The functions included are:
- void demo_renderer::create();
- void demo_renderer::destroy();
- bool demo_renderer::update();
- void demo_renderer::render();

***************************************************************************/

#pragma once
#include "window.hpp"

namespace cs350{
	class demo_renderer {
	private:
		// For camera update
		float     m_last_frame_time{};
		glm::vec2 m_cursor_pos = {};
	public:
		void create();
		void destroy();
		bool update();
		void render();
	private:


	};
}