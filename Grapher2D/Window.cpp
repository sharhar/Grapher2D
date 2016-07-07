#include "Window.h"

Window::Window(int width, int height, const char* title) {
	m_window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMakeContextCurrent(m_window);

	glfwSetWindowAspectRatio(m_window, width, height);

	glfwSwapInterval(1);

	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glfwSetWindowPos(m_window,
		(vidMode->width - width)/2,
		(vidMode->height - height) / 2);

	glfwSetCursorPosCallback(m_window, input::callbacks::mouse::pos);
	glfwSetMouseButtonCallback(m_window, input::callbacks::mouse::button);
	glfwSetScrollCallback(m_window, input::callbacks::mouse::scroll);
}

bool Window::isOpen() {
	return !glfwWindowShouldClose(m_window);
}

void Window::poll() {
	glfwPollEvents();
}

void Window::destroy() {
	glfwDestroyWindow(m_window);
}

void Window::swapBuffers() {
	glfwSwapBuffers(m_window);
}

void Window::getSize(int* width, int* height) {
	glfwGetWindowSize(m_window, width, height);
}