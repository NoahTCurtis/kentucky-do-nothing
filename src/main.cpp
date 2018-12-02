#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <cstdio>
#include <iostream>
#include <ctime>

#include "callbacks.h"
#include "Globals.h"
#include "input.h"
#include "main_loop.h"
#include "mesh.h"
#include "render.h"
#include "scene_loader.h"
#include "shaders.h"
#include "util.h"
#include "window.h"


//Globals here
struct GlobalData Globals;



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
	//make meshes
	///Mesh* newMesh = new Mesh("cube.obj");
	///Renderer::get()->add_mesh(newMesh);
	///newMesh->worldTransform.v = glm::vec3(0, 1, -1);
	///
	///ReadAssimpFile(get_full_file_path("samba.fbx", "res\\models"));

	//Main loop!
	main_init();
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

