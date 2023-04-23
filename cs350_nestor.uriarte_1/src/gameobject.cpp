/**
* @file	gameobject.cpp 
* @author Nestor Uriarte ,540000817, nestor.uriarte@digipen.edu
* @date	Fri Oct 23 20:19:07 2020  
* @brief	This file containst the implementation of the game object class The camera that will be used on the project.
* @copyright Copyright (C) 2020 DigiPen Institute of Technology .
*/
#include "gameobject.hpp"
#include "imgui.hpp"

/**
* @brief	Constructor of the class
* @param	meshType mesh
* @param	glm::vec3& pos
* @param	glm::vec3& scale
**/
cs350::GameObject::GameObject(meshType mesh, glm::vec3& pos, glm::vec3& scale) : mMesh(mesh), mPosition(pos), mScale(scale)
{
	//setting the name
	switch (mesh)
	{
	case cs350::meshType::bunny:
		mName = "Bunny";
		break;
	case cs350::meshType::cube:
		mName = "Cube";
		break;
	case cs350::meshType::cylinder:
		mName = "Cylinder";
		break;
	case cs350::meshType::gourd:
		mName = "Gourd";
		break;
	case cs350::meshType::icosahedron:
		mName = "Icosahedron";
		break;
	case cs350::meshType::line:
		mName = "Line";
		break;
	case cs350::meshType::octohedron:
		mName = "Octohedron";
		break;
	case cs350::meshType::quad:
		mName = "Quad";
		break;
	case cs350::meshType::segment:
		mName = "Segment";
		break;
	case cs350::meshType::sphere:
		mName = "Sphere";
		break;
	case cs350::meshType::sponza:
		mName = "Sponza";
		break;
	case cs350::meshType::suzzane:
		mName = "Suzzane";
		break;
	case cs350::meshType::triangle:
		mName = "triangle";
		break;
	default:
		mName = "GameObject";
		break;
	}

	//computing the m2w matrix
	mModel2World = glm::translate(pos);

	mModel2World = glm::scale(mModel2World, pos);
}

/**
* @brief	render function of the object
**/
void cs350::GameObject::render()
{
	//getting the renderer
	renderer& renderer = cs350::renderer::instance();

	//calling to render
	debug_draw_fancy(renderer.getMesh(mMesh), mModel2World, glm::vec4(1,1,1,0.5), false);
}

/**
* @brief	update function of the object
**/
void cs350::GameObject::update()
{
	//updating the m2w matrix
	mModel2World = glm::translate(mPosition);
	mModel2World = glm::scale(mModel2World, mScale);
}

/**
* @brief	getter for the m2w matrix
**/
glm::mat4x4 cs350::GameObject::model2world() const
{
	//returning the function
	return mModel2World;
}

/**
* @brief	Edit function for the object
* @param	int i
**/
bool cs350::GameObject::Edit(int i)
{
	ImGui::PushID(i);

	//making a tree to modify the objects transform
	if (ImGui::TreeNode("Transform"))
	{
		ImGui::DragFloat3("Position", glm::value_ptr(mPosition));
		ImGui::DragFloat3("Scale"   , glm::value_ptr(mScale));

		ImGui::TreePop();
	}

	ImGui::Separator();
	int prev = static_cast<int>(mMesh);

	int tag = static_cast<int>(mMesh);
	const char* dyn[13] = { "Bunny", "Cube", "Cylinder", "Gourd", "Icosahedron", "Line", "Octohedron", "Quad", "Segment", "Sphere", "Sponza", "Suzzane", "Triangle" };
	//to change the mesh
	if (ImGui::Combo(" ", &tag, dyn, 13, 14)) {
		switch (tag)
		{
		case 0:
			mMesh = meshType::bunny;
			break;
		case 1:
			mMesh = meshType::cube;
			break;
		case 2:
			mMesh = meshType::cylinder;
			break;
		case 3:
			mMesh = meshType::gourd;
			break;
		case 4:
			mMesh = meshType::icosahedron;
			break;
		case 5:
			mMesh = meshType::line;
			break;
		case 6:
			mMesh = meshType::octohedron;
			break;
		case 7:
			mMesh = meshType::quad;
			break;
		case 8:
			mMesh = meshType::segment;
			break;
		case 9:
			mMesh = meshType::sphere;
			break;
		case 10:
			mMesh = meshType::sponza;
			break;
		case 11:
			mMesh = meshType::suzzane;
			break;
		case 12:
			mMesh = meshType::triangle;
			break;
		}
	}

	//setting the name
	mName = dyn[tag];


	//if the meshes changed
	if (prev != tag)
	{
		ImGui::PopID();
		return true;
	}

	ImGui::PopID();
	return false;

}
