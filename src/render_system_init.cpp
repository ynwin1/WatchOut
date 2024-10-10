// internal
#include "render_system.hpp"

#include <array>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"

// This creates circular header inclusion, that is quite bad.
#include "tiny_ecs_registry.hpp"

// stlib
#include <iostream>
#include <sstream>

// Debugging
namespace {
	void glfw_err_cb(int error, const char* desc) {
		fprintf(stderr, "%d: %s", error, desc);
	}
}

GLFWwindow* RenderSystem::create_window(Camera* camera) {
	this->camera = camera;

	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(this->camera->getWidth(), this->camera->getHeight(), "Watch Out!", nullptr, nullptr);
	if (window == nullptr) {
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}

	// Get the primary monitor and its video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Make the window fullscreen by resizing and positioning it
    glfwSetWindowPos(window, 0, 0); 
    glfwSetWindowSize(window, mode->width, mode->height); 
    glfwMakeContextCurrent(window);

	

	return window;
}


// World initialization
bool RenderSystem::init()
{
	
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	const int is_fine = gl3w_init();
	assert(is_fine == 0);

	// We are not really using VAO's but without at least one bound we will crash in
	// some systems.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	gl_has_errors();

	initializeGlTextures();
	initializeGlEffects();
	initializeGlGeometryBuffers();

	return true;
}

void RenderSystem::initializeGlTextures()
{
	glGenTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());

	for (uint i = 0; i < texture_paths.size(); i++)
	{
		const std::string& path = texture_paths[i];
		ivec2& dimensions = texture_dimensions[i];

		stbi_uc* data;
		data = stbi_load(path.c_str(), &dimensions.x, &dimensions.y, NULL, 4);

		if (data == NULL)
		{
			const std::string message = "Could not load the file " + path + ".";
			fprintf(stderr, "%s", message.c_str());
			assert(false);
		}
		glBindTexture(GL_TEXTURE_2D, texture_gl_handles[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions.x, dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		gl_has_errors();
		stbi_image_free(data);
	}
	gl_has_errors();
}

void RenderSystem::initializeGlEffects()
{
	for (uint i = 0; i < effect_paths.size(); i++)
	{
		const std::string vertex_shader_name = effect_paths[i] + ".vs.glsl";
		const std::string fragment_shader_name = effect_paths[i] + ".fs.glsl";

		bool is_valid = loadEffectFromFile(vertex_shader_name, fragment_shader_name, effects[i]);
		assert(is_valid && (GLuint)effects[i] != 0);
	}
}

// One could merge the following two functions as a template function...
template <class T>
void RenderSystem::bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(uint)gid]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	gl_has_errors();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(uint)gid]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
	gl_has_errors();
}

void RenderSystem::initializeGlGeometryBuffers()
{
	// Vertex Buffer creation.
	glGenBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	// Index Buffer creation.
	glGenBuffers((GLsizei)index_buffers.size(), index_buffers.data());

	//////////////////////////
	// Initialize sprite
	// The position corresponds to the center of the texture.
	std::vector<TexturedVertex> textured_vertices(4);
	textured_vertices[0].position = { -1.f / 2, +1.f / 2, 0.f };
	textured_vertices[1].position = { +1.f / 2, +1.f / 2, 0.f };
	textured_vertices[2].position = { +1.f / 2, -1.f / 2, 0.f };
	textured_vertices[3].position = { -1.f / 2, -1.f / 2, 0.f };
	textured_vertices[0].texcoord = { 0.f, 1.f };
	textured_vertices[1].texcoord = { 1.f, 1.f };
	textured_vertices[2].texcoord = { 1.f, 0.f };
	textured_vertices[3].texcoord = { 0.f, 0.f };

	// Counterclockwise as it's the default opengl front winding direction.
	const std::vector<uint16_t> textured_indices = { 0, 3, 1, 1, 3, 2 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::SPRITE, textured_vertices, textured_indices);

	// initialize game space triangles
	std::vector<TexturedVertex> game_space_vertices(4);
	game_space_vertices[0].position = {0.0f, 0.0f, 0.0f};
	game_space_vertices[1].position = {(float)window_width_px, 0.0f, 0.0f};
	game_space_vertices[2].position = {(float)window_width_px, (float)window_height_px, 0.0f};
	game_space_vertices[3].position = {0.0f, (float)window_height_px, 0.0f };
	game_space_vertices[0].texcoord = { 0.f, 0.f };
	game_space_vertices[1].texcoord = { 1.f, 0.f };
	game_space_vertices[2].texcoord = { 1.f, 1.f };
	game_space_vertices[3].texcoord = { 0.f, 1.f };

	const std::vector<uint16_t> game_space_indices = { 0, 1, 2, 0, 2, 3 };
	bindVBOandIBO(GEOMETRY_BUFFER_ID::GAME_SPACE, game_space_vertices, game_space_indices);
}

RenderSystem::~RenderSystem()
{
	// Don't need to free gl resources since they last for as long as the program,
	// but it's polite to clean after yourself.
	glDeleteBuffers((GLsizei)vertex_buffers.size(), vertex_buffers.data());
	glDeleteBuffers((GLsizei)index_buffers.size(), index_buffers.data());
	glDeleteTextures((GLsizei)texture_gl_handles.size(), texture_gl_handles.data());
	gl_has_errors();

	for (uint i = 0; i < effect_count; i++) {
		glDeleteProgram(effects[i]);
	}
	// delete allocated resources
	gl_has_errors();

	// remove all entities created by the render system
	while (registry.renderRequests.entities.size() > 0)
		registry.remove_all_components_of(registry.renderRequests.entities.back());

	glfwDestroyWindow(window);
}

bool gl_compile_shader(GLuint shader)
{
	glCompileShader(shader);
	gl_has_errors();
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint log_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
		std::vector<char> log(log_len);
		glGetShaderInfoLog(shader, log_len, &log_len, log.data());
		glDeleteShader(shader);

		gl_has_errors();

		fprintf(stderr, "GLSL: %s", log.data());
		return false;
	}

	return true;
}

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program)
{
	// Opening files
	std::ifstream vs_is(vs_path);
	std::ifstream fs_is(fs_path);
	if (!vs_is.good() || !fs_is.good())
	{
		fprintf(stderr, "Failed to load shader files %s, %s", vs_path.c_str(), fs_path.c_str());
		assert(false);
		return false;
	}

	// Reading sources
	std::stringstream vs_ss, fs_ss;
	vs_ss << vs_is.rdbuf();
	fs_ss << fs_is.rdbuf();
	std::string vs_str = vs_ss.str();
	std::string fs_str = fs_ss.str();
	const char* vs_src = vs_str.c_str();
	const char* fs_src = fs_str.c_str();
	GLsizei vs_len = (GLsizei)vs_str.size();
	GLsizei fs_len = (GLsizei)fs_str.size();

	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vs_src, &vs_len);
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fs_src, &fs_len);
	gl_has_errors();

	// Compiling
	if (!gl_compile_shader(vertex))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}
	if (!gl_compile_shader(fragment))
	{
		fprintf(stderr, "Vertex compilation failed");
		assert(false);
		return false;
	}

	// Linking
	out_program = glCreateProgram();
	glAttachShader(out_program, vertex);
	glAttachShader(out_program, fragment);
	glLinkProgram(out_program);
	gl_has_errors();

	{
		GLint is_linked = GL_FALSE;
		glGetProgramiv(out_program, GL_LINK_STATUS, &is_linked);
		if (is_linked == GL_FALSE)
		{
			GLint log_len;
			glGetProgramiv(out_program, GL_INFO_LOG_LENGTH, &log_len);
			std::vector<char> log(log_len);
			glGetProgramInfoLog(out_program, log_len, &log_len, log.data());
			gl_has_errors();

			fprintf(stderr, "Link error: %s", log.data());
			assert(false);
			return false;
		}
	}

	// No need to carry this around. Keeping these objects is only useful if we recycle
	// the same shaders over and over, which we don't, so no need and this is simpler.
	glDetachShader(out_program, vertex);
	glDetachShader(out_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	gl_has_errors();

	return true;
}

