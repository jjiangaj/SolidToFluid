#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <../stb_image.h>
#include <shader/shader.h>
#include <camera/camera.h>
#include <iostream>
#include "plyReader.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
    // -----------------------------
	glEnable(GL_DEPTH_TEST);

	Shader defaultShader("vshader.vs", "fshader.fs");

	// load the model
	// -----------------------------
	PLYLoader bunny;
	bunny.loadModel("../../resources/bunny.ply");
	float* vertices = bunny.getVertexXYZ();
	int* indices = bunny.getVertexIndex();
	int counts = bunny.getTotalVertices();
	int faces = bunny.getTotalFaces();
	
	for (int i = 0; i < faces; i++) {
		int i1 = indices[3 * i + 0] * 6;
		int i2 = indices[3 * i + 1] * 6;
		int i3 = indices[3 * i + 2] * 6;

		glm::vec3 v1 = glm::vec3(vertices[i1] - vertices[i2], vertices[i1 + 1] - vertices[i2 + 1], vertices[i1 + 2] - vertices[i2 + 2]);
		glm::vec3 v2 = glm::vec3(vertices[i1] - vertices[i3], vertices[i1 + 1] - vertices[i3 + 1], vertices[i1 + 2] - vertices[i3 + 2]);
		glm::vec3 norm = glm::normalize(glm::cross(v1, v2));
		
		vertices[i1 + 3] += norm.x;
		vertices[i1 + 4] += norm.y;
		vertices[i1 + 5] += norm.z;
		vertices[i2 + 3] += norm.x;
		vertices[i2 + 4] += norm.y;
		vertices[i2 + 5] += norm.z;
		vertices[i3 + 3] += norm.x;
		vertices[i3 + 4] += norm.y;
		vertices[i3 + 5] += norm.z;
	}

	// VBO, EBO, VAO
	unsigned int VBO, EBO, VAO;
	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, counts * 6 *sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);
	
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces * 3 * sizeof(GL_FLOAT), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// -----
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// activate the shader
		// ------
		defaultShader.use();
		glm::mat4 view;
		glm::mat4 projection = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		defaultShader.setMat4("view", view);
		defaultShader.setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		defaultShader.setMat4("model", model);

		glm::vec3 lightPos(1.2f, -1.0f, -2.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
		defaultShader.setVec3("light.ambient", ambientColor);
		defaultShader.setVec3("light.diffuse", diffuseColor);
		defaultShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		defaultShader.setVec3("light.position", lightPos);
		defaultShader.setVec3("material.ambient", 0.6f, 0.56f, 0.76f);
		defaultShader.setVec3("material.diffuse", 0.6f, 0.56f, 0.76f);
		defaultShader.setVec3("material.specular", 0.8f, 0.8f, 0.8f);
		defaultShader.setFloat("material.shininess", 32.0f);

		// render the triangle
		// ------
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, faces * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}