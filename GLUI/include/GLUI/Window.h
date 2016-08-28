#pragma once

struct GLFWwindow;
struct GLFWimage;

namespace glui{

	class Window {
	private:
		GLFWwindow* m_window;
		int m_width;
		int m_height;
	public:
		Window(const char* title, int width, int height, bool resizeable, int iconNum, GLFWimage* icon);
		void poll();
		int getWidth() { return m_width; }
		int getHeight() { return m_height; }
		bool isOpen();
		void swap();
		void destroy();

		void* getGLFWwindow();
	};
}