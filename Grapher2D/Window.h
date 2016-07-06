#pragma once

#include "Input.h"
#include <GLFW/glfw3.h>

class Window {
private:
	GLFWwindow* m_window;
public:
	Window(int width, int height, const char* title);
	bool isOpen();
	void poll();
	void destroy();
	void swapBuffers();
	void getSize(int* width, int* height);
};

