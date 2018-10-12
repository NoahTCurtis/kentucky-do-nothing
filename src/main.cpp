#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <cstdio>
#include <iostream>

#include "main_loop.h"
#include "input.h"
#include "window.h"
#include "shaders.h"
#include "Globals.h"
#include "callbacks.h"
#include "mesh.h"


//Globals here
struct GlobalData Globals;

float randFloat01()
{
	return (float)rand() / (float)RAND_MAX;
}

int main(void)
{
	//Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	///glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//Create a window, set context to current
	mainWindow = new Window(600, 800, "CS460 Advanced Demo");

	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		assert(false);
		return -1;
	}
	glViewport(0, 0, 800, 600);
	//glDebugMessageCallback
	glDebugMessageCallback(debug_callback, nullptr);
	glEnable(GL_DEPTH_TEST);

	//Set callbacks
	glfwSetFramebufferSizeCallback(mainWindow->getGLFWwindow(), framebuffer_size_callback);
	///glfwSetKeyCallback(mainWindow, input_key_callback);

	//Create Managers
	Input = new InputManager;
	Input->Initialize();
	Input->RegisterWindow(mainWindow->getGLFWwindow());

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(mainWindow->getGLFWwindow(), false);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

	//make a quick shader program (actually the renderer does this now)
	//make a default mesh
	meshes.push_back(new Mesh());
	meshes.push_back(new Mesh());
	(*meshes.begin())->worldPosition = glm::vec3(0, 1, -1);

	//Main loop!
	while ( main_loop() == false );

	// Cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//Exit safely
	delete mainWindow;
	glfwTerminate();
	return 0;
}

