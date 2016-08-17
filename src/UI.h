#pragma once

#include <GLUI/GLUI.h>
#include <GLUI/Button.h>
#include <GLUI/TextBox.h>
#include <GLUI/Window.h>
#include <GLUI/Renderer.h>
#include <thread>
#include <iostream>

class UI {
private:
	static std::thread* thread;
public:
	static glui::TextBox** texts;
	static glui::Button** buttons;
	static void addGraph(std::string eq, int index);
	static bool open;
	static void start();
	static void stop();
};