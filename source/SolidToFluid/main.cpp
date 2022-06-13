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
#include "model.h"

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

	Shader bunnyShader("bunny.vs", "bunny.fs");

	// load the model
	// the Model class load the vertices and indices data, 
	// and calculate the vertex normal(to be used for Gouraud shading).
	// The default velocities and forces are set to 0.
	// -----------------------------
	Model bunny("../../resources/bunny.ply");
	float* vertices = bunny.getVertexXYZ();
	int* indices = bunny.getVertexIndex();
	int counts = bunny.getTotalVertices();
	int faces = bunny.getTotalFaces();
	float* velocity = bunny.getVelocity();
	float* forces = bunny.getForce();

	float floorVertices[] = {
		// positions          // normals          
		-2.0f,  -0.25f, -2.0f,  0.0f,  1.0f,  0.0f,
		 2.0f,  -0.25f, -2.0f,  0.0f,  1.0f,  0.0f,
		 2.0f,  -0.25f,  2.0f,  0.0f,  1.0f,  0.0f,
		 2.0f,  -0.25f,  2.0f,  0.0f,  1.0f,  0.0f,
		-2.0f,  -0.25f,  2.0f,  0.0f,  1.0f,  0.0f,
		-2.0f,  -0.25f, -2.0f,  0.0f,  1.0f,  0.0f
	};
	
	// VBO, EBO, VAO
	// -----------------------------
	unsigned int VBO, EBO, VAO;
	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); 
	glBufferData(GL_ARRAY_BUFFER, counts * 6 *sizeof(GL_FLOAT), vertices, GL_STATIC_DRAW);
	//VAO Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces * 3 * sizeof(GL_FLOAT), indices, GL_STATIC_DRAW);
	//UnbindVAO
	glBindVertexArray(0);

	//floorVAO,floorVBO
	// -----------------------------
	unsigned int floorVAO, floorVBO;
	//VAO
	glGenVertexArrays(1, &floorVAO);
	glBindVertexArray(floorVAO);
	//VBO
	glGenBuffers(1, &floorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GL_FLOAT), floorVertices, GL_STATIC_DRAW);
	//VAO Attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	//UnbindVAO
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

		// render the bunny
		// ------
		bunnyShader.use();
		glm::mat4 view;
		glm::mat4 projection = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		bunnyShader.setMat4("view", view);
		bunnyShader.setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		bunnyShader.setMat4("model", model);

		glm::vec3 lightPos(1.2f, -1.0f, -2.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
		bunnyShader.setVec3("light.ambient", ambientColor);
		bunnyShader.setVec3("light.diffuse", diffuseColor);
		bunnyShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		bunnyShader.setVec3("light.position", lightPos);
		bunnyShader.setVec3("material.ambient", 0.6f, 0.56f, 0.76f);
		bunnyShader.setVec3("material.diffuse", 0.6f, 0.56f, 0.76f);
		bunnyShader.setVec3("material.specular", 0.8f, 0.8f, 0.8f);
		bunnyShader.setFloat("material.shininess", 32.0f);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, faces * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// render the floor and wall
		// ------
		glBindVertexArray(floorVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
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