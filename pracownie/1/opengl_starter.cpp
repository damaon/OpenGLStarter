// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <common/shader.hpp>

#include <math.h>
#include <vector>

struct geo{
	double x;
	double y;
	geo(double _x, double _y) : x(_x), y(_y) // Create an object of type _book.
	{
	}
};

void geoGetMinMax(std::vector<geo> &cord, double & min_x, double & max_x, double & min_y, double & max_y){
	for (int i = 0; i < cord.size(); i++){
		{
			geo c = cord[i];
			if (c.x > max_x) max_x = c.x;
			if (c.x < min_x) min_x = c.x;
			if (c.y > max_y) max_y = c.y;
			if (c.y < min_y) min_y = c.y;
		}
	}
}

void normalizeGeo(std::vector<geo> &cord, double min_x, double max_x, double min_y, double max_y){
	double diff_x = abs(min_x - max_x);
	fprintf(stdout, "\nDIFF_X - %f", diff_x);
	double diff_y = abs(min_y - max_y);
	fprintf(stdout, "\nDIFF_Y - %f", diff_y);
	for (int i = 0; i < cord.size(); i++){
		cord[i].x = ((cord[i].x - min_x) / diff_x) * 2.0f - 1.0f;
		cord[i].y = ((cord[i].y - min_y) / diff_y) * 2.0f - 1.0f;
	}
}

void geoToVertices(std::vector<geo> &cord, std::vector<GLfloat> & vertices){
	for (int i = 0; i < cord.size(); i++){
		geo c = cord[i];
		vertices.push_back(c.x);
		vertices.push_back(c.y);
		vertices.push_back(float(i) / float(cord.size()));
	}
}

int main( void )
{
	geo g1(53.11, 22.33);
	geo g2(20.11, 15.33);
	geo g3(55.11, 45.33);
	geo g4(35.11, 67.33);
	std::vector<geo> cord;
	cord.push_back(g1);
	cord.push_back(g2);
	cord.push_back(g3);
	cord.push_back(g4);
	std::vector<GLfloat> vertices;
	fprintf(stdout, "\nElo elo 3 5 0!\n");

	double min_x, max_x, min_y, max_y;
	min_x = max_x = g1.x;
	min_y = max_y = g1.y;
	geoGetMinMax(cord, min_x, max_x, min_y, max_y);
	normalizeGeo(cord, min_x, max_x, min_y, max_y);

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
	GLint uni_cameras = glGetUniformLocation(programID, "CameraS");
	geoToVertices(cord, vertices);
	for (int i = 0; i < vertices.size(); i++) fprintf(stdout, "\nv - %f", vertices[i]);
	static const GLfloat * g_vertex_buffer_data = &vertices[0];

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*vertices.size(), g_vertex_buffer_data, GL_STATIC_DRAW);


	// zmienne
	bool key_down[500] = { 0 };
	bool key_used[500] = { 0 };
	vec2 kamera(0.0f, 0.0f);
	GLfloat zoom = 1.0f;

	do{
		if (uni_cameras != -1) glUniform3f(uni_cameras, kamera.x, kamera.y, zoom);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
			);

		// Draw the PATH
		glDrawArrays(GL_LINE_STRIP, 0, cord.size()); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	
		// keyboard
		for (int i = 0; i < 500; i++){
			if (glfwGetKey(window, i) == GLFW_PRESS){
				key_down[i] = true && !key_used[i];
			}
			else {
				key_down[i] = false;
				key_used[i] = false;
			}
		}
		//check other keys
		if (key_down[GLFW_KEY_MINUS]){
			key_used[GLFW_KEY_MINUS] = true;
			if (zoom > 0.2f) zoom /= 2.0f;
		}
		else if (key_down[GLFW_KEY_EQUAL]){
			key_used[GLFW_KEY_EQUAL] = true;
			if (zoom < 2.0f) zoom *= 2.0f;
		}

		if (key_down[GLFW_KEY_UP]){
			key_used[GLFW_KEY_UP] = true;
			kamera.y -= 0.1f;
		}
		if (key_down[GLFW_KEY_DOWN]){
			key_used[GLFW_KEY_DOWN] = true;
			kamera.y += 0.1f;
		}
		if (key_down[GLFW_KEY_LEFT]){
			key_used[GLFW_KEY_LEFT] = true;
			kamera.x += 0.1f;
		}
		if (key_down[GLFW_KEY_RIGHT]){
			key_used[GLFW_KEY_RIGHT] = true;
			kamera.x -= 0.1f;
		}
		if (key_down[GLFW_KEY_R]){
			key_used[GLFW_KEY_RIGHT] = true;
			kamera.x = 0.0f;
			kamera.y = 0.0f;
			zoom = 1.0f;
		}

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

