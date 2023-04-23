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

***************************************************************************/


#include "pch.hpp"
#include "opengl.hpp"
#include "imgui.hpp"
#include "window.hpp"
#include "renderer.hpp"

namespace cs350{

	/**************************************************************************
	*!
	\fn     window::create

	\brief 
	Creates the window

	\param  int w
	the width of the window

	\param  int h
	the height of the window

	\param  const char* window_name
	the title for the window

	\param  bool hidden
	if we want to open it or not

	\return bool
	if it sucessfully was created or not

	*
	**************************************************************************/
	bool window::create(int w, int h, const char* window_name, bool hidden)
	{
		//initializing
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		if (hidden)
			glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

		//setting the size
		m_size = glm::ivec2(w, h);

		//creating the window
		m_window = glfwCreateWindow(w, h, window_name, NULL, NULL);

		//checking it was sucessfully created
		if (m_window == NULL)
		{
			std::cout << "Failed to create window" << std::endl;
			glfwTerminate();
			return false;
		}

		//initia;lizing the member variables
		mLastFrame = 0.0F;
		mMousePos = glm::vec2(0,0);

		//creating the context
		glfwMakeContextCurrent(m_window);

		//initializing glad
		assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != false);

		//etting up the opengl debug call back details
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);

		//setting the parameters of opengl
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glFrontFace(GL_CCW);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glPointSize(2.0F);

		//initializing imgui
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 130");

		//returning it was sucessfully created
		return true;
	}

	/**************************************************************************
	*!
	\fn     window::update

	\brief 
	The update function for the window

	\return bool
	boolean to notify the window was closed

	*
	**************************************************************************/
	bool window::update()
	{

		//if the window should close return false
		if (glfwWindowShouldClose(m_window) != false)
			return false;

		//handle input
		handleInput();

		//returning true to keep the loop going
		return true;
	}

	/**************************************************************************
	*!
	\fn     window::destroy

	\brief 
	Destroys the window

	*
	**************************************************************************/
	void window::destroy()
	{
		//terminating glfw
		glfwTerminate();
	}

	/**************************************************************************
	*!
	\fn     window::clear

	\brief 
	clears the window

	*
	**************************************************************************/
	void window::clear()
	{
		//clearing the color buffer to black
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	/**************************************************************************
	*!
	\fn     window::handleInput

	\brief 
	input handling function for the update

	*
	**************************************************************************/
	void window::handleInput()
	{
		camera& camera = cs350::renderer::instance().camera();

		//computing the delta time
		float currentFrame = static_cast<float>(glfwGetTime());
		float delta = currentFrame - mLastFrame;
		mLastFrame = currentFrame;

		//variables to store the mouse position
		double posx;
		double posy;

		//getting the mouse position
		glfwGetCursorPos(handle(), &posx, &posy);

		//if WASD is pressed move the camera 
		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_window, true);

		if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
			camera.move(delta, cs350::camera::directions::Forward);

		if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
			camera.move(delta, cs350::camera::directions::Left);

		if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
			camera.move(delta, cs350::camera::directions::Backward);

		if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
			camera.move(delta, cs350::camera::directions::Right);

		if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			camera.set_speed(5.0F);

		if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
			camera.set_speed(2.5F);
		

		//if the right click of the mose is pressed rotate the camera
		if (glfwGetMouseButton(handle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			//computing the offset on x and y
			float xoffset = static_cast<float>(posx) - mMousePos.x;
			float yoffset = mMousePos.y - static_cast<float>(posy); // reversed since y-coordinates go from bottom to top

			//rotate the camera
			camera.rotate(xoffset, yoffset);
		}

		//setting the mouse position
		setMousePos({ posx, posy });

	}

	/**************************************************************************
	*!
	\fn     window::setMousePos

	\brief 
	setter function for the mouse

	\param  glm::vec2 pos
	the position to set

	*
	**************************************************************************/
	void window::setMousePos(glm::vec2 pos)
	{
		mMousePos = pos;
	}

	
}
