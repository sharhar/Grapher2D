#include <GLFW/glfw3.h>

int main() {
	GLFWwindow* window;

	glfwInit();

	window = glfwCreateWindow(800, 600, "GLFW Window!", NULL, NULL);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	return 0;
}