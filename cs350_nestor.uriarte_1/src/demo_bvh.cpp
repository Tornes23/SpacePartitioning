/**
* @file	demo_bvh.cpp 
* @author Nestor Uriarte ,540000817, nestor.uriarte@digipen.edu
* @date	Fri Oct 23 20:18:42 2020  
* @brief	This file contains the demo of the BVH tree and the three algorithms showed in class The camera that will be used on the project.
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "pch.hpp"
#include <opengl.hpp>
#include <renderer.hpp>
#include <imgui.hpp>
#include <demo_bvh.hpp>
#include "mesh_data.hpp"
#include "mesh.hpp"

namespace cs350 {

/**
* @brief	Creates the demo
**/
    void demo_bvh::create()
    {
        //getting the renderer
        renderer& renderer = cs350::renderer::instance();

        //creating the demo and initializing the reqired variables
        renderer.create(1280, 720, "CS350", false);

        CreateEnviroment();

        GetTriangles();

        mMethod = TreeMethod::TopDown;

        switch (mMethod)
        {
        case cs350::TreeMethod::TopDown:
            mTopDown.Initialize(mTriangles);
            TopDown(mTopDown.GetRoot());
            break;
        case cs350::TreeMethod::BottomUp:
            BottomUp();
            break;
        case cs350::TreeMethod::Incremental:
            Incremental();
            break;
        default:
            mTopDown.Initialize(mTriangles);
            TopDown(mTopDown.GetRoot());
            break;
        }


        mRenderBVH = true;
        mRenderBVHTrangles = false;
    }

/**
* @brief	destroys the demo
**/
    void demo_bvh::destroy()
    {
        //getting the instance
        renderer& renderer = cs350::renderer::instance();

        //destroying
        renderer.destroy();
    }

/**
* @brief	updates the demo
**/
    bool demo_bvh::update()
    {
        //getting the instance
        renderer& renderer = cs350::renderer::instance();

        //getting the window
        window& window = renderer.window();
        camera& camera= renderer.camera();

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        //if the window is closed return fals to stop the demo
        if (window.update() == false)
            return false;

        window.clear();

        //updating the objects
        for (unsigned i = 0; i < mObjs.size(); i++)
            mObjs[i].update();

        //updating tthe camera
        camera.update();

        //rendering the scene
        render();

        //calling to edit the demo
        Edit();

        //rendering imgui
        renderGui();

        //swap the buffers
        glfwSwapBuffers(window.handle());

        //returning to continue the update
        return true;
    }

/**
* @brief	edit function to use imgui 
**/
    void demo_bvh::Edit()
    {
        //if the tree method has been modified recompute the tree
        if (EditBVH())
        {
            RecomputeTree();
        }

        //window to modify the game objects
        if (!ImGui::Begin("GameObjects"))
        {
            ImGui::End();
        }

        for (unsigned i = 0; i < mObjs.size(); i++)
        {
            //setting the tab title
            std::string title = mObjs[i].mName;

            //creating the tab
            if (ImGui::CollapsingHeader(title.c_str()))
            {
                //recomputing the hierarchy if the mesh gets modified
                if (mObjs[i].Edit(i))
                {
                    RecomputeTree();
                }
            }
        }

        ImGui::End();

    }

 /**
* @brief	renders the demo
**/
    void demo_bvh::render() 
    {
        //calling to render for each object
        for (unsigned i = 0; i < mObjs.size(); i++)
        {
            mObjs[i].render();
        }
    }

/**
* @brief	renders imgui
**/
    void demo_bvh::renderGui()
    {
        //calling to render imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

/**
* @brief	edir function for the BVH trees
**/
    bool demo_bvh::EditBVH()
    {
        //creating the window to edit the BVH
        if (!ImGui::Begin("Bounding Volume Hierarchy"))
        {
            ImGui::End();
        }

        int prev = static_cast<int>(mMethod);

        //buttons to select the tree method
        if (ImGui::RadioButton("TopDown", mMethod == TreeMethod::TopDown))
        {
            mMethod = TreeMethod::TopDown;
        }
        if (ImGui::RadioButton("BottomUp", mMethod == TreeMethod::BottomUp))
        {
            mMethod = TreeMethod::BottomUp;
        }
        if (ImGui::RadioButton("Incremental", mMethod == TreeMethod::Incremental))
        {
            mMethod = TreeMethod::Incremental;
        }

        int curr = static_cast<int>(mMethod);

        //booleans to know if we have to render the triangles and the bounding volumes
        bool render = mRenderBVH;
        bool triangles = mRenderBVHTrangles;

        //checkboxes to modify the flags
        ImGui::Checkbox("Render Triangles", &triangles);

        ImGui::SameLine();

        ImGui::Checkbox("Render Volumes", &render);

        mRenderBVH = render;
        mRenderBVHTrangles = triangles;

        ImGui::Separator();

        //tab which will show the root
        if (ImGui::TreeNode("Root"))
        {
            Node<triangle>* root = nullptr;

            //based on the current method getting the root of the proper tree
            switch (mMethod)
            {
            case cs350::TreeMethod::TopDown:
                root = mTopDown.GetRoot();
                break;
            case cs350::TreeMethod::BottomUp:
                root = mBottomUp.GetRoot();
                break;
            case cs350::TreeMethod::Incremental:
                root = mIncremental.GetRoot();
                break;
            default:
                root = mTopDown.GetRoot();
                break;
            }

            //if the root is null end
            if (root == nullptr)
            {
                ImGui::TreePop();
                ImGui::End();
                return false;
            }

            ImGui::SameLine();

            //buuton to make the bv and triangles visible
            if (ImGui::Button("Visible"))
            {
                root->mVisible = !root->mVisible;
            }
            
            //if we want to render, do it
            if(render)
                if (root->mVisible)
                    RenderBVH(root, triangles);
           
            //calling to edit the node
            EditNode(root, render, triangles);
            ImGui::TreePop();
        }

      
        ImGui::End();

        //if there was a change in the trees return true
        if (prev != curr)
            return true;

        return false;
    }

/**
* @brief	renders a node of the BVH tree
* @param    Node<triangle>* target
* @param    bool triangles
**/
    void demo_bvh::RenderBVH(Node<triangle>* target, bool triangles)
    {
        //getting the abb of the node
        aabb box = ComputeAABB(target->mInside);

        //computing the center of the aabb
        glm::vec3 center = (box.mMin + box.mMax) / 2.0F;

        float scaleX = box.mMax.x - box.mMin.x;
        float scaleY = box.mMax.y - box.mMin.y;
        float scaleZ = box.mMax.z - box.mMin.z;

        //create the m2w
        glm::mat4x4 m2w = glm::translate(center);
        m2w = glm::scale(m2w, glm::vec3(scaleX, scaleY, scaleZ));

        //drawing the aabb
        debug_draw_segments(box.mMesh, m2w,glm::vec4(1.0F, 0.0F, 0.0F, 0.3F));
        
        //if we want to render the affected triangles
        if (triangles)
        {
            //render each triangle
            for (unsigned i = 0; i < target->mInside.size(); i++)
            {
                debug_draw_triangle(target->mInside[i], glm::vec4(0.0F, 0.0F, 1.0F, 0.25F));
            }
        }
    }

/**
* @brief	Edit function for a node of the tree
* @param    Node<triangle>* current
* @param    bool render
* @param    bool triangles
**/
    void demo_bvh::EditNode(Node<triangle>* current, bool render, bool triangles)
    {
        //if the node is null
        if (current == nullptr)
            return;

        //the triangle count on this node
        ImGui::Text("Triangle Count = %d", current->mInside.size());

        //if we want to render it
        if (render)
            if(current->mVisible)
                RenderBVH(current, triangles);

        ImGui::Separator();

        //showing the left node
        if (ImGui::TreeNode("Left"))
        {
            //if the left node is not nullptr
            if (current->mLeft != nullptr)
            {
                ImGui::SameLine();

                //button to make it visible
                if (ImGui::Button("Visible"))
                {
                    current->mLeft->mVisible = !current->mLeft->mVisible;
                }

                //calling to edit
                EditNode(current->mLeft, render, triangles);
            }

            ImGui::TreePop();
        }

        ImGui::Separator();

        //showing the right node
        if (ImGui::TreeNode("Right"))
        {
            //if the right node is not nullptr
            if (current->mRight != nullptr)
            {
                ImGui::SameLine();

                //button to make it visible
                if (ImGui::Button("Visible"))
                {
                    current->mRight->mVisible = !current->mRight->mVisible;
                }

                //calling to edit
                EditNode(current->mRight, render, triangles);
            }

            ImGui::TreePop();
        }
    }

/**
* @brief	recomputes trees
**/
    void demo_bvh::RecomputeTree()
    {
        //clearing the containers and trees and recomputing them based on the selected method
        mTriangles.clear();

        GetTriangles();

        switch (mMethod)
        {
        case cs350::TreeMethod::TopDown:
            mTopDown.clear();
            mTopDown.Initialize(mTriangles);
            TopDown(mTopDown.GetRoot());
            break;
        case cs350::TreeMethod::BottomUp:
            mBottomUp.clear();
            BottomUp();
            break;
        case cs350::TreeMethod::Incremental:
            mIncremental.clear();
            Incremental();
            break;
        default:
            mTopDown.clear();
            mTopDown.Initialize(mTriangles);
            TopDown(mTopDown.GetRoot());
            break;
        }
    }

/**
* @brief	gets all the triangles on the demo
**/
    std::vector<triangle>& demo_bvh::GetTriangles()
    {
        //getting the renederer
        renderer& renderer = renderer::instance();

        //for each object add the model trianlges to the vector
        for (unsigned i = 0; i < mObjs.size(); i++)
        {

            auto& triangles = renderer.getMesh(mObjs[i].mMesh)->getTriangles();

            mTriangles.insert(mTriangles.end(), triangles.begin(), triangles.end());
        }

        //returning the vector
        return mTriangles;
    }

/**
* @brief	performs the partition
* @param    std::vector<triangle>& container
* @param    std::vector<triangle>& right
* @param    std::vector<triangle>& left
**/
    void demo_bvh::Partition(std::vector<triangle>& container, std::vector<triangle>& right, std::vector<triangle>& left)
    {
        //getting in which axis parition
        AXIS partition = GetPartitionAxis(container);

        //computing the bv
        aabb bounding = ComputeAABB(container);

        //the burning plane at a thousand degrees which will cut the bounding volume
        plane bpatd;
        
        //vec3 to store the centroid of the axis
        glm::vec3 centroid = glm::vec3(0,0,0);

        //computing the centroid
        for (unsigned i = 0; i < container.size(); i++)
            centroid += ComputeCentre(container[i]);

        centroid /= static_cast<float>(container.size());

        //setting the position of the plane
        bpatd.mPosition = centroid;

        //based on the axis in which we cut, set the normal vector
        switch (partition)
        {
        case cs350::AXIS::X:
            bpatd.mNormal = glm::vec3(1.0F, 0.0F, 0.0F);
            break;
        case cs350::AXIS::Y:
            bpatd.mNormal = glm::vec3(0.0F, 1.0F, 0.0F);
            break;
        case cs350::AXIS::Z:
            bpatd.mNormal = glm::vec3(0.0F, 0.0F, 1.0F);
            break;
        default:
            break;
        }

        //for each triangle on the node
        for (unsigned i = 0; i < container.size(); i++)
        {
            //classify if is at the left or right
            classification_t result = classify_plane_triangle(bpatd, container[i], cEpsilon);

            //based on the result push it to a container
            if (result == classification_t::inside)
                left.push_back(container[i]);
            else if(result == classification_t::outside)
                right.push_back(container[i]);
            else
            {   
                //if pverlaps get the centre of the triangle and choose based on the clasification of the centre
                glm::vec3 centre = ComputeCentre(container[i]);

                classification_t centreResult = classify_plane_point(bpatd, centre, cEpsilon);

                if (centreResult == classification_t::inside)
                    left.push_back(container[i]);
                else if (centreResult == classification_t::outside)
                    right.push_back(container[i]);
            }
        }

    }

/**
* @brief	performs top down algorithm
* @param    Node<triangle>* current
**/
    void demo_bvh::TopDown(Node<triangle>* current)
    {
        //if the node is null
        if (current == nullptr)
            return;

        //if the triangle count on the container is equal to the minimun return
        if (current->mInside.size() <= MIN_TRIANGLES)
            return;

        //fill up the tree recursively
        std::vector<triangle> leftNode;
        std::vector<triangle> rightNode;

        //perform the parition
        Partition(current->mInside, rightNode, leftNode);

        //checking that none of both containers are empty
        if (leftNode.size() == 0 || rightNode.size() == 0)
            return;

        //if the size is higher than the minimum triangles per node
        if (leftNode.size() > MIN_TRIANGLES)
        {
            //updating the type of node it is
            current->mType = Node<triangle>::NodeType::Node;

            //inserting the node on the tree
            mTopDown.InsertNode(current, &(current->mLeft), leftNode);
        }
        
        if (rightNode.size() > MIN_TRIANGLES)
        {
            //updating the type of node it is
            current->mType = Node<triangle>::NodeType::Node;

            //inserting the node on the tree
            mTopDown.InsertNode(current, &(current->mRight), rightNode);
        }

        //recursive call to the functio to continue with the left and right nodes
        TopDown(current->mLeft);
        TopDown(current->mRight);
    }

/**
* @brief	performs bottom up algorithm
**/
    void demo_bvh::BottomUp()
    {
        //creating a node per triangle
        std::vector<Node<triangle>*> nodes;
        nodes.resize(mTriangles.size());

        for (unsigned i = 0; i < nodes.size(); i++)
        {
            nodes[i] = new Node<triangle>();

            nodes[i]->mInside.push_back(mTriangles[i]);
        }

        int count = static_cast<int>(mTriangles.size());

        //while the node count is higher than 1
        while (count > 1)
        {
            //indexes for the left and right node
            int left;
            int right;

            //get closest nodes
            std::vector<triangle> linked = GetClosestNodes(nodes, count, &left, &right);

            //creating the new node
            Node<triangle>* addition = new Node<triangle>();

            assert(addition != nullptr);
            
            //setting the type, left and right nodes, and the triangle container
            addition->mType = Node<triangle>::NodeType::Node;

            addition->mLeft = nodes[left];
            addition->mRight = nodes[right];

            addition->mInside = linked;

            //updating the node container
            int min = left;
            int max = right;

            min = left > right ? right : left;
            max = left > right ? left : right;

            nodes[min] = addition;
            nodes[max] = nodes[count - 1];

            //decreasing the node count
            count--;

        }

        //initializing the tree with the last node
        mBottomUp.Initialize(&(nodes[0]));

    }

/**
* @brief	gets the colest node of a given one
* @param    std::vector<Node<triangle>*>& nodes
* @param    int count
* @param    int* left
* @param    int* right
**/
    std::vector<triangle> demo_bvh::GetClosestNodes(std::vector<Node<triangle>*>& nodes, int count, int* left, int* right)
    {
        //vector which will contain the final triangles
        std::vector<triangle> linked;

        //float of the area
        float area = std::numeric_limits<float>::max();

        //check each node with the other one and the one with minimum surface area store it
        for (int i = 0; i < count- 1; i++)
        {
            for (int j = i + 1; j < count; j++)
            {
                //temporal vector to store the fusion of the i and j nodes
                std::vector<triangle> temp;

                temp = nodes[i]->mInside;
                temp.insert(temp.end(), nodes[j]->mInside.begin(), nodes[j]->mInside.end());

                //computing the area of the container
                float currentArea = GetSurfaceArea(temp);
                
                //if is lower than the current one update the values
                if (currentArea < area)
                {
                    *left = i;
                    *right = j;
                    area = currentArea;
                }
            }
        }

        //storing the final triangles
        linked = nodes[*left]->mInside;
        linked.insert(linked.end(), nodes[*right]->mInside.begin(), nodes[*right]->mInside.end());

        //returning the  vector
        return linked;
    }

/**
* @brief	performs incremental algorithm
**/
    void demo_bvh::Incremental()
    {
        //creating a node per triangle
        std::vector<Node<triangle>*> nodes;
        nodes.resize(mTriangles.size());

        for (unsigned i = 0; i < nodes.size(); i++)
        {
            nodes[i] = new Node<triangle>();

            nodes[i]->mInside.push_back(mTriangles[i]);
        }

        //while the vector is not empty
        while (!nodes.empty())
        {
            //randomly get a node
            int index = static_cast<int>(glm::linearRand(0.0F, static_cast<float>(nodes.size())));

            //pointer to store the best node
            Node<triangle>* bestNode = nullptr;

            //value to store the surface area
            float surface = std::numeric_limits<float>::max();

            //getting the best node
            GetBestNode(nodes[index], mIncremental.GetRoot(), surface, &bestNode);

            //insert the node
            mIncremental.MergeNodes(&nodes[index], &bestNode);

            //removing the node from the vector
            nodes.erase(std::find(nodes.begin(), nodes.end(), nodes[index]));
        }

    }

/**
* @brief	gets best node given one
* @param    const Node<triangle>* target
* @param    Node<triangle>* current
* @param    float& area
* @param    Node<triangle>** bestNode
**/
    void demo_bvh::GetBestNode(const Node<triangle>* target, Node<triangle>* current, float& area, Node<triangle>** bestNode)
    {
        //if the current node or the one we want are null return
        if (target == nullptr || current == nullptr)
            return;

        //if the node type is nor a leaf 
        if (current->mType == Node<triangle>::NodeType::Node)
        {
            //recursive call with the left and right nodes
            GetBestNode(target, current->mLeft, area, bestNode);
            GetBestNode(target, current->mRight, area, bestNode);

            return;
        }

        //getting the merged triangle container
        std::vector<triangle> merged = target->mInside;
        merged.insert(merged.end(), current->mInside.begin(), current->mInside.end());

        //computing the surface area
        float currentArea = GetSurfaceArea(merged);

        //if the area is lower than the given one update the values
        if (currentArea < area)
        {
            area = currentArea;
            *bestNode = current;
            
            return;
        }
    }

/**
* @brief	computes aabb based on a container
* @param    std::vector<triangle>& toBound
**/
    aabb demo_bvh::ComputeAABB(std::vector<triangle>& toBound)
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
        for (unsigned i = 0; i < toBound.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    if (toBound[i][j][k] <= mins[k])
                        mins[k] = toBound[i][j][k];

                    if (toBound[i][j][k] >= maxs[k])
                        maxs[k] = toBound[i][j][k];
                }
            }

        }

        //setting the points
        bounding.mMin = glm::vec3(mins[0], mins[1], mins[2]);
        bounding.mMax = glm::vec3(maxs[0], maxs[1], maxs[2]);

        return bounding;
    }

/**
* @brief	computes te centreno of a triangle
* @param    triangle& triangle
**/
    glm::vec3 demo_bvh::ComputeCentre(triangle& triangle)
    {
        //vec3 to store the centre of the triangle
        glm::vec3 centre;

        //making an average on the x, y and z coords
        float centreX = (triangle.mV0.x + triangle.mV1.x + triangle.mV2.x) / 3.0F;
        float centreY = (triangle.mV0.y + triangle.mV1.y + triangle.mV2.y) / 3.0F;
        float centreZ = (triangle.mV0.z + triangle.mV1.z + triangle.mV2.z) / 3.0F;

        //setting the values
        centre = glm::vec3(centreX, centreY, centreZ);

        //returning the point
        return centre;
    }

/**
* @brief	gets the partition axis
* @param    std::vector<triangle>& container
**/
    AXIS demo_bvh::GetPartitionAxis(std::vector<triangle>& container)
    {
        //getting the aabb
        aabb bounding = ComputeAABB(container);

        //computing whixh axis has more differnece
        float diffX = bounding.mMax.x - bounding.mMin.x;
        float diffY = bounding.mMax.y - bounding.mMin.y;
        float diffZ = bounding.mMax.z - bounding.mMin.z;

        //based on the comparisons return the correct axis in which we need to partition
        if (diffX >= diffY)
        {
            if (diffX >= diffZ)
                return AXIS::X;
            else
                return AXIS::Z;
        }
        else
        {
            if (diffY >= diffZ)
                return AXIS::Y;
        }


        return AXIS::Z;
    }

/**
* @brief	computes the surface area given a container
* @param    std::vector<triangle>& container
**/
    float demo_bvh::GetSurfaceArea(std::vector<triangle>& container)
    {
        //if the container is empty return 0
        if (container.empty())
            return 0.0F;
        
        //compute the bounding volume of the container 
        aabb bv = ComputeAABB(container);

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
* @brief	creates the enviroment for the demo
**/
    void demo_bvh::CreateEnviroment()
    {
        //adding the game object
        mObjs.push_back(GameObject());
    }

}
