/*!**************************************************************************
\file    ../debug.cpp

\author  Nestor Uriarte 

\par     DP email:  nestor.uriarte@digipen.edu 

\par     Course:    CS350 

\par     assignemnt 0 

\date    Mon Sep 21 17:16:04 2020

\brief	 This file containsm the implementation of the 
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


#include "pch.hpp"
#include "debug.hpp"
#include "geometry.hpp"
#include "opengl.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include <GLM/gtc/type_ptr.hpp>

namespace glm {
    std::istream& operator>>(std::istream& is, vec2& v)
    {
        is >> v.x >> v.y;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, vec2 const& p)
    {
        os << p.x << " " << p.y;
        return os;
    }

    std::istream& operator>>(std::istream& is, vec3& v)
    {
        is >> v.x >> v.y >> v.z;
        return is;
    }

    std::ostream& operator<<(std::ostream& os, vec3 const& p)
    {
        os << p.x << " " << p.y << " " << p.z;
        return os;
    }
}

namespace cs350 {

    /**************************************************************************
    *!
    \fn     debug_draw_point

    \brief 
    debug function for drawing a point

    \param  glm::vec3 pt
    the point

    \param  glm::vec4 color
    the color for the point

    *
    **************************************************************************/
    void debug_draw_point(glm::vec3 pt, glm::vec4 color)
    {
        //getting the renderer instance
        renderer& renderer = cs350::renderer::instance();

        shaderProgram& shader = cs350::renderer::instance().getShader();

        camera& camera = renderer.camera();

        resources& resources = renderer.resources();

        //setting the shader to use
        shader.use();

        //create the m2w
        glm::mat4x4 m2w = glm::translate(pt);
        m2w *= glm::scale(glm::vec3(0.05F, 0.05F, 0.05F));
        m2w = camera.projection() * camera.view() * m2w;

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m2w));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(resources.meshes.cube->getVAO());

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, resources.meshes.cube->getDrawElements());

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);
    }
    
    /**************************************************************************
    *!
    \fn     debug_draw_points

    \brief 
        draws a mesh as points

    \param  std::shared_ptr<mesh> const& mesh
    the mesh to draw

    \param  glm::mat4 const& m2w
    the model to world matrix

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_points(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        //getting the shader and camera
        shaderProgram& shader = cs350::renderer::instance().getShader();
        camera& camera = cs350::renderer::instance().camera();

        //setting the shader to use
        shader.use();

        //computing the world to viewport matrix
        glm::mat4x4 w2c = camera.projection() * camera.view() * m2w;

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(w2c));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(mesh->getVAO());

        // Drawing
        glDrawArrays(GL_POINTS, 0, mesh->getDrawElements());

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);
    }

    /**************************************************************************
    *!
    \fn     debug_draw_segment

    \brief 
        draws a segment

    \param  segment const& s
        the segment with the data

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_segment(segment const& s, glm::vec4 color)
    {
        //getting the renderer instance
        renderer& renderer = cs350::renderer::instance();

        shaderProgram& shader = cs350::renderer::instance().getShader();

        camera& camera = renderer.camera();

        //setting the shader to use
        shader.use();

        std::vector<glm::vec3> positions;
        positions.push_back(s.mP0);
        positions.push_back(s.mP1);

        GLuint vao;
        GLuint vbo;

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        //binding the VAO
        glBindVertexArray(vao);

        //addign pos
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), &positions[0].x, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);


        //create the m2w
        glm::mat4x4 m2w = camera.projection() * camera.view();

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m2w));
        glUniform4fv(1, 1, &color[0]);

        // Drawing
        glDrawArrays(GL_LINES, 0, 2);

        //freeing the buffers
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);
    }

    /**************************************************************************
    *!
    \fn     debug_draw_segments

    \brief 
        Draws a mesh as segments

    \param  std::shared_ptr<mesh> const& mesh
    the mesh to draw

    \param  glm::mat4 const& m2w
    the model to world matrix

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_segments(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color)
    {
        //setting the polygon mode to lines
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //getting the instances
        shaderProgram& shader = cs350::renderer::instance().getShader();
        camera& camera = cs350::renderer::instance().camera();

        //setting the shader to use
        shader.use();

        //computing the world to viewport matrix
        glm::mat4x4 w2c = camera.projection() * camera.view() * m2w;

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(w2c));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(mesh->getVAO());

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, mesh->getDrawElements());

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);

    }

    /**************************************************************************
    *!
    \fn     debug_draw_triangle

    \brief 
        Debug function to draw triangles

    \param  triangle const& t
        the triangle

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_triangle(triangle const& t, glm::vec4 color)
    {
        //getting the instances
        renderer& renderer = cs350::renderer::instance();

        shaderProgram& shader = cs350::renderer::instance().getShader();

        camera& camera = renderer.camera();

        //creating a temporl mesh with the point in world
        mesh_data temp;

        temp.positions.push_back(t.mV0);
        temp.positions.push_back(t.mV1);
        temp.positions.push_back(t.mV2);

        glm::vec<3, glm::ivec3> face;

        face[0][0] = 0;
        face[1][0] = 1;
        face[2][0] = 2;

        temp.faces.push_back(face);

        mesh triangle;

        triangle.generate(temp);

        //setting the shader to use
        shader.use();

        //create the m2w
        glm::mat4x4 m2w = camera.projection() * camera.view();

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m2w));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(triangle.getVAO());

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, triangle.getDrawElements());

        //setting the draw mode to lines
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glDrawArrays(GL_TRIANGLES, 0, triangle.getDrawElements());

        //setting the draw mode to lines
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);


    }
    
    /**************************************************************************
    *!
    \fn     debug_draw_plane

    \brief 
        debug render function for the plane

    \param  glm::vec3 pt
        a point on the plane

    \param  glm::vec3 n
        the normal of the plane

    \param  glm::vec4 color
    the color for the plane

    \param  float scale
    the scale for the quad to be rendered

    *
    **************************************************************************/
    void debug_draw_plane(glm::vec3 pt, glm::vec3 n, glm::vec4 color, float scale)
    {

        //getting the instances
        renderer& renderer = cs350::renderer::instance();

        shaderProgram& shader = cs350::renderer::instance().getShader();

        camera& camera = renderer.camera();

        //setting the shader to use
        shader.use();

        //create the m2w
        glm::mat4x4 m2w(1);

        if(n != glm::vec3(0,1,0))
            m2w *= glm::inverse(glm::lookAt(pt, pt + n, glm::vec3(0, 1, 0)));
        else
            m2w *= glm::inverse(glm::lookAt(pt, pt + n, glm::vec3(0, 0, 1)));

        m2w = glm::scale(m2w, glm::vec3(scale, scale, scale));


        glm::mat4x4 w2c = camera.projection() * camera.view() * m2w;

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(w2c));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(renderer.resources().meshes.quad->getVAO());

        
        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, renderer.resources().meshes.quad->getDrawElements());

        glFrontFace(GL_CW);

        glm::vec4 grey(0.5,0.5, 0.5, 0.1);

        glUniform4fv(1, 1, &grey[0]);

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, renderer.resources().meshes.quad->getDrawElements());

        glFrontFace(GL_CCW);

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);

        //drawing the normal
        debug_draw_segment(segment(pt, pt + glm::normalize(n)), { 1,0,0,1 });

        //drawing the wireframe
        debug_draw_segments(renderer.resources().meshes.quad, m2w, { 0,0,0,1 });
    }
    
    /**************************************************************************
    *!
    \fn     debug_draw_aabb

    \brief 
        debug render for aabbs

    \param  aabb const& a
    the aabb

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_aabb(aabb const& a, glm::vec4 color)
    {
        //getting the sintances
        renderer& renderer = cs350::renderer::instance();

        shaderProgram& shader = cs350::renderer::instance().getShader();

        camera& camera = renderer.camera();
        
        //setting the shader to be used
        shader.use();
        
        //computing the center of the aabb
        glm::vec3 center = (a.mMin + a.mMax) / 2.0F;

        float scaleX = a.mMax.x - a.mMin.x;
        float scaleY = a.mMax.y - a.mMin.y;
        float scaleZ = a.mMax.z - a.mMin.z;

        //create the m2w
        glm::mat4x4 m2w = glm::translate(center);
        m2w = glm::scale(m2w, glm::vec3(scaleX, scaleY, scaleZ));

        glm::mat4x4 w2c = camera.projection() * camera.view() * m2w;

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(w2c));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(a.mMesh->getVAO());

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, a.mMesh->getDrawElements());

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);

        //drawing the wireframe
        debug_draw_segments(a.mMesh, m2w, { 0,0,0,1 });

    }
    
    /**************************************************************************
    *!
    \fn     debug_draw_sphere

    \brief 
    debug render for spheres

    \param  sphere const& s
    the shpere

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_sphere(sphere const& s, glm::vec4 color)
    {
        //getting the instances
        renderer& renderer = cs350::renderer::instance();

        shaderProgram& shader = cs350::renderer::instance().getShader();

        camera& camera = renderer.camera();

        //setting the shader to be used
        shader.use();

        //create the m2w
        glm::mat4x4 m2w = glm::translate(s.mPosition);
        m2w = glm::scale(m2w, glm::vec3(s.mRadius, s.mRadius, s.mRadius));

        glm::mat4x4 w2c = camera.projection() * camera.view() * m2w;

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(w2c));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(s.mMesh->getVAO());

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, s.mMesh->getDrawElements());

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);

        //drawing the wireframe
        debug_draw_segments(s.mMesh, m2w, {0,0,0,1});

    }
    
    /**************************************************************************
    *!
    \fn     debug_draw_sphere_discs

    \brief 
        debug draw for spheres using disc method

    \param  sphere const& s
    the sphere

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_sphere_discs(sphere const& s, glm::vec4 color)
    {
        //renderer& renderer = cs350::renderer::instance();
        //
        //shaderProgram& shader = renderer.getShader();
        //
        //camera& camera = renderer.camera();
        //
        //shader.use();
        //
        ////glm::mat4x4 m2w = camera.generatePerMatrix() * camera.generateCamMatrix();
        //glm::mat4x4 m2w(1);
        //
        //glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m2w));
        //glUniform4fv(1, 1, &color[0]);
        //
        //
        ////binding the objects VAO
        //glBindVertexArray(renderer.mMeshes[0]->getVAO());
        //
        //// Drawing
        //glDrawArrays(GL_TRIANGLES, 0, renderer.mMeshes[0]->getDrawElements());
        //
        ////unbinding the VAOs
        //glBindVertexArray(0);
        //glUseProgram(0);
    }
    
    /**************************************************************************
    *!
    \fn     debug_draw_frustum

    \brief 
    debug draw for the frustum

    \param  frustum const& s
    the frustum

    \param  glm::vec4 color
    the color

    *
    **************************************************************************/
    void debug_draw_frustum(frustum const& s, glm::vec4 color)
    {
        //renderer& renderer = cs350::renderer::instance();
        //
        //shaderProgram& shader = renderer.getShader();
        //
        //camera& camera = renderer.camera();
        //
        //shader.use();
        //
        ////glm::mat4x4 m2w = camera.generatePerMatrix() * camera.generateCamMatrix();
        //glm::mat4x4 m2w(1);
        //
        //glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(m2w));
        //glUniform4fv(1, 1, &color[0]);
        //
        //
        ////binding the objects VAO
        //glBindVertexArray(renderer.mMeshes[0]->getVAO());
        //
        //// Drawing
        //glDrawArrays(GL_TRIANGLES, 0, renderer.mMeshes[0]->getDrawElements());
        //
        ////unbinding the VAOs
        //glBindVertexArray(0);
        //glUseProgram(0);
    }
    
    /**************************************************************************
    *!
    \fn     debug_draw_fancy

    \brief 
    debug function to render fancy(?) meshes

    \param  std::shared_ptr<mesh> const& mesh
    the mesh to render

    \param  glm::mat4 const& m2w
    the model to world matrix

    \param  glm::vec4 color
    the color

    \param  bool wire
    if wireframe or not

    *
    **************************************************************************/
    void debug_draw_fancy(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 color, bool wire)
    {
        if (wire)
        {
            debug_draw_segments(mesh, m2w, color);
            return;
        }

        //getting the instances
        shaderProgram& shader = cs350::renderer::instance().getShader();
        camera& camera = cs350::renderer::instance().camera();

        //setting the shader to use
        shader.use();

        //computing the world to viewport matrix
        glm::mat4x4 w2c = camera.projection() * camera.view() * m2w;

        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(w2c));
        glUniform4fv(1, 1, &color[0]);

        //binding the objects VAO
        glBindVertexArray(mesh->getVAO());

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, mesh->getDrawElements());

        //unbinding the VAOs
        glBindVertexArray(0);
        glUseProgram(0);

        debug_draw_segments(mesh, m2w, { 0,0,0,1 });
    }

    /**************************************************************************
    *!
    \fn     debug_draw_plain_color

    \brief 
    

    \param  std::shared_ptr<mesh> const& mesh
    the mesh to render

    \param  glm::mat4 const& m2w
    the model to world matrix

    \param  glm::vec4 const& color
    the color

    \param  unsigned mode
    the moode of the render

    *
    **************************************************************************/
    void debug_draw_plain_color(std::shared_ptr<mesh> const& mesh, glm::mat4 const& m2w, glm::vec4 const& color, unsigned mode)
    {
       
    }

    /**************************************************************************
    *!
    \fn     openglCallbackFunction

    \brief 
    The debug callback function

    \param  GLenum source
    the source of the error

    \param  GLenum type
    the error type

    \param  GLuint id
    the error id

    \param  GLenum severity
    the severity

    \param  GLsizei length
    

    \param  const GLchar* message
    

    \param  const void* userParam
    

    *
    **************************************************************************/
    void openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
    {
        

        //if is an error
        if (type == GL_DEBUG_TYPE_ERROR)
        {
            std::string _type = "ERROR";
            std::string _severity;

            //add the severity
            switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
                _severity = "HIGH";
                break;

            case GL_DEBUG_SEVERITY_MEDIUM:
                _severity = "MEDIUM";
                break;

            case GL_DEBUG_SEVERITY_LOW:
                _severity = "LOW";
                break;

            default:
                _severity = "UNKNOWN";
                break;
            }

            //print the error
            printf("%d: %s of %s severity, raised because: %s\n",
                id, _type.c_str(), _severity.c_str(), message);

            //abort after the print
            assert(type != GL_DEBUG_TYPE_ERROR);
        }

    }

}
