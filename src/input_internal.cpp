#include <iostream>
#include <cstring>
#include <cstdlib>

#include "glm/glm.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

#include "window.h"
#include "input.h"
#include "input_internal.h"


//Static member function declaration
unsigned short* InputManager::keys_;
float* InputManager::axes_;
double InputManager::mouseX_;
double InputManager::mouseY_;
float InputManager::scrollY_ = 0.0f;
bool InputManager::active_;


inline float TriggerScale(float value)
{
	return (value + 1.0f) / 2.0f;
}


void InputManager::Initialize()
{
	// allocate and erase arrays
	keys_ = new unsigned short[ALL_KEYS_MAX];
	axes_ = new float[GAMEPAD_AXES_MAX];
	std::memset(keys_, 0, ALL_KEYS_MAX * sizeof(keys_[0]));
	std::memset(axes_, 0, GAMEPAD_AXES_MAX * sizeof(axes_[0]));

	//Unpause
	active_ = true;
}

void InputManager::Update(float dt)
{
	//Update the arrays from pressed to down 
	// and from released to up
	for (size_t i = 0; i < ALL_KEYS_MAX; i++)
	{
		if (keys_[i] == KEY_IS_PRESSED)	keys_[i] = KEY_IS_DOWN;
		if (keys_[i] == KEY_IS_RELEASED) keys_[i] = KEY_IS_UP;
	}

	//now and only now, get the new key presses
	glfwPollEvents();

	// GAMEPAD
	int axesCount = 0;
	int buttonsCount = 0;

	if (glfwJoystickPresent(GLFW_JOYSTICK_1))
	{
		// update axes
		const float* returnedAxes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		for (int i = 0; i < glm::min(GAMEPAD_AXES_MAX, axesCount); i++)
		{
			if (i == Pad::LT || i == Pad::LT)
				axes_[i] = TriggerScale(returnedAxes[i]);
			else
				axes_[i] = returnedAxes[i];
		}

		// update buttons
		const unsigned char* returnedButtons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonsCount);
		for (int i = 0; i < buttonsCount; i++)
		{
			if (!(keys_[Pad::BASE + i + 1] & ~KEY_IS_RELEASED) && returnedButtons[i] == GLFW_PRESS)
				keys_[Pad::BASE + i + 1] = KEY_IS_PRESSED;
			else if (keys_[Pad::BASE + i + 1] & ~KEY_IS_RELEASED && returnedButtons[i] == GLFW_RELEASE)
				keys_[Pad::BASE + i + 1] = KEY_IS_RELEASED;
		}
	}
}

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
	delete[] keys_;
}


//////////////////////////////////////////////////////////////////////


void InputManager::RegisterWindow(GLFWwindow* window)
{
	glfwSetCharCallback(window, charFunc_);
	///glfwSetCharModsCallback(window, charModsFunc_);
	///glfwSetCursorEnterCallback(window, cursorEnterFunc_);
	glfwSetCursorPosCallback(window, cursorPosFunc_);
	glfwSetDropCallback(window, dropFunc_);
	glfwSetKeyCallback(window, keyFunc_);
	glfwSetMouseButtonCallback(window, mouseButtonFunc_);
	glfwSetScrollCallback(window, scrollFunc_);
}


//////////////////////////////////////////////////////////////////////


void InputManager::keyFunc_(
	GLFWwindow* window,
	int keycode,
	int scancode,
	int action,
	int mods)
{
	if (keycode > MOUSE_KEYS_NUM && keycode < ALL_KEYS_MAX)
	{
		if (action == GLFW_PRESS)
		{
			keys_[keycode] = KEY_IS_PRESSED;
		}
		else if (action == GLFW_RELEASE)
		{
			keys_[keycode] = KEY_IS_RELEASED;
		}
	}

	ImGui_ImplGlfw_KeyCallback(window, keycode, scancode, action, mods);
}


void InputManager::mouseButtonFunc_(
	GLFWwindow* window,
	int button,
	int action,
	int mods)
{
	if (button >= 0 && button <= MOUSE_KEYS_NUM)
	{
		if (action == GLFW_PRESS)
		{
			keys_[button] = KEY_IS_PRESSED;
		}
		else if (action == GLFW_RELEASE)
		{
			keys_[button] = KEY_IS_RELEASED;
		}
	}

	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void InputManager::scrollFunc_(
	GLFWwindow* window,
	double xoffset,
	double yoffset)
{
	scrollY_ += (float)yoffset;

	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void InputManager::cursorPosFunc_(
	GLFWwindow*,//window,
	double xpos,
	double ypos)
{
	mouseX_ = xpos;
	mouseY_ = ypos;
	//std::cout << "{" << ndc.x << ", " << ndc.y << "}<" << win.x << ", " << win.y << ">\n";
}

void InputManager::dropFunc_(GLFWwindow* window, int file_count, const char** file_names)
{
	for (int i = 0; i < file_count; i++)
	{
		std::cout << file_names[i] << std::endl;
	}
}

void InputManager::charFunc_(GLFWwindow* window, unsigned c)
{
	ImGui_ImplGlfw_CharCallback(window, c);
}


//////////////////////////////////////////////////////////////////////