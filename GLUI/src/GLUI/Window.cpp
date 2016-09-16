#include <GLUI/GLUI.h>
#include <GLUIExt.h>
#include <iostream>

namespace glui {
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		input::callbacks::key(key, action);
	}

	static void mousePosCallback(GLFWwindow* window, double xpos, double ypos) {
		input::callbacks::mousePosition((int) xpos, (int) ypos);
	}

	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		input::callbacks::mouseButton(button, action);
	}

	static void textCallBack(GLFWwindow* window, unsigned int codepoint) {
		input::callbacks::text(codepoint);
	}

	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		input::callbacks::mouseScroll(yoffset);
	}

	Window::Window(const char* title, int width, int height, bool resizeable, int iconNum, GLFWimage* icon) {
		m_width = width;
		m_height = height;

		if (resizeable) {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		} else {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}

		GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
		glfwMakeContextCurrent(window);

		glfwSwapInterval(1);

		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		glfwSetCursorPosCallback(window, mousePosCallback);
		glfwSetCharCallback(window, textCallBack);
		glfwSetScrollCallback(window, mouseScrollCallback);
		m_window = window;
		
		if (icon != NULL) {
			glfwSetWindowIcon(window, iconNum, icon);
		}
	}

	void Window::poll() {
		glfwPollEvents();

		GLenum err = GL_NO_ERROR;
		while ((err = glGetError()) != GL_NO_ERROR) {
			std::cout << "GLError: " << err << "\n";
		}
	}

	void Window::swap() {
		glfwSwapBuffers(m_window);
	}

	bool Window::isOpen() {
		return !glfwWindowShouldClose(m_window);
	}

	void Window::destroy() {
		glfwDestroyWindow(m_window);
	}

	void* Window::getGLFWwindow() {
		return m_window;
	}
}