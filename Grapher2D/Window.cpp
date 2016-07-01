#include "Window.h"

Window::Window(int width, int height, const char* title) {
	m_window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMakeContextCurrent(m_window);

	glfwSwapInterval(1);
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