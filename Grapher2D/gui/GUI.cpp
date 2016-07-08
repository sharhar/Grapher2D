#include "GUI.h"

static void run(GUI* gui) {

}

GUI::GUI() {
	running = true;
	m_thread = std::thread(&run, this);
}

void GUI::stop() {
	running = false;
	m_thread.join();
}