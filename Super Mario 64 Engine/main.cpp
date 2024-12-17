#include <filesystem>
namespace fs = std::filesystem;

#include "model.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Integers defining window width and height
const unsigned int windowWidth = 640;
const unsigned int windowHeight = 480;

// Main Function
int main() {
	// Initialize GLFW
	glfwInit();

	// Tell GLFW we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Define the window size and title using GLFW
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Super Mario 64", NULL, NULL);

	// Throw an error if the window fails to create
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Load GLAD (set to OpenGL)
	gladLoadGL();

	glViewport(0, 0, windowWidth, windowHeight);

	// Initialize ImGUi
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	float camFov = 50.0f;
	float bgColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float lightColor[4] = { 0.0f, 1.0f, 0.8f, 0.8f };

	// Generates Shader object using shaders defualt.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Light related stuff
	glm::vec3 lightPos = glm::vec3(0.0f, 0.8f, 0.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	// Model Path
	Model model("models/sword/scene.gltf");

	// Main while loop
	while (!glfwWindowShouldClose(window)) {
		// Tell ImGui to start rendering frames
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Only show ImGui if holding F1
		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
			ImGui::Begin("RT64 Reference?!?!");
			ImGui::Text("Dario is watching me...");
			ImGui::ColorEdit3("Background Color", bgColor);
			ImGui::ColorEdit3("Light Color", lightColor);
			ImGui::SliderFloat("Camera FOV", &camFov, 15.0f, 120.0f);
			ImGui::SliderFloat("Light Pos X", &lightPos.x, -10.0f, 10.0f);
			ImGui::SliderFloat("Light Pos Y", &lightPos.y, 1.0f, 10.0f);
			ImGui::SliderFloat("Light Pos Z", &lightPos.z, -10.0f, 10.0f);
			ImGui::End();
		}

		// Color of Background
		glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);

		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (!io.WantCaptureMouse) {
			// Handles camera inputs
			camera.Inputs(window);
		}

		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(camFov, 0.1f, 100.0f);

		shaderProgram.Activate();
		glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

		// Draw Model
		model.Draw(shaderProgram, camera);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);

		// Take care of all GLFW events
		glfwPollEvents();
	}

	// End all ImGui functions
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Delete all the objects we've created
	shaderProgram.Delete();

	// Delete window before ending the program
	glfwDestroyWindow(window);

	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}