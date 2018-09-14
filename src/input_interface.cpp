#include <iostream>
#include <cstring>
#include <cstdlib>

#include <glm/glm.hpp>

#include "input.h"
#include "input_internal.h"


glm::vec2 InputManager::MousePosNDC(void)
{
	//TODO: fix
	///auto& winHan = mainWindow;
	///return glm::vec2(static_cast<float>(mouseX_) / (static_cast<float>(winHan->windowWidth_)	/ 2.0) - 1.0,
	///								 static_cast<float>(mouseY_) / (static_cast<float>(winHan->windowHeight_) / 2.0) - 1.0);
	return glm::vec2();
}


glm::vec2 InputManager::MousePosWindow(void)
{
	return glm::vec2(mouseX_, mouseY_);
}


///glm::vec2 InputManager::MousePos_Special(void)
///{
///	glm::vec2 ndc = MousePos_NDC();
/// 
///	glm::vec4 ret = ColoredCameras::red_camera->Calculate_NDCtoWorld() * glm::vec4(ndc.x, -ndc.y, 1.0, 1.0);
///	
///	//std::cout << "{R{" << ret.x << "," << ret.y << "}}\n";
///	return glm::vec2(ret.x, ret.y);
///}


//////////////////////////////////////////////////////////////////////


int InputManager::IsTriggered(int keycode)
{
	if (!active_) return 0;
	if(keycode >= 0 && keycode < ALL_KEYS_MAX)
	{
		return keys_[keycode] & KEY_IS_PRESSED;
	}
	return KEY_IS_UNKNOWN;
}


int InputManager::IsDown(int keycode)
{
	if (!active_) return 0;
	if (keycode >= 0 && keycode < ALL_KEYS_MAX)
	{
		return keys_[keycode] & ~KEY_IS_RELEASED;
	}
	return KEY_IS_UNKNOWN;
}


int InputManager::IsReleased(int keycode)
{
	if (!active_) return 0;
	if (keycode >= 0 && keycode < ALL_KEYS_MAX)
	{
		return keys_[keycode] & KEY_IS_RELEASED;
	}
	return KEY_IS_UNKNOWN;
}

int InputManager::IsUp(int keycode)
{
	if (!active_) return 0;
	if (keycode >= 0 && keycode < ALL_KEYS_MAX)
	{
		return !(keys_[keycode] & ~KEY_IS_RELEASED);
	}
	return KEY_IS_UNKNOWN;
}


///////////////////////////////////////////////////////////////////////


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


///////////////////////////////////////////////////////////////////////


float InputManager::Axis(int axis)
{
	if (!active_) return 0;
	return axes_[axis];
}


glm::vec2 InputManager::RightStick()
{
	if (!active_) return glm::vec2(0, 0);
	return glm::vec2(axes_[Pad::RX], axes_[Pad::RY]);
}


glm::vec2 InputManager::LeftStick()
{
	if (!active_) return glm::vec2(0, 0);
	return glm::vec2(axes_[Pad::LX], axes_[Pad::LY]);
}


float InputManager::RightTrigger()
{
	if (!active_) return 0.0f;
	return axes_[Pad::RT];
}


float InputManager::LeftTrigger()
{
	if (!active_) return 0.0f;
	return axes_[Pad::LT];
}


glm::vec2 InputManager::DPad()
{
	if (!active_) return glm::vec2(0, 0);
	int up		= true && keys_[Pad::Up];
	int down	= true && keys_[Pad::Down];
	int right = true && keys_[Pad::Right];
	int left	= true && keys_[Pad::Left];
	return glm::vec2(up - down, right - left);
}