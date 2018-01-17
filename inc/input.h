/******************************************************
*******************HOW TO USE INPUT********************
*******************************************************
* 1> Inlcude this header                              *
*                                                     *
* 2> Do the following:                                *
*                                                     *
*   if( Input::IsTriggered( KEYS::SPACE ) )           *
*   {                                                 *
*     //Your code goes here!                          *
*   }                                                 *
*                                                     *
* 3> Other functions are available:                   *
*      int Input::IsTriggered (int keycode);          *
*      int Input::IsDown      (int keycode);          *
*      int Input::IsReleased  (int keycode);          *
*      int Input::IsUp        (int keycode);          *
*                                                     *
* 3.5> All of these functions act as booleans, so     *
*      use them accordingly. It's a lot like Zilch.   *
*                                                     *
* 4> The list of keys below is the list of usable     *
*    keys. Please use the enum-ed ones, not the       *
*    GLFW_KEY_X ones.                                 *
******************************************************/
#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>


/* Key position values */
#define KEY_IS_UNKNOWN  0
#define KEY_IS_UP       0
#define KEY_IS_PRESSED  1
#define KEY_IS_DOWN     2
#define KEY_IS_RELEASED 4

namespace Mouse
{
  enum Mouse
  {
    /* Mouse Keys */
      Left = 0
    , Right = 1
    , Middle = 2
  }; // end of enum
} // end of namespace


namespace Keys
{
  enum Keys
  {
    /* Printable keys */
      Space             = GLFW_KEY_SPACE
    , Apostrophe        = GLFW_KEY_APOSTROPHE
    , Comma             = GLFW_KEY_COMMA
    , Minus             = GLFW_KEY_MINUS
    , Period            = GLFW_KEY_PERIOD
    , Slash             = GLFW_KEY_SLASH
    , Zero              = GLFW_KEY_0
    , One               = GLFW_KEY_1
    , Two               = GLFW_KEY_2
    , Three             = GLFW_KEY_3
    , Four              = GLFW_KEY_4
    , Five              = GLFW_KEY_5
    , Six               = GLFW_KEY_6
    , Seven             = GLFW_KEY_7
    , Eight             = GLFW_KEY_8
    , Nine              = GLFW_KEY_9
    , Semicolon         = GLFW_KEY_SEMICOLON
    , Equal             = GLFW_KEY_EQUAL
    , A                 = GLFW_KEY_A
    , B                 = GLFW_KEY_B
    , C                 = GLFW_KEY_C
    , D                 = GLFW_KEY_D
    , E                 = GLFW_KEY_E
    , F                 = GLFW_KEY_F
    , G                 = GLFW_KEY_G
    , H                 = GLFW_KEY_H
    , I                 = GLFW_KEY_I
    , J                 = GLFW_KEY_J
    , K                 = GLFW_KEY_K
    , L                 = GLFW_KEY_L
    , M                 = GLFW_KEY_M
    , N                 = GLFW_KEY_N
    , O                 = GLFW_KEY_O
    , P                 = GLFW_KEY_P
    , Q                 = GLFW_KEY_Q
    , R                 = GLFW_KEY_R
    , S                 = GLFW_KEY_S
    , T                 = GLFW_KEY_T
    , U                 = GLFW_KEY_U
    , V                 = GLFW_KEY_V
    , W                 = GLFW_KEY_W
    , X                 = GLFW_KEY_X
    , Y                 = GLFW_KEY_Y
    , Z                 = GLFW_KEY_Z
    , LeftBracket       = GLFW_KEY_LEFT_BRACKET
    , Backslash         = GLFW_KEY_BACKSLASH
    , RightBracket      = GLFW_KEY_RIGHT_BRACKET
    , GraveAccent       = GLFW_KEY_GRAVE_ACCENT

    /* special keys */
    , Escape            = GLFW_KEY_ESCAPE
    , Enter             = GLFW_KEY_ENTER
    , Tab               = GLFW_KEY_TAB
    , Backspace         = GLFW_KEY_BACKSPACE
    , Insert            = GLFW_KEY_INSERT
    , Delete            = GLFW_KEY_DELETE
    , Right             = GLFW_KEY_RIGHT
    , Left              = GLFW_KEY_LEFT
    , Down              = GLFW_KEY_DOWN
    , Up                = GLFW_KEY_UP
    , PageUp            = GLFW_KEY_PAGE_UP
    , PageDown          = GLFW_KEY_PAGE_DOWN
    , Home              = GLFW_KEY_HOME
    , End               = GLFW_KEY_END
    , CapsLock          = GLFW_KEY_CAPS_LOCK
    , ScrollLock        = GLFW_KEY_SCROLL_LOCK
    , NumLock           = GLFW_KEY_NUM_LOCK
    , PrintScreen       = GLFW_KEY_PRINT_SCREEN
    , Pause             = GLFW_KEY_PAUSE
    , F1                = GLFW_KEY_F1
    , F2                = GLFW_KEY_F2
    , F3                = GLFW_KEY_F3
    , F4                = GLFW_KEY_F4
    , F5                = GLFW_KEY_F5
    , F6                = GLFW_KEY_F6
    , F7                = GLFW_KEY_F7
    , F8                = GLFW_KEY_F8
    , F9                = GLFW_KEY_F9
    , F10               = GLFW_KEY_F10
    , F11               = GLFW_KEY_F11
    , F12               = GLFW_KEY_F12
    , F13               = GLFW_KEY_F13
    , F14               = GLFW_KEY_F14
    , F15               = GLFW_KEY_F15
    , F16               = GLFW_KEY_F16
    , F17               = GLFW_KEY_F17
    , F18               = GLFW_KEY_F18
    , F19               = GLFW_KEY_F19
    , F20               = GLFW_KEY_F20
    , F21               = GLFW_KEY_F21
    , F22               = GLFW_KEY_F22
    , F23               = GLFW_KEY_F23
    , F24               = GLFW_KEY_F24
    , F25               = GLFW_KEY_F25
    , Keypad0           = GLFW_KEY_KP_0
    , Keypad1           = GLFW_KEY_KP_1
    , Keypad2           = GLFW_KEY_KP_2
    , Keypad3           = GLFW_KEY_KP_3
    , Keypad4           = GLFW_KEY_KP_4
    , Keypad5           = GLFW_KEY_KP_5
    , Keypad6           = GLFW_KEY_KP_6
    , Keypad7           = GLFW_KEY_KP_7
    , Keypad8           = GLFW_KEY_KP_8
    , Keypad9           = GLFW_KEY_KP_9
    , KeypadDecimal     = GLFW_KEY_KP_DECIMAL
    , KeypadDivide      = GLFW_KEY_KP_DIVIDE
    , KeypadMultiply    = GLFW_KEY_KP_MULTIPLY
    , KeypadSubtract    = GLFW_KEY_KP_SUBTRACT
    , KeypadAdd         = GLFW_KEY_KP_ADD
    , KeypadEnter       = GLFW_KEY_KP_ENTER
    , KeypadEqual       = GLFW_KEY_KP_EQUAL
    , LeftShift         = GLFW_KEY_LEFT_SHIFT
    , LeftControl       = GLFW_KEY_LEFT_CONTROL
    , LeftAlt           = GLFW_KEY_LEFT_ALT
    , LeftSuper         = GLFW_KEY_LEFT_SUPER
    , RightShift        = GLFW_KEY_RIGHT_SHIFT
    , RightControl      = GLFW_KEY_RIGHT_CONTROL
    , RightAlt          = GLFW_KEY_RIGHT_ALT
    , RightSuper        = GLFW_KEY_RIGHT_SUPER
    , Menu              = GLFW_KEY_MENU
  }; // end of enum
} // end of namespace


namespace Gamepad
{
  enum Gamepad
  {
    /* gamepad keys */
      BASE = 350
    , A
    , B
    , X
    , Y
    , LeftBumper
    , RightBumper
    , Select
    , Start
    , LeftStick
    , RightStick
    , Up
    , Right
    , Down
    , Left

    /* gamepad axes */
    , LeftXAxis    = 0
    , LeftYAxis    = 1
    , RightXAxis   = 2
    , RightYAxis   = 3
    , LeftTrigger  = 4
    , RightTrigger = 5
    , LX = 0
    , LY = 1
    , RX = 2
    , RY = 3
    , LT = 4
    , RT = 5

  }; // end of enum
} // end of namespace
namespace Pad = Gamepad;
namespace Controller = Gamepad;


//////////////////////////////////////////////////////////////


class InputManager
{
public:
  void Initialize();

  void Update(float dt);

  InputManager();
  ~InputManager();

  static void RegisterWindow(GLFWwindow* window);

  static void HandleKeyEvent(
    GLFWwindow *window,
    int keycode,
    int scancode,
    int action,
    int mode);

  static void HandleMouseEvent(
    GLFWwindow* window,
    int button,
    int action,
    int mods);

  static void HandleScrollEvent(
    GLFWwindow* window,
    double xoffset,
    double yoffset);

  static void HandleMousePosition(
    GLFWwindow* window,
    double xpos,
    double ypos);

  static glm::vec2 MousePosNDC(void);
  static glm::vec2 MousePosWindow(void);
  //static glm::vec2 MousePosSpecial(void);

  static int IsTriggered(int keycode);
  static int IsDown     (int keycode);
  static int IsReleased (int keycode);
  static int IsUp       (int keycode);
  
  static float Axis(int axis);
  static glm::vec2 RightStick();
  static glm::vec2 LeftStick();
  static float RightTrigger();
  static float LeftTrigger();
  static glm::vec2 DPad();
  static bool IsScrolling();
  static float GetScroll();

  static bool active;

private:

  static unsigned short* keys_;
  static float* axes_;
  static double mouseX_;
  static double mouseY_;
  static float  scrollY_;
    
}; // end of class InputManager

typedef InputManager Input;

#endif // end of once-over