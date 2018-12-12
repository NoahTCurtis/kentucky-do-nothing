#include "cloth.h"
#include "render.h"
#include "util.h"
#include "input.h"


Cloth::Cloth(int w_, int h_)
	: width(w_), height(h_)
{
	position.reserve(width);
	velocity.reserve(width);
	acceler8.reserve(width);
	constrain.reserve(width);
	movable.reserve(width);
	for (int w = 0; w < width; w++)
	{
		position.push_back(std::vector<glm::vec3>());
		position[w].reserve(height);
		velocity.push_back(std::vector<glm::vec3>());
		velocity[w].reserve(height);
		acceler8.push_back(std::vector<glm::vec3>());
		acceler8[w].reserve(height);
		constrain.push_back(std::vector<glm::vec3>());
		constrain[w].reserve(height);
		movable.push_back(std::vector<bool>());
		movable[w].reserve(height);
		for (int h = 0; h < height; h++)
		{
			float wf = 1.0f - ((float)w / (float)(width-1));
			float hf = 1.0f - (float)h / (float)(height-1);
			position[w].push_back(glm::vec3(wf, hf, 0));
			velocity[w].push_back(glm::vec3(0));
			acceler8[w].push_back(glm::vec3(0));
			constrain[w].push_back(glm::vec3(0));
			movable[w].push_back(h != 0);
		}
	}
	Reset();

	restVertical = 1.0f / float(height - 1);
	restHorizontal = 1.0f / float(width - 1);
	restDiagonal = glm::sqrt(restHorizontal * restHorizontal + restVertical * restVertical);
}

void Cloth::Reset()
{
	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			float wf = 1.0f - ((float)w / (float)(width - 1));
			float hf = 1.0f - (float)h / (float)(height - 1);
			position[w][h] = glm::vec3(wf, hf, 0);
			velocity[w][h] = glm::vec3(0);
			acceler8[w][h] = glm::vec3(0);
			constrain[w][h] = glm::vec3(0);
			if(Input->IsDown(Keys::F))
				movable[w][h] = (true);
			else if (Input->IsDown(Keys::Two)) {
				glm::vec2 pos(wf, hf);
				glm::vec2 c1(0, 1);
				glm::vec2 c2(1, 1);
				float dist = glm::min(glm::distance(pos, c1), glm::distance(pos, c2));
				movable[w][h] = (dist > 0.2f);
			}
			else
				movable[w][h] = (h != 0);
		}
	}
}

void Cloth::DebugDraw()
{
	DebugLine dl;

	for (int w = 0; w < width; w++)
	{
		float wf = 1.0f - ((float)w / (float)width);
		dl.end = position[w][0];
		dl.endcolor = glm::vec3(wf, 1, 1);
		for (int h = 0; h < height - 1; h++)
		{
			float hf = 1.0f - ((float)h / (float)height);

			dl.start = dl.end;
			dl.end = position[w][h + 1];
			dl.startcolor = dl.endcolor;
			dl.endcolor = glm::vec3(wf, 1, hf);

			Renderer::get()->add_debug_line(dl);
		}
	}

	for (int h = 0; h < height; h++)
	{
		float hf = 1.0f - ((float)h / (float)height);
		dl.end = position[0][h];
		dl.endcolor = glm::vec3(1, 1, hf);
		for (int w = 0; w < width - 1; w++)
		{
			float wf = 1.0f - ((float)w / (float)width);
	
			dl.start = dl.end;
			dl.startcolor = dl.endcolor;
			dl.end = position[w+1][h];
			dl.endcolor = glm::vec3(wf, 1, hf);
	
			Renderer::get()->add_debug_line(dl);
		}
	}
}

void Cloth::Update(float dt)
{
	//wiggle
	if(Input->IsDown(Keys::T))
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
			if(movable[w][h])
				acceler8[w][h] += glm::vec3(randRange(-1, 1), randRange(-1,1), randRange(-1, 1));

	//push
	if (Input->IsDown(Keys::G))
		for (int w = 0; w < width; w++)
			for (int h = 0; h < height; h++)
				if (movable[w][h])
					acceler8[w][h] += glm::vec3(randRange(-1, 1), randRange(-1, 1), -7.0f);

	//wind
	if (Input->IsDown(Keys::V))
		Wind(windDirection);

	//reset
	if (Input->IsTriggered(Keys::R))
		Reset();

	//compute with substep
	float substepLength = 1.0f / (float)substeps;

	//update
	for (int step = 0; step < substeps; step++)
	{
		Constrain_Structural();
		Constrain_Shear();
		Constrain_Bend();
		Apply_Constraints(constraintStrength);
		Collide();
		Integrate(dt * substepLength);
	}
}

void Cloth::Integrate(float dt)
{
	float damp = 1.0f - velocityDamping;
	float dt2 = dt;
	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height; h++)
		{
			//apply
			if (movable[w][h])
			{
				velocity[w][h] += (acceler8[w][h] + gravity) * dt2;
				position[w][h] += velocity[w][h] * dt;
			}

			//dampen
			acceler8[w][h] = glm::vec3(0);
			velocity[w][h] *= damp;
		}
	}
}

void Cloth::Constrain(int w1, int h1, int w2, int h2, float rest, int type)
{
	if (true)
	{
		glm::vec3 vec12 = position[w2][h2] - position[w1][h1];
		float dist = glm::length(vec12);
		glm::vec3 fixer = vec12 * (1.0f - rest / dist) * 0.5f;
		constrain[w1][h1] += fixer;
		constrain[w2][h2] -= fixer;
	}

	if(true)
	{
		glm::vec3 deltaPos = position[w1][h1] - position[w2][h2];
		float dist = glm::length(deltaPos);

		float Hterm = (dist - rest) * springStiff[type];

		glm::vec3 deltaVel = velocity[w1][h1] - velocity[w2][h2];
		float Dterm = (glm::dot(deltaPos, deltaVel) * springDamp[type]) / dist;

		glm::vec3 springForce = deltaPos / dist;
		springForce *= -(Hterm + Dterm);
		
		acceler8[w1][h1] += springForce;
		acceler8[w2][h2] -= springForce;
	}
}

void Cloth::Constrain_Structural()
{
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
		{
			if (h < height - 1)
				Constrain(w, h, w, h + 1, restVertical, 0);
			if (w < width - 1)
				Constrain(w, h, w + 1, h, restHorizontal, 0);
		}

}

void Cloth::Constrain_Shear()
{
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
		{
			if (h < height - 1 && w < width - 1)
				Constrain(w, h, w + 1, h + 1, restDiagonal, 1);
			if (h > 0 && w < width - 1)
				Constrain(w, h, w + 1, h - 1, restDiagonal, 1);
		}
}

void Cloth::Constrain_Bend()
{
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
		{
			if (h < height - 2)
				Constrain(w, h, w, h + 2, restVertical * 2.0f, 2);
			if (w < width - 2)
				Constrain(w, h, w + 2, h, restHorizontal * 2.0f, 2);

			if (h < height - 2 && w < width - 2)
				Constrain(w, h, w + 2, h + 2, restDiagonal * 2.0f, 2);
			if (h > 1 && w < width - 2)
				Constrain(w, h, w + 2, h - 2, restDiagonal * 2.0f, 2);
		}
}

void Cloth::Apply_Constraints(float strength)
{
	for (int w = 0; w < width; w++)
	{
		for (int h = 0; h < height - 1; h++)
		{
			if(movable[w][h])
				position[w][h] += constrain[w][h] * strength;
			constrain[w][h] = glm::vec3(0);
		}
	}
}

void Cloth::Wind(glm::vec3 direction)
{
	auto W = [&](int w) { return glm::max(glm::min(w, width - 1), 0); };
	auto H = [&](int h) { return glm::max(glm::min(h, height - 1), 0); };

	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
			if (movable[w][h])
			{
				glm::vec3 wdiff = position[W(w+1)][H(h)] - position[W(w-1)][H(h)];
				glm::vec3 hdiff = position[W(w)][H(h+1)] - position[W(w)][H(h-1)];
				glm::vec3 normal = glm::normalize(glm::cross(wdiff, hdiff));
				acceler8[w][h] += normal * glm::dot(normal, direction);
			}
}

void Cloth::Collide()
{
	for (int w = 0; w < width; w++)
		for (int h = 0; h < height; h++)
			if (movable[w][h])
			{
				glm::vec3 vec = position[w][h] - glm::vec3(sphere);
				if (vec.x == 0 && vec.y == 0 && vec.z == 0) continue;
				float dist = glm::length(vec);
				vec /= dist;
				float depth = dist - sphere.w;
				position[w][h] -= vec * glm::min(0.0f, depth);
			}
}
