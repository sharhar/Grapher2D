#include "Input.h"

input::MouseData* m_mouse;

void input::callbacks::mouse::pos(double xpos, double ypos) {
	double difX, difY;
	
	if (m_mouse->posX == -1) {
		difX = 0;
		difY = 0;
	} else {
		difX = xpos - m_mouse->posX;
		difY = ypos - m_mouse->posY;
	}

	m_mouse->posX = xpos;
	m_mouse->posY = ypos;
	
	if (m_mouse->button1) {
		input::mouse::dragged(difX, difY);
	} else {
		input::mouse::moved(difX, difY);
	}
}

void input::callbacks::mouse::button(int button, bool state) {
	if (button == 1) {
		m_mouse->button1 = state;
	}
}

void input::callbacks::mouse::scroll(double xoffset, double yoffset) {
	input::mouse::scrolled(yoffset);
}

void input::mouse::init() {
	m_mouse = (MouseData*)malloc(sizeof(MouseData));
	m_mouse->posX = -1;
	m_mouse->posY = -1;
	m_mouse->button1 = false;
}

input::MouseData* input::mouse::getMouseData() {
	return m_mouse;
}