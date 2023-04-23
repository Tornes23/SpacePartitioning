/*!**************************************************************************
\file    ../debug.cpp

\author  Nestor Uriarte

\par     DP email:  nestor.uriarte@digipen.edu

\par     Course:    CS350

\par     assignemnt 0

\date    Mon Sep 21 17:16:04 2020

\brief	 This file containsm the definition of the
debug draw features

The functions included are:
- void debug_draw_point(glm::vec3 pt, glm::vec4 color);
- void debug_draw_points(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color);
- void debug_draw_segment(segment const& s, glm::vec4 color);
- void debug_draw_segments(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color);
- void debug_draw_triangle(triangle const& t, glm::vec4 color);
- void debug_draw_plane(glm::vec3 pt, glm::vec3 n, glm::vec4 color, float scale);
- void debug_draw_aabb(aabb const& a, glm::vec4 color);
- void debug_draw_sphere(sphere const& s, glm::vec4 color);
- void debug_draw_sphere_discs(sphere const& s, glm::vec4 color);
- void debug_draw_frustum(frustum const& s, glm::vec4 color);
- void debug_draw_fancy(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color, bool wire);
- void debug_draw_plain_color(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 const& color, unsigned mode);
- void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

***************************************************************************/
#pragma once
#include "math.hpp"
#include "geometry.hpp"

// IO
namespace glm{
    std::istream& operator>>(std::istream& is, vec2& v);
    std::ostream& operator<<(std::ostream& os, vec2 const& p);
	std::istream& operator>>(std::istream& is, vec3& v);
	std::ostream& operator<<(std::ostream& os, vec3 const& p);
}


namespace cs350{
	// Debug drawing
	void debug_draw_point(glm::vec3 pt, glm::vec4 color);
	void debug_draw_points(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_segment(segment const& s, glm::vec4 color);
	void debug_draw_segments(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color);
	void debug_draw_triangle(triangle const& t, glm::vec4 color);
	void debug_draw_plane(glm::vec3 pt, glm::vec3 n, glm::vec4 color, float scale = 1.0f);
	void debug_draw_aabb(aabb const& a, glm::vec4 color);
	void debug_draw_sphere(sphere const& s, glm::vec4 color);
    void debug_draw_sphere_discs(sphere const& s, glm::vec4 color);
    void debug_draw_frustum(frustum const& s, glm::vec4 color);
	void debug_draw_fancy(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color, bool wire = true);
	void debug_draw_plain_color(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 const& color, unsigned mode);

	void openglCallbackFunction (GLenum source,
								GLenum type,
								GLuint id,
								GLenum severity,
								GLsizei length,
								const GLchar* message,
								const void* userParam);
}