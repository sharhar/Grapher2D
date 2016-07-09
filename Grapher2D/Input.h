#pragma once
#ifndef INPUT_H
#define INPUT_H

#include <SDL/SDL.h>
#include <iostream>

namespace input {
	typedef struct MouseData {
		double posX, posY;
		bool button1;
	} MouseData;

	namespace callbacks {
		namespace mouse {
			void pos(double xpos, double ypos);
			void button(int button, bool state);
			void scroll(double xoffset, double yoffset);
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