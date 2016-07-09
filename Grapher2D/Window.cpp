#include "Window.h"

Window::Window(int width, int height, const char* title) {
	m_originalWidth = width;
	m_originalHeight = height;
	m_title = (char*)title;
	m_aspectRatio = (float)width / (float)height;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	m_window = SDL_CreateWindow(m_title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		m_originalWidth, m_originalHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	open = true;

	m_context = SDL_GL_CreateContext(m_window);
}

bool Window::isOpen() {
	return open;
}

void Window::poll() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) {
			open = false;
		} else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				input::callbacks::mouse::button(1, true);
			}
		} else if (e.type == SDL_MOUSEBUTTONUP) {
			if (e.button.button == SDL_BUTTON_LEFT) {
				input::callbacks::mouse::button(1, false);
			}
		} else if (e.type == SDL_MOUSEMOTION) {
			input::callbacks::mouse::pos(e.motion.x, e.motion.y);
		} else if (e.type == SDL_MOUSEWHEEL) {
			input::callbacks::mouse::scroll(e.wheel.x, e.wheel.y);
		} else if (e.type == SDL_WINDOWEVENT) {
			if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
				int width = e.window.data1;
				int height = e.window.data2;
				float aspectRatio = (float)width / (float)height;

				if (aspectRatio != m_aspectRatio) {
					if (aspectRatio > m_aspectRatio) {
						height = (1.f / m_aspectRatio) * width;
					}
					else {
						width = m_aspectRatio * height;
					}
				}

				m_originalWidth = width;
				m_originalHeight = height;
			}
		}
	}

	SDL_SetWindowSize(m_window, m_originalWidth, m_originalHeight);
}

void Window::destroy() {
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
}

void Window::swapBuffers() {
	SDL_GL_SwapWindow(m_window);
}