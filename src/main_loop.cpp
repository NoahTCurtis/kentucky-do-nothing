#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
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


bool main_loop()
{
	//Imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
		ImGui::Begin("Editor");
			ImGui::ColorEdit3("clear color", (float*)&Globals.clear_color);
			if (ImGui::Button("Randomize Mesh positions"))
			{
				for (auto& mesh : Meshes)
					mesh->worldPosition = glm::vec3(randRange(-5, 5), randRange(-5, 5), randRange(-5, 5));
			}
		ImGui::End();

		ImGui::Begin("Meshes");
			ImGui::TextColored(ImVec4(1,0,1,1), "There are %i meshes", Meshes.size());
			int i = 1;
			for(auto& mesh : Meshes)
			{
				ImGui::Separator();
				ImGui::Text("Mesh #%i", i);
				ImGui::Text("%i verts, %i tris", mesh->vertCount, mesh->indexCount / 3);
				ImGui::InputFloat3("Position", reinterpret_cast<float*>(&mesh->worldPosition), 2);
				ImGui::Checkbox("Visible", &mesh->visible);
				i++;
			}
		ImGui::End();
	ImGui::Render();

	///ImGui_ImplOpenGL3_NewFrame();
	///ImGui_ImplGlfw_NewFrame();
	///ImGui::NewFrame();
	///	ImGui::Begin("Meshes");
	///		ImGui::ColorEdit3("clear color", (float*)&Globals.clear_color);
	///	ImGui::End();
	///ImGui::Render();

	//Reload Shaders
	if (Input->IsTriggered(Keys::F9))
	{
		glUseProgram(0);
		glDeleteProgram(Globals.mesh_shader_program_name);
		Globals.mesh_shader_program_name = create_shader_program("vert.shader", "frag.shader");
	}

	//update systems
	Input->Update(0);
	clock.recompute_delta_time();
	Camera::get()->Update();

	//reanem window
	std::stringstream ss;
	ss << clock.fps() << " (" << (int)(1.0f / clock.dt()) << ")";
	mainWindow->change_title(ss.str());

	//clear screen
	glClearColor(Globals.clear_color.x, Globals.clear_color.y, Globals.clear_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//push world2cam to vert shader
	glUseProgram(Globals.mesh_shader_program_name);
	glm::mat4 world2camMat = Camera::get()->get_world_to_camera_matrix();
	glUniformMatrix4fv(glGetUniformLocation(Globals.mesh_shader_program_name, "world2cam"), 1, GL_FALSE, &world2camMat[0][0]);

	//push cam2persp to vert shader
	glm::mat4 perspectiveMat = mainWindow->compute_perspective_matrix(0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(Globals.mesh_shader_program_name, "perspective"), 1, GL_FALSE, &perspectiveMat[0][0]);

	//draw meshes
	glUseProgram(Globals.mesh_shader_program_name);
	for (auto& mesh : Meshes)
	{		
		if (mesh->visible == false) continue;
		mesh->bind();
		glm::mat4 model2world = mesh->get_model_to_world_matrix();
		glUniformMatrix4fv(glGetUniformLocation(Globals.mesh_shader_program_name, "model2world"), 1, GL_FALSE, &model2world[0][0]);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
	}

	//draw debug lines
	DebugLine dbl;
	int gridSize = 7;
	for (int i = -gridSize; i <= gridSize; i++)
	{
		dbl.start = glm::vec3(i, 0, -gridSize);
		dbl.end = glm::vec3(i, 0, gridSize);
		dbl.startcolor = dbl.endcolor = glm::vec3(1, 0.3f, 0.7f);
		Renderer::get()->add_debug_line(dbl);
		dbl.start = glm::vec3(-gridSize, 0, i);
		dbl.end = glm::vec3(gridSize, 0, i);
		dbl.startcolor = dbl.endcolor = glm::vec3(0.7f, 0.3f, 1);
		Renderer::get()->add_debug_line(dbl);
	}

	Renderer::get()->render_debug_lines();

	//finish render
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(mainWindow->getGLFWwindow());
	glFlush();
	return Input->IsTriggered(Keys::Escape) || glfwWindowShouldClose(mainWindow->getGLFWwindow());
}