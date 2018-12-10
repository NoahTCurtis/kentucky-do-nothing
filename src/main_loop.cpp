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
				for (auto& mesh : Renderer::get()->Meshes)
					mesh->worldTransform.v = glm::vec3(randRange(-5, 5), randRange(-5, 5), randRange(-5, 5));
			}
		ImGui::End();

		ImGui::Begin("Meshes");
			ImGui::TextColored(ImVec4(1,0,1,1), "There are %i meshes", Renderer::get()->Meshes.size());
			int i = 1;
			for(auto& mesh : Renderer::get()->Meshes)
			{
				char label[256];
				ImGui::Separator();
				sprintf_s(label, 256, "%s##%i", mesh->name.c_str(), i);
				ImGui::Text(label);
				sprintf_s(label, 256, "%i verts, %i tris", mesh->vertCount, mesh->indexCount / 3);
				ImGui::Text(label);
				sprintf_s(label, 256, "Position##%i", i);
				ImGui::InputFloat3(label, reinterpret_cast<float*>(&mesh->worldTransform.v), 2);
				sprintf_s(label, 256, "Visible##%i", i);
				ImGui::Checkbox(label, &mesh->visible);
				i++;
			}
		ImGui::End();

		ImGui::Begin("Animations");
			Skeleton& skel = Renderer::get()->skeleTemp;
			ImGui::TextColored(ImVec4(1, 0, 1, 1), "There are %i animations", skel.mAnimations.size());
			ImGui::SliderFloat("Animation Speed", &Globals.animationSpeed, 0, 50);
			ImGui::SliderFloat("Move/Animate Speed Ratio", &Globals.moveAnimateSpeedRatio, 1, 300);
			ImGui::Text("Speed = %f", Globals.computedMoveSpeed);
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
		ImGui::End();

		ImGui::Begin("Curve Editor");
			ImGui::InputInt("Line Segments (ignore)", &Globals.curve.lineSegments);
			if(!ImGui::Checkbox("Show individual curves", &Globals.curve.showOriginalCurves))
			{
				ImGui::Checkbox("Apply Smoothing", &Globals.curve.useSmoothness);
			}
			if (ImGui::Button("Recompute Arc Length Table"))
				Globals.curve.recompute_arc_length_table();
			ImGui::Separator();
			ImGui::Text("Add/Remove Points");
			ImGui::SameLine();
			if (ImGui::Button("-"))
			{
				Globals.curve.pop();
			}
			ImGui::SameLine();
			if(ImGui::Button("+"))
			{
				Globals.curve.push();
			}
			for (int i = 0; i < Globals.curve.points.size(); i++)
			{
				char label[256];
				sprintf_s(label, 256, "Point##%i", i);
				ImGui::InputFloat3(label, reinterpret_cast<float*>(&Globals.curve.points[i]), 2);
			}
		ImGui::End();

		ImGui::Begin("IK");
			IK& ik = *(Renderer::get()->skeleTemp.ik);
			ImGui::InputText("Bone Name", ik.endEffectorName, IK_BUFFER_SIZE);
			ImGui::InputFloat3("Target Pos", &ik.targetWorldPoint.x, 3);
			ImGui::InputInt("Depth", &ik.depth);
			ImGui::SliderFloat("Fader", &ik.fader01, 0, 1);
			ImGui::InputText("Breaker", ik.boneBreakName, IK_BUFFER_SIZE);
			///if (!ik.Compute())
			///	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Bone Not Found");
		ImGui::End();
	ImGui::Render();

	//(TEMP) move the cubey boi to the IK target pos
	glm::vec3 moveTarget;
	moveTarget.x = float(Input->IsDown(Keys::Right) - Input->IsDown(Keys::Left));
	moveTarget.y = float(Input->IsDown(Keys::PageUp) - Input->IsDown(Keys::PageDown));
	moveTarget.z = float(Input->IsDown(Keys::Down) - Input->IsDown(Keys::Up));
	Renderer::get()->skeleTemp.ik->targetWorldPoint += moveTarget * clock.dt() * 1.0f;
	Renderer::get()->Meshes.begin().operator*()->worldTransform.v = Renderer::get()->skeleTemp.ik->targetWorldPoint;
	Renderer::get()->Meshes.begin().operator*()->worldTransform.s = glm::vec3(0.1f);
	Renderer::get()->Meshes.begin().operator*()->visible = true;


	//update systems
	Input->Update(0);
	clock.recompute_delta_time();
	Camera::get()->Update();

	//rename window
	std::stringstream ss;
	ss << "CS460 Advanced Demo (" << (int)clock.fps() << ") " << clock.dt();
	mainWindow->change_title(ss.str());

	//create grid debug lines
	DebugLine dbl;
	int gridSize = 20;
	glm::vec3 camPos(glm::floor(Camera::get()->position.x), 0, glm::floor(Camera::get()->position.z));
	for (int i = -gridSize; i <= gridSize; i++)
	{
		dbl.start = glm::vec3(i, 0, -gridSize) + camPos;
		dbl.end = glm::vec3(i, 0, gridSize) + camPos;
		dbl.startcolor = dbl.endcolor = glm::vec3(1, 0.3f, 0.7f);
		Renderer::get()->add_debug_line(dbl);
		dbl.start = glm::vec3(-gridSize, 0, i) + camPos;
		dbl.end = glm::vec3(gridSize, 0, i) + camPos;
		dbl.startcolor = dbl.endcolor = glm::vec3(0.7f, 0.3f, 1);
		Renderer::get()->add_debug_line(dbl);
	}
	
	//draw things other than meshes
	Globals.curve.debug_draw();
	Renderer::get()->skeleTemp.Update(clock.dt());

	//finish render
	Renderer::get()->Update();

	return Input->IsTriggered(Keys::Escape) || glfwWindowShouldClose(mainWindow->getGLFWwindow());
}