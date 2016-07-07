#pragma once

#include <GLFW/glfw3.h>
#include <thread>
#include <iostream>

class GUI {
private:
	GLFWwindow* m_window;
	std::thread m_thread;
	
public:
	bool running;

	GUI();
	void stop();
	friend static void run(GUI* gui);
};