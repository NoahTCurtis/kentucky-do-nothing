/**
* \name Input.cpp
* \author Noah Curtis
* \date
* \brief
*
* ©Copyright 2016 DigiPen Institute of Technology, All Rights Reserved
*/

#include <iostream>
#include <cstring>
#include <cstdlib>

#include <glm/glm.hpp>

#include "window.h"
#include "input.h"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define ALL_KEYS_MAX 375
#define MOUSE_KEYS_NUM 10
#define GAMEPAD_AXES_MAX 10
#define GAMEPAD_BUTTONS_MAX 20

//Static member function declaration
unsigned short* InputManager::keys_;
float* InputManager::axes_;
double InputManager::mouseX_;
double InputManager::mouseY_;
float InputManager::scrollY_ = 0.0f;
bool InputManager::active;



inline float TriggerScale(float value)
{
  return (value + 1.0f) / 2.0f;
}



void InputManager::Initialize()
{
  RegisterWindow(mainWindow);
  // allocate and erase arrays
  keys_ = new unsigned short[ALL_KEYS_MAX];
  axes_ = new float[GAMEPAD_AXES_MAX];
  std::memset(keys_, 0, ALL_KEYS_MAX     * sizeof(keys_[0]));
  std::memset(axes_, 0, GAMEPAD_AXES_MAX * sizeof(axes_[0]));

  //Unpause
  active = true;
}

void InputManager::Update(float dt)
{
  //Update the arrays from pressed to down 
  // and from released to up
  for (size_t i = 0; i < ALL_KEYS_MAX; i++)
  {
    if (keys_[i] == KEY_IS_PRESSED)  keys_[i] = KEY_IS_DOWN;
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

  /*
  static int spaceVal = 0;
  if (keys_[Keys::SPACE] != spaceVal) {
    spaceVal = keys_[Keys::SPACE];
    std::cout << spaceVal << std::flush;
  }
  if (IsTriggered(Keys::SPACE))std::cout << "!" << std::flush;*/

  /*
  for (int i = 0; i < 20; i++)
  {
    std::cout << axes_[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "{" << buttonsCount << "} ";
  for (int i = PAD::BASE + 1; i <= PAD::BASE + buttonsCount; i++)
  {
    if (keys_[i] != 0) std::cout << i << " ";
  }
  //*/
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
  glfwSetKeyCallback(        window, HandleKeyEvent);
  glfwSetMouseButtonCallback(window, HandleMouseEvent);
  glfwSetCursorPosCallback(  window, HandleMousePosition);
  glfwSetScrollCallback(     window, HandleScrollEvent);
}


//////////////////////////////////////////////////////////////////////


void InputManager::HandleKeyEvent(
  GLFWwindow*, //window,
  int keycode,
  int , //scancode,
  int action,
  int ) //mode)
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

  ///ImGuiIO& io = ImGui::GetIO();
  ///if (action == GLFW_PRESS)
  ///  io.KeysDown[keycode] = true;
  ///if (action == GLFW_RELEASE)
  ///  io.KeysDown[keycode] = false;
  ///
  ///io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
  ///io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
  ///io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
  ///io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}


void InputManager::HandleMouseEvent(
                  GLFWwindow* ,//window,
                  int button,
                  int action,
                  int )//mods)
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
}

void InputManager::HandleScrollEvent(
                  GLFWwindow* ,//window,
                  double ,//xoffset,
                  double yoffset)
{
  scrollY_ += (float)yoffset;
}

void InputManager::HandleMousePosition(
                  GLFWwindow* ,//window,
                  double xpos,
                  double ypos)
{
  mouseX_ = xpos;
  mouseY_ = ypos;
  //std::cout << "{" << ndc.x << ", " << ndc.y << "}<" << win.x << ", " << win.y << ">\n";
}


//////////////////////////////////////////////////////////////////////


glm::vec2 InputManager::MousePosNDC(void)
{
  //TODO: fix
  ///auto& winHan = mainWindow;
  ///return glm::vec2(static_cast<float>(mouseX_) / (static_cast<float>(winHan->windowWidth_)  / 2.0) - 1.0,
  ///                 static_cast<float>(mouseY_) / (static_cast<float>(winHan->windowHeight_) / 2.0) - 1.0);
}

glm::vec2 InputManager::MousePosWindow(void)
{
  return glm::vec2(mouseX_, mouseY_);
}

/*glm::vec2 InputManager::MousePos_Special(void)
{
  glm::vec2 ndc = MousePos_NDC();
 
  glm::vec4 ret = ColoredCameras::red_camera->Calculate_NDCtoWorld() * glm::vec4(ndc.x, -ndc.y, 1.0, 1.0);
  
  //std::cout << "{R{" << ret.x << "," << ret.y << "}}\n";
  return glm::vec2(ret.x, ret.y);
}*/


//////////////////////////////////////////////////////////////////////


int InputManager::IsTriggered(int keycode)
{
  if (!active) return 0;
  if(keycode >= 0 && keycode < ALL_KEYS_MAX)
  {
    return keys_[keycode] & KEY_IS_PRESSED;
  }
  return KEY_IS_UNKNOWN;
}


int InputManager::IsDown(int keycode)
{
  if (!active) return 0;
  if (keycode >= 0 && keycode < ALL_KEYS_MAX)
  {
    return keys_[keycode] & ~KEY_IS_RELEASED;
  }
  return KEY_IS_UNKNOWN;
}


int InputManager::IsReleased(int keycode)
{
  if (!active) return 0;
  if (keycode >= 0 && keycode < ALL_KEYS_MAX)
  {
    return keys_[keycode] & KEY_IS_RELEASED;
  }
  return KEY_IS_UNKNOWN;
}

bool InputManager::IsScrolling()
{
  bool toReturn = !!scrollY_;
  scrollY_ += (scrollY_ < 0) ? 0.1f : -0.1f;
  if (scrollY_ > -0.1f && scrollY_ < 0.1f)
  {
    scrollY_ = 0.0f;
  }
  return toReturn;
}

float InputManager::GetScroll()
{
  float toReturn = scrollY_;
  return toReturn;
}

int InputManager::IsUp(int keycode)
{
  if (!active) return 0;
  if (keycode >= 0 && keycode < ALL_KEYS_MAX)
  {
    return !(keys_[keycode] & ~KEY_IS_RELEASED);
  }
  return KEY_IS_UNKNOWN;
}


///////////////////////////////////////////////////////////////////////


float InputManager::Axis(int axis)
{
  if (!active) return 0;
  return axes_[axis];
}


glm::vec2 InputManager::RightStick()
{
  if (!active) return glm::vec2(0, 0);
  return glm::vec2(axes_[Pad::RX], axes_[Pad::RY]);
}


glm::vec2 InputManager::LeftStick()
{
  if (!active) return glm::vec2(0, 0);
  return glm::vec2(axes_[Pad::LX], axes_[Pad::LY]);
}


float InputManager::RightTrigger()
{
  if (!active) return 0.0f;
  return axes_[Pad::RT];
}


float InputManager::LeftTrigger()
{
  if (!active) return 0.0f;
  return axes_[Pad::LT];
}


glm::vec2 InputManager::DPad()
{
  if (!active) return glm::vec2(0, 0);
  int up    = true && keys_[Pad::Up];
  int down  = true && keys_[Pad::Down];
  int right = true && keys_[Pad::Right];
  int left  = true && keys_[Pad::Left];
  return glm::vec2(up - down, right - left);
}