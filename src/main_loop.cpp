#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "main_loop.h"
#include "input.h"
#include "Globals.h"
#include "window.h"
#include "clock.h"
#include "shaders.h"
#include "camera.h"
#include "mesh.h"
#include "render.h"
#include "util.h"
#include "kdn_math.h"
#include "scene_loader.h"
#include "neural.h"

void Instantiate_NN(int n)
{
	if (net) {
		delete net;
		net = nullptr;
	}
	net = new nn::NeuralNet(n);
	net->SetGamma(nn::Sigmoid);
	net->SetTarget(nn::T2);
	net->ScrambleWeights();
	net->PrecomputeAgainstTarget();
	net->Error();
}


void main_init()
{
	Instantiate_NN(20);
}

bool main_loop()
{
	//Imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
		ImGui::Begin("Editor");
			ImGui::ColorEdit3("clear color", (float*)&Globals.clear_color);
			ImGui::Separator();

			float tempFloat = (float)net->learningRate;
			ImGui::InputFloat("Learning Rate", &tempFloat, 0, 0, "%.9f");
			net->learningRate = (prec)tempFloat;

			if (ImGui::Button("Scramble Weights")) {
				net->ScrambleWeights();
				net->PrecomputeAgainstTarget();
				net->Error();
			}
			static bool autoUpdate = true;
			ImGui::Checkbox("Auto", &autoUpdate);
			ImGui::SameLine();
			if (ImGui::Button("Update") || autoUpdate)
				net->Update();
			ImGui::Text("Error: %f", net->computedError);
		ImGui::End();

		ImGui::Begin("Weights");
			int tempWidth = net->GetWidth();
			ImGui::InputInt("Neurons", &tempWidth);
			if (tempWidth != net->GetWidth() && tempWidth >= 0)
				Instantiate_NN(tempWidth);

			ImGui::Text("W0: %f", (float)net->GetWeight(0));
			for (int i = 0; i < net->GetWidth(); i++)
			{
				ImGui::Text("W%i: %f", i + 1, (float)net->GetWeight(i));
			}
		ImGui::End();

		ImGui::Begin("Target Function");
			#define SwitchTarget(name, func) if(ImGui::Button(#name)) net->SetTarget(nn::func);
			SwitchTarget(X^2, T2);
			SwitchTarget(L, Oot);
			SwitchTarget(Sine, DenseSinGamma);
			SwitchTarget(2 Steps, Steps2);
			SwitchTarget(4 Steps, Steps4);
			SwitchTarget(Snake, Snake);
		ImGui::End();

		ImGui::Begin("Activation Function");
			#define SwitchGamma(name, func) if(ImGui::Button(#name)) net->SetGamma(nn::func);
			SwitchGamma(Sigmoid, Sigmoid);
			tempFloat = (float)nn::Alpha;
			ImGui::SliderFloat("Alpha", &tempFloat, 0.0f, 15.0f);
			nn::Alpha = (prec)tempFloat;
			ImGui::Separator();
			SwitchGamma(Sine, SinGamma);
			SwitchGamma(Rectified, ReLU);
			SwitchGamma(Y = X, Direct);
		ImGui::End();
	ImGui::Render();

	//update systems
	Input->Update(0);
	clock.recompute_delta_time();
	Camera::get()->Update();

	//rename window
	std::stringstream ss;
	ss << "MAT362 Neural Demo (" << (int)clock.fps() << "fps)";
	mainWindow->change_title(ss.str());

	//create grid debug lines
	DebugLine dbl;
	int gridSize = 20;
	glm::vec3 camPos(glm::floor(Camera::get()->position.x), glm::floor(Camera::get()->position.z), 0);
	for (int i = -gridSize; i <= gridSize; i++)
	{
		dbl.start = glm::vec3(i, -gridSize, 0) + camPos;
		dbl.end = glm::vec3(i, gridSize, 0) + camPos;
		dbl.startcolor = dbl.endcolor = glm::vec3(1, 0.3f, 0.7f);
		Renderer::get()->add_debug_line(dbl);

		dbl.start = glm::vec3(-gridSize, i, 0) + camPos;
		dbl.end = glm::vec3(gridSize, i, 0) + camPos;
		dbl.startcolor = dbl.endcolor = glm::vec3(0.7f, 0.3f, 1);
		Renderer::get()->add_debug_line(dbl);
	}

	//draw origin
	for (int i = 0; i < 50; i++)
	{
		dbl.start = glm::vec3(0, 0, 0);
		dbl.end = glm::rotate(glm::vec3(0.025, 0, 0), 2.0f * (float)pi_const * (float)i / 50, glm::vec3(0,0,1));
		dbl.startcolor = dbl.endcolor = glm::vec3(0, 0, 1);
		Renderer::get()->add_debug_line(dbl);
	}

	//Draw the net and target lines
	int segments = 200;
	for (int i = 0; i < segments - 1; i++)
	{
		float t = (float)i / (float)segments;
		float t1 = (float)(i + 1) / (float)segments;

		dbl.start = glm::vec3(t, net->Compute(t), 0.002);
		dbl.end = glm::vec3(t1, net->Compute(t1), 0.002);
		dbl.startcolor = dbl.endcolor = glm::vec3(1, 0, 0);
		Renderer::get()->add_debug_line(dbl);

		dbl.start = glm::vec3(t, net->GetTarget()(t), 0.001);
		dbl.end = glm::vec3(t1, net->GetTarget()(t1), 0.001);
		dbl.startcolor = dbl.endcolor = glm::vec3(0, 1, 0);
		Renderer::get()->add_debug_line(dbl);
	}

	//finish render
	Renderer::get()->Update();

	return Input->IsTriggered(Keys::Escape) || glfwWindowShouldClose(mainWindow->getGLFWwindow());
}