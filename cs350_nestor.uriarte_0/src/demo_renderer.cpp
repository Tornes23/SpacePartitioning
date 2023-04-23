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

***************************************************************************/


#include "pch.hpp"
#include <opengl.hpp>
#include <renderer.hpp>
#include <imgui.hpp>
#include <demo_renderer.hpp>
#include "mesh_data.hpp"
#include "mesh.hpp"

namespace cs350 {

    /**************************************************************************
    *!
    \fn     demo_renderer::create

    \brief 
    Creates the demo

    *
    **************************************************************************/
    void demo_renderer::create()
    {
        //getting the renderer
        renderer& renderer = cs350::renderer::instance();


        renderer.create(1280, 720, "CS350", false);

    }

    /**************************************************************************
    *!
    \fn     demo_renderer::destroy

    \brief 
    Destroys the demo

    *
    **************************************************************************/
    void demo_renderer::destroy()
    {
        //getting the instance
        renderer& renderer = cs350::renderer::instance();

        //destroying
        renderer.destroy();
    }

    /**************************************************************************
    *!
    \fn     demo_renderer::update

    \brief 
    Update function for the demo

    \return bool
    boolean to know when to stop

    *
    **************************************************************************/
    bool demo_renderer::update()
    {
        //getting the instance
        renderer& renderer = cs350::renderer::instance();

        //getting the window
        window& window = renderer.window();

        //if the window is closed return fals to stop the demo
        if (window.update() == false)
            return false;

        window.clear();

        render();

        //swap the buffers
        glfwSwapBuffers(window.handle());
        glfwPollEvents();

        //returning to continue the update
        return true;
    }
    void demo_renderer::render()
    {
        //demo for point
        cs350::debug_draw_point({ 0,0,0 }, { 1, 0, 0, 1 });

        // demo segments 
        cs350::debug_draw_segment(cs350::segment({ 2, 0, 0 }, { 4, 2, 0 }), { 1, 1, 1, 1 });
        
        //demo triangles
        cs350::debug_draw_triangle(cs350::triangle({ 5, 0, 0 }, { 9, 0, 0 }, { 7, 2, 0 }), { 1, 1, 1, 1 });
        
        //demo for planes
        cs350::debug_draw_plane({ 10, 0, 0 }, glm::normalize(glm::vec3({ 1, 1, 1 })), { 1, 1, 1, 1 });
        cs350::debug_draw_plane({ 15, 0, 0 }, glm::normalize(glm::vec3({ 1, 1, -1 })), { 1, 1, 1, 1 }, 2.0f);

        //demo aabbs
        cs350::debug_draw_aabb(cs350::aabb({ 20, 0, 0 }, { 22, 2, 0 }), { 1, 1, 1, 1 });
        
        //demo spheres
        cs350::debug_draw_sphere(cs350::sphere({ 25, 0, 0 }, 1.0f), { 1, 1, 1, 1 });
        
        //demo bunny
        auto& bunny = cs350::renderer::instance().resources().meshes.bunny;
        auto  T = glm::translate(glm::vec3(30, -2, 0));
        auto  S = glm::scale(glm::vec3(30.0f));
        auto  m2w = T * S;
        
        //bunny wireframe
        cs350::debug_draw_fancy(bunny, m2w, { 1, 1, 1, 0.5 });
        
        T = glm::translate(glm::vec3(35, -2, 0));
        m2w = T * S;
        
        //bunny regular
        cs350::debug_draw_fancy(bunny, m2w, { 1, 1, 1, 0.5 }, false);
        
        T = glm::translate(glm::vec3(40, -2, 0));
        m2w = T * S;
        
        //bunny points
        cs350::debug_draw_points(bunny, m2w, { 1, 1, 1, 0.5 });

        //demo sphere discs

        //demo frustum

        //demo sponza
        auto& sponza = cs350::renderer::instance().resources().meshes.sponza;

        T = glm::translate(glm::vec3(52, -2, 0));
        S = glm::scale(glm::vec3(0.5f));
        m2w = T * S;
        
        cs350::debug_draw_fancy(sponza, m2w, { 1, 1, 1, 0.2 });
        

    }
}
