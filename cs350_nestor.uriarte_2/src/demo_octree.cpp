/**
* @file		 demo_octree.cpp
* @author	 Nestor Uriarte,  nestor.uriarte@digipen.edu
* @date		 Sat Nov  7 22:16:50 2020
* @brief	 Contains the implementation of the demo functions
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/

#include "pch.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "opengl.hpp"
#include "renderer.hpp"
#include "imgui.hpp"
#include "demo_octree.hpp"

namespace cs350 {

    namespace {

        /**
         * @brief
		 *  Renders a mesh using the plain color shader. (specific to this demo)		 
		 * @param mesh
		 * @param m2w
		 * @param color
		 */
        void debug_draw_phong(std::shared_ptr<mesh> const& mesh, const glm::mat4& m2w, glm::vec4 const& color)
        {
            // Objects
            auto& inst   = renderer::instance();
            auto& shd    = inst.resources().shaders.phong;
            auto& camera = inst.camera();

            // Parameters
            auto const& v              = camera.view();
            auto const& p              = camera.projection();
            auto        mv             = v * m2w;
            auto        mvp            = p * mv;
            auto        normal_matrix  = glm::mat3(glm::transpose(glm::inverse(mv)));
            auto        view_pos       = glm::vec3(500, 500, 100);
            auto        view_light_pos = v * glm::vec4(view_pos, 1);

            // OpenGL set
            shd.use();
            shd.set_uniform("uniform_mvp", mvp);                                  // Note: expensive
            shd.set_uniform("uniform_color", color);                              // Note: expensive
            shd.set_uniform("uniform_mv", mv);                                    // Note: expensive
            shd.set_uniform("uniform_normal_matrix", normal_matrix);              // Note: expensive
            shd.set_uniform("uniform_view_light_pos", glm::vec3(view_light_pos)); // Note: expensive

            // Draw
            mesh->bind();
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mesh->getDrawElements()));
        
            glUseProgram(0);
        }

        /**
		 * @param window
		 * @param key
		 * @param scancode
		 * @param action
		 * @param mods
		 */
        void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            // ImGui has priority
            if (ImGui::GetIO().WantCaptureKeyboard) {
                return;
            }

            auto demo = reinterpret_cast<demo_octree*>(glfwGetWindowUserPointer(renderer::instance().window().handle()));
            if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
                demo->options().physics_enabled ^= 1u;
            }
            if (key == 'R' && action == GLFW_PRESS) {
                demo->destroy();
            }
        }

        /**
		 *
		 * @param window
		 * @param button
		 * @param action
		 * @param mods
		 */
        void mouse_callback(GLFWwindow* window, int button, int action, int mods)
        {
            // ImGui has priority
            if (ImGui::GetIO().WantCaptureMouse) {
                return;
            }

            auto demo = reinterpret_cast<demo_octree*>(glfwGetWindowUserPointer(renderer::instance().window().handle()));

            // Shoot on click
            if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
                demo->shoot(glm::linearRand(1.0f, 5.0f));
            }
            if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS) {
                demo->shoot(0.0f);
            }
        }
    }

    /**
	 * @brief
	 */
    demo_octree::~demo_octree()
    {
        destroy();
    }

    /**
	 *
	 */
    void demo_octree::create()
    {
        { // Window/GFX
            renderer::instance().create(1280, 720, "CS350", false);

            // ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(renderer::instance().window().handle(), true);
            ImGui_ImplOpenGL3_Init("#version 150");

            // Input
            glfwSetWindowUserPointer(renderer::instance().window().handle(), this);
            glfwSetKeyCallback(renderer::instance().window().handle(), key_callback);
            glfwSetMouseButtonCallback(renderer::instance().window().handle(), mouse_callback);
        }

        // Octree
        m_octree_dynamic.Initialize(1 << m_options.octree_size_bit, m_options.octree_levels);
    }

    /**
	 *
	 * @return true Everything is ok
	 * @return false Window closed
	 */
    bool demo_octree::update()
    {
        auto& window = renderer::instance().window();
        if (!window.update()) {
            return false;
        }

        // Frame start
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glDepthMask(GL_TRUE);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, window.size().x, window.size().y);

        float dt = ImGui::GetIO().DeltaTime;

        // Camera update
        update_camera(dt);

        // Physics update
        for (auto* obj : m_dynamic_objects) {
            if (m_options.physics_enabled) {
                // Phy
                obj->position = obj->position + obj->velocity * dt;
            }

            // Clamping and bouncing on walls
            float boundary = m_octree_dynamic.root_size() * 0.5f;
            boundary -= 5.0f; // Make them bounce before boundary
            for (int i = 0; i < 3; ++i) {
                if (obj->position[i] > boundary && obj->velocity[i] > 0) {
                    obj->position[i] = boundary;
                    obj->velocity[i] *= -1;
                }
                if (obj->position[i] < -boundary && obj->velocity[i] < 0) {
                    obj->position[i] = -boundary;
                    obj->velocity[i] *= -1;
                }
            }

            // BV (Compute BV)
            obj->bv_world = aabb(obj->position, obj->radius);

            // Debug draw BV
            glDisable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            //computing the m2w matrix
            glm::mat4x4 m2w = glm::translate(obj->position);
            glm::vec3 scale = obj->bv_world.mMax - obj->bv_world.mMin;
            m2w = glm::scale(m2w, glm::vec3(scale.x, scale.y, scale.z));
            
            //render call
            debug_draw_fancy(obj->bv_world.mMesh, m2w, {1,1,1,0.5f}, false);

            {   // Octree update
                //getting the node for the object
                octree<physics_object>::node* node = m_octree_dynamic.find_create_node(obj->bv_world);
                
                //if the object hasn't a node assigned
                if (obj->octree_node == nullptr)
                {
                    //setting the prev and next as null
                    obj->octree_next_object = nullptr;
                    obj->octree_prev_object = nullptr;

                    //if the node already has a list
                    if (node->first != nullptr)
                    {
                        //pushing the node to the front
                        node->first->octree_prev_object = obj;

                        obj->octree_next_object = node->first;

                    }
                    //setting the start of the list as the obj
                    node->first = obj;

                    //setting that the obj belongs to this node
                    obj->octree_node = node;
                }
                else
                {
                    //if is a different node
                    if (node->locational_code != obj->octree_node->locational_code)
                    {
                        //erasing it from the node list
                        
                        //if the obj is at the start 
                        if (obj->octree_node->first == obj)
                        {
                            //move the start pointer to the next
                            obj->octree_node->first = obj->octree_next_object;

                            if(obj->octree_node->first != nullptr)
                                obj->octree_node->first->octree_prev_object = nullptr;

                        }
                        else
                        {
                            //getting previous and next objs
                            physics_object* previous = obj->octree_prev_object;
                            physics_object* next = obj->octree_next_object;

                            //linking the list
                            previous->octree_next_object = next;
                            
                            //if it has a next obj
                            if(next != nullptr)
                                next->octree_prev_object = previous;
                        }

                        //setting pointers to null
                        obj->octree_next_object = nullptr;
                        obj->octree_prev_object = nullptr;

                        //if the node already has a list
                        if (node->first != nullptr)
                        {
                            //pushing the node to the front
                            node->first->octree_prev_object = obj;

                            obj->octree_next_object = node->first;

                        }
                        //setting the start of the list as the obj
                        node->first = obj;

                        //storing the previous node
                        octree<physics_object>::node* oldNode = obj->octree_node;

                        //if the previous node is empty delete it
                        if (oldNode->first == nullptr && oldNode->children_active == 0)
                            m_octree_dynamic.delete_node(oldNode->locational_code);

                        //setting that the obj belongs to this node
                        obj->octree_node = node;
                    }
                }

            }
        }

        // Render
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for (auto* obj : m_dynamic_objects) {
            auto  m2w   = glm::translate(obj->position) * glm::scale(glm::vec3(obj->radius));
            auto& mesh  = renderer::instance().resources().meshes.sphere;
            auto  color = glm::vec4(0.5, 0.5, 0.5, 1);

            // Highlight object?
            if (m_options.highlight_level != -1 &&
                obj->octree_node &&
                locational_code_depth(obj->octree_node->locational_code) == m_options.highlight_level) {
                color = {0, 1, 0, 1};
            }
            debug_draw_phong(mesh, m2w, color);
        }

        if (m_options.debug_octree) {

            glCullFace(GL_FRONT);
            glEnable(GL_BLEND);
            glLineWidth(2.0F);

            // Debug draw nodes of the selected highlight-level
            std::vector<octree<physics_object>::node*> levelNodes;

            //getting the node
            m_octree_dynamic.GetNodesOfLevel(m_options.highlight_level, levelNodes);

            //for each node
            for (unsigned i = 0; i < levelNodes.size(); i++)
            {
                //compute the bv
                aabb bv = compute_bv(levelNodes[i]->locational_code, m_octree_dynamic.root_size());

                //getting th scale and the position of the bv
                glm::vec3 scale = bv.mMax - bv.mMin;
                glm::vec3 centre = (bv.mMax + bv.mMin) / 2.0F;

                //creating the m2w matrix
                glm::mat4x4 m2w = glm::translate(centre);
                m2w = glm::scale(m2w, glm::vec3(scale.x, scale.y, scale.z));

                //render call
                debug_draw_fancy(bv.mMesh, m2w, { 0, 0, 1, 1 });
            }

            glCullFace(GL_BACK);
            glDisable(GL_BLEND);
            glLineWidth(1);
        }

        { // All pairs debug
            if (m_options.brute_force) {
                for (size_t i = 0; i < m_dynamic_objects.size(); ++i) {
                    for (size_t j = i + 1; j < m_dynamic_objects.size(); ++j) {
                        check_intersection(m_dynamic_objects[i], m_dynamic_objects[j]);
                    }
                }
            } else {
                // Octree all pairs
                check_intersection_octree(0b1);

            }
        }

        // Help
        if (ImGui::Begin("Help")) {
            ImGui::Text("Help: \n"
                        "\tLook:      Right click + mouse\n"
                        "\tMove:      WASD\n"
                        "\tMove fast: SHIFT+WASD\n"
                        "\tPause:     SPACE\n"
                        "\tRecreate:  r");

            if (ImGui::SliderInt("Octree size", &m_options.octree_size_bit, 1, 32)) {
                // If octree size changes, orphan everything (forces reinsertion)
                m_octree_dynamic.destroy();
                m_octree_dynamic.set_root_size(1u << m_options.octree_size_bit);
                for (auto obj : m_dynamic_objects) {
                    obj->octree_node        = nullptr;
                    obj->octree_next_object = nullptr;
                    obj->octree_prev_object = nullptr;
                }
            }

            if (ImGui::SliderInt("Octree levels", &m_options.octree_levels, 1, 10)) {
                // If octree max levels changes, orphan everything (forces reinsertion)
                m_octree_dynamic.destroy();
                m_octree_dynamic.set_levels(m_options.octree_levels);
                for (auto obj : m_dynamic_objects) {
                    obj->octree_node        = nullptr;
                    obj->octree_next_object = nullptr;
                    obj->octree_prev_object = nullptr;
                }
            }

            ImGui::SliderInt("Highlight level", &m_options.highlight_level, -1, m_options.octree_levels);

            ImGui::Checkbox("Octree debug render", &m_options.debug_octree);
            ImGui::Checkbox("Pair debug render", &m_options.debug_intersections);
            ImGui::Checkbox("Physics enabled", &m_options.physics_enabled);
            ImGui::Checkbox("Brute force", &m_options.brute_force);
            if (ImGui::Button("Random")) {
                for (int i = 0; i < 10; ++i) {
                    float boundary = m_octree_dynamic.root_size();
                    boundary -= 5.0f; // Make them not bounce outside
                    auto  p = glm::linearRand(glm::vec3(boundary) * -0.5f, glm::vec3(boundary) * 0.5f);
                    auto  v = glm::ballRand(glm::linearRand(1.0f, 5.0f));
                    float r = glm::linearRand(0.5f, 2.0f);

                    // Create
                    auto obj      = new physics_object;
                    obj->position = p;
                    obj->velocity = v;
                    obj->radius   = r;

                    m_dynamic_objects.push_back(obj);
                }
            }

            // Keep track of checks
            m_options.checks_history.push_back((float)m_options.checks_this_frame);
            if (m_options.checks_history.size() > 500) {
                m_options.checks_history.erase(m_options.checks_history.begin());
            }
            m_options.checks_this_frame = 0;

            ImGui::Text("Objects: %d", int(m_dynamic_objects.size()));
            ImGui::Text("Intersection checks: %d", int(m_options.checks_history.back()));
            ImGui::PlotLines("", m_options.checks_history.data(), m_options.checks_history.size(), 0, "", 0, FLT_MAX, ImVec2(0, 64));
            ImGui::Text("Max: %d", static_cast<int>(*std::max_element(m_options.checks_history.begin(), m_options.checks_history.end())));
        }
        ImGui::End();

        // Frame end
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window.handle());
        glfwPollEvents();

        return true;
    }

    /**
	 * @brief
	 */
    void demo_octree::destroy()
    {

    }

    /**
	 * @brief
	 */
    void demo_octree::shoot(float v)
    {
        auto const& camera          = renderer::instance().camera();
        auto        camera_position = camera.position();
        auto        camera_dir      = camera.viewVec();

        auto obj = new physics_object;

        obj->position = camera_position;
        obj->velocity = camera_dir * v;
        obj->radius   = glm::linearRand(0.5f, 2.0f);

        m_dynamic_objects.push_back(obj);
    }

    /**
	 * @brief
     *  Placeholder intersection test, only draws and keeps track of statistics
	 * @param a
	 * @param b
	 */
    void demo_octree::check_intersection(physics_object const* a, physics_object const* b)
    {
        if (m_options.debug_intersections) {
            if (m_options.highlight_level == -1 ||
                (b->octree_node && locational_code_depth(b->octree_node->locational_code) == m_options.highlight_level) ||
                (a->octree_node && locational_code_depth(a->octree_node->locational_code) == m_options.highlight_level)) {
                glDisable(GL_CULL_FACE);
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDepthMask(GL_FALSE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                debug_draw_segment({a->position + glm::vec3{0, a->radius, 0}, b->position}, {1, 0, 1, 0.25f});
            }
        }
        m_options.checks_this_frame++;
    }

    /**
     * @brief   recursive iteration checking pairs of nodes
     * @param   uint32_t locational
     * @return  std::vector<physics_object*>
     */
    std::vector<physics_object*> demo_octree::check_intersection_octree(uint32_t locational)
    {
        //vectors to store the objects
        std::vector<physics_object*> objects;
        std::vector<physics_object*> childObjs;

        //checkign the locational code is valid
        if (locational_code_depth(locational) > m_options.octree_levels)
            return objects;

        octree<physics_object>::node* current = m_octree_dynamic.find_node(locational);

        //if the node exists
        if (current == nullptr)
            return objects;

        //getting the first object
        physics_object* list1 = current->first;

        //if there are objects on the list
        if (list1 != nullptr)
        {
            //adding them to the vector
            while (list1 != nullptr)
            {
                objects.push_back(list1);

                list1 = list1->octree_next_object;
            }
        }

        //recursive call to check through all the active childs
        for (int i = 0; i < 8; i++)
        {
            if ((current->children_active & (1 << i)))
            {
                //gettingthe childs locational code
                uint32_t childCode = (locational << 3) | i;

                //getting the returned vector
                std::vector<physics_object*> returned = check_intersection_octree(childCode);

                //inserting the to the vector
                childObjs.insert(childObjs.end(), returned.begin(), returned.end());
            }
        }

        //checking collision in my list
        for (unsigned i = 0; i < objects.size(); i++)
        {
            for (unsigned j = i + 1; j < objects.size(); j++)
            {
                check_intersection(objects[i], objects[j]);
            }
        }

        //checking collision with my objs and the ones in my childs
        for (unsigned i = 0; i < objects.size(); i++)
        {
            for (unsigned j = 0; j < childObjs.size(); j++)
            {
                check_intersection(objects[i], childObjs[j]);
            }
        }

        //insrting them to the main vector
        objects.insert(objects.end(), childObjs.begin(), childObjs.end());
     
        //returinig the a vector conatining all the objects from my list and my childs lists
        return objects;

    }

    /**
     * @brief
     *  Provided camera controls
     * @param dt 
     */
    void demo_octree::update_camera(float dt)
    {
        auto& camera          = renderer::instance().camera();
        auto& window          = renderer::instance().window();
        auto* glfwWindow      = window.handle();

        // Cursor
        double cursor_x = 0.0;
        double cursor_y = 0.0;

        glfwGetCursorPos(glfwWindow, &cursor_x, &cursor_y);
        { // Input
            if (glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_2)) {

                float speed = 10.0f;

                if (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT)) {
                    speed *= 4.0f;
                }

                camera.set_speed(speed);

                // Move
                if (glfwGetKey(glfwWindow, GLFW_KEY_W)) {
                    camera.move(dt, camera::directions::Forward);
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_S)) {
                    camera.move(dt, camera::directions::Backward);
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_A)) {
                    camera.move(dt, camera::directions::Left);
                }
                if (glfwGetKey(glfwWindow, GLFW_KEY_D)) {
                    camera.move(dt, camera::directions::Right);
                }

                camera.rotate(static_cast<float>(cursor_x - m_cursor_pos.x), static_cast<float>(cursor_y - m_cursor_pos.y));

            }

            m_cursor_pos       = {(float)cursor_x, (float)cursor_y};
        }
    }
}
