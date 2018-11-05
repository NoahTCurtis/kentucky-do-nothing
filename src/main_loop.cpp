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
#include "scene_loader.h"


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
				char label[256];
				ImGui::Separator();
				sprintf_s(label, 256, "%s##%i", mesh->name.c_str(), i);
				ImGui::Text(label);
				sprintf_s(label, 256, "%i verts, %i tris", mesh->vertCount, mesh->indexCount / 3);
				ImGui::Text(label);
				sprintf_s(label, 256, "Position##%i", i);
				ImGui::InputFloat3(label, reinterpret_cast<float*>(&mesh->worldPosition), 2);
				sprintf_s(label, 256, "Visible##%i", i);
				ImGui::Checkbox(label, &mesh->visible);
				i++;
			}
		ImGui::End();

		ImGui::Begin("Animations");
			Skeleton& skel = Renderer::get()->skeleTemp;
			ImGui::TextColored(ImVec4(1, 0, 1, 1), "There are %i animations", skel.mAnimations.size());
			ImGui::SliderFloat("AnimTime01", &Renderer::get()->skeleTemp.mAnimTime01, 0, 1);
			ImGui::SliderFloat("QuaternionScale", &Globals.quatExponent, 0, 5);
			ImGui::Checkbox("Animate on curve", &Globals.animateOnCurve);
			i = 0;
			for (auto& anim : skel.mAnimations)
			{
				char label[256];
				///ImGui::Separator();
				sprintf_s(label, 256, "%s", anim->mName.C_Str());
				if (ImGui::Button(label))
					skel.StartAnimation(i);
				i++;
			}
			ImGui::Separator();
			if (ImGui::Button("Reset"))
				Globals.quatDebugVec = glm::vec3(0);
			ImGui::SameLine();
			ImGui::SliderFloat3("QuatDebugger", &Globals.quatDebugVec[0], -1.0f, 1.0f);
		ImGui::End();

		//*
		ImGui::Begin("Curve Editor");
			ImGui::InputInt("Line Segments (ignore)", &Globals.curve.lineSegments);
			if(!ImGui::Checkbox("Show individual curves", &Globals.curve.showOriginalCurves))
			{
				ImGui::Checkbox("Apply Smoothing", &Globals.curve.useSmoothness);
			}
			ImGui::Separator();
			ImGui::Text("Add/Remove Points");
			ImGui::SameLine();
			if(ImGui::Button("+"))
			{
				Globals.curve.push();
			}
			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				Globals.curve.pop();
			}
			for (int i = 0; i < Globals.curve.points.size(); i++)
			{
				char label[256];
				sprintf_s(label, 256, "Point##%i", i);
				ImGui::InputFloat3(label, reinterpret_cast<float*>(&Globals.curve.points[i]), 2);
			}
		ImGui::End();//*/
	ImGui::Render();


	//Reload Shaders
	if (Input->IsTriggered(Keys::F9))
	{
		glUseProgram(0);
		glDeleteProgram(Globals.mesh_shader_program_name);
		Globals.mesh_shader_program_name = create_shader_program("mesh_vert.shader", "mesh_frag.shader");
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
		/*auto rot = glm::angleAxis(clock.dt() * 5.0f, glm::vec3(0, 1, 0));
		rot = rot * mesh->worldRotation;
		mesh->worldRotation.w = rot.w;
		mesh->worldRotation.x = rot.x;
		mesh->worldRotation.y = rot.y;
		mesh->worldRotation.z = rot.z;//*/
		
		if (mesh->visible == false) continue;
		mesh->bind();
		glm::mat4 model2world = mesh->get_model_to_world_matrix();
		glUniformMatrix4fv(glGetUniformLocation(Globals.mesh_shader_program_name, "model2world"), 1, GL_FALSE, &model2world[0][0]);
		glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, 0);
	}

	//create grid debug lines
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

	//position the animation
	float scalef = 0.01f;
	Renderer::get()->skeleTemp.modelToWorld.s = glm::vec3(scalef);
	if (Globals.animateOnCurve)
		Renderer::get()->skeleTemp.modelToWorld.v = Globals.curve(Renderer::get()->skeleTemp.mAnimTime01);
	else
		Renderer::get()->skeleTemp.modelToWorld.v = glm::vec3(0);

	//draw things other than meshes
	Globals.curve.DebugDraw();
	Renderer::get()->skeleTemp.DebugDraw();
	Renderer::get()->render_debug_lines();


	//finish render
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(mainWindow->getGLFWwindow());
	glFlush();
	return Input->IsTriggered(Keys::Escape) || glfwWindowShouldClose(mainWindow->getGLFWwindow());
}