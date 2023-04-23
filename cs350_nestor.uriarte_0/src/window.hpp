/*!**************************************************************************
\file    window.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 13:15:17 2020

\brief	 This file containsm the implementation of the
window class

The functions included are:
- bool window::create(int w, int h, const char* window_name, bool hidden);
- bool window::update();
- void window::destroy();
- void window::clear();
- void window::handleInput();
- void window::setMousePos(glm::vec2 pos);
- void mouseCallback(GLFWwindow* win, double xpos, double ypos);

***************************************************************************/

#pragma once
#include "math.hpp"

namespace cs350{

	

	class window {
	private:
		GLFWwindow* m_window = {};
		glm::ivec2 m_size = {};

		float mLastFrame = 0.0F;

		glm::vec2 mMousePos = glm::vec2(0.0F, 0.0F);

	public:
		~window() {destroy();};
		bool create(int w, int h, const char* window_name, bool hidden);
		bool update();
		void destroy();
		void clear();
		void handleInput();
		void setMousePos(glm::vec2 pos);
		glm::vec2& mousePosition();

		[[nodiscard]] glm::ivec2 size() const noexcept {return m_size;}
		[[nodiscard]] GLFWwindow* handle() const noexcept {return m_window;}
	};
}