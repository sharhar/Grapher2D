#pragma once

#include "Input.h"
#include "SDL.h"
#include "SDL_opengl.h"

class Window {
private:
	SDL_Window* m_window;
	SDL_Window* e_window;
	SDL_Renderer* e_renderer;
	SDL_GLContext m_context;
	int m_originalWidth;
	int m_originalHeight;
	char* m_title;
	bool open = false;
	float m_aspectRatio;
public:
	Window(int width, int height, const char* title);
	void setVSync(bool vSync);
	bool isOpen();
	void poll();
	void destroy();
	void swapBuffers();
	int getWidth() { return m_originalWidth; };
	int getHeight() { return m_originalHeight; };
};

