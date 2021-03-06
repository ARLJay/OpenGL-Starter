#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <renderer.h>
#include <vector.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Camera
static float zoom = -1.0f;
static float x = 1.0f;
static float y = 1.0f;

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		y -= 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		y += 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		x -= 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		x += 0.05f;
	}

	if(glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		zoom+= 0.05f;
	}
	if(glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		zoom-= 0.05f;
	}
	setOrthoMatrix(-5.0*zoom, 5.0*zoom, -5.0*zoom, 5.0*zoom, -1.0f, 10.0f);
	setViewMatrix(x, y);

	glUniformMatrix4fv(glGetUniformLocation(getShader(), "projection"), 1, GL_FALSE, getOrthoMatrix());
	glUniformMatrix4fv(glGetUniformLocation(getShader(), "view"), 1, GL_FALSE, getViewMatrix());
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	if(yoffset < 0)
	{
		zoom += 0.05f;
	} else {
		zoom -= 0.05f;
	}
	setOrthoMatrix(-5.0*zoom, 5.0*zoom, -5.0*zoom, 5.0*zoom, 0.0f, 10.0f);
}

int main(void){
	glfwInit();

	const char* title = "OpenGL";
	const unsigned int width = 800;
	const unsigned int height = 600;

	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if(window == NULL)
	{
		printf("Failed to initialise GLFW window\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialise GLAD\n");
		return -1;
	}

	glViewport(0, 0, width, height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	renderer_shader_init("assets/shaders/vert.glsl", "assets/shaders/frag.glsl");
	renderer_init();

	unsigned int tex = LoadTexture("./assets/textures/tex.png");
	renderer_add_texture(tex);

	vec2 quad0Pos = {-1.5f,-0.5f};
	vec2 quad1Pos = {0.5f,-0.5f};

	vec2 quadSize = {1.0f, 1.0f};
	vec4 quadColour = {1.0f, 0.0f, 0.0f, 1.0f};

	while(!glfwWindowShouldClose(window))
	{
		renderer_begin_batch();

		renderer_draw_quad_colour(quad0Pos, quadSize, quadColour);
		renderer_draw_quad_texture(quad1Pos, quadSize, tex);

		renderer_end_batch();
		renderer_flush();

		processInput(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}    
	renderer_terminate();
	glfwTerminate();
	return 0;
}
