#include "GUI.h"

static void run(GUI* gui) {
	gui->m_window = glfwCreateWindow(300, 600, "GUI", NULL, NULL);
	glfwMakeContextCurrent(gui->m_window);

	const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glfwSetWindowPos(gui->m_window,
		(vidMode->width - 300) / 2 - 600,
		(vidMode->height - 600) / 2);

	glfwSwapInterval(1);

	glClearColor(1, 0, 1, 1);

	while (gui->running && !glfwWindowShouldClose(gui->m_window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(gui->m_window);
	}

	glfwDestroyWindow(gui->m_window);

	gui->running = false;
}

GUI::GUI() {
	running = true;
	m_thread = std::thread(&run, this);
}

void GUI::stop() {
	running = false;
	m_thread.join();
}