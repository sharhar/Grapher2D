#pragma once

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
};

