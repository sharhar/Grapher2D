#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <iostream>

namespace input {
	typedef struct MouseData {
		double posX, posY;
		bool button1;
	} MouseData;

	namespace callbacks {
		namespace mouse {
			void pos(GLFWwindow* window, double xpos, double ypos);
			void button(GLFWwindow* window, int button, int action, int mods);
			void scroll(GLFWwindow* window, double xoffset, double yoffset);
		}
	}

	namespace mouse {
		void init();

		MouseData* getMouseData();

		void moved(double x, double y);
		void dragged(double x, double y);
		void scrolled(double s);
	}
}

#endif