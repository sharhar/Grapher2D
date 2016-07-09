#pragma once

#include <SDL/SDL.h>
#include <thread>
#include <iostream>

class GUI {
private:
	
	std::thread m_thread;
public:
	bool running;

	GUI();
	void stop();
	friend static void run(GUI* gui);
};