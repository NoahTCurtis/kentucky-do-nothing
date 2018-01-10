#include <input.h>

bool someKeyPressed = false;

void input_key_callback(GLFWwindow*, 
                        int key,
                        int scancode,
                        int action,
                        int mods)
{
  someKeyPressed = true;
}