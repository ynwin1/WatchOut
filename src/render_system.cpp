// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawMesh(Entity entity, const mat3& projection)
{
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	if(registry.motions.has(entity)) {
		Motion& motion = registry.motions.get(entity);
		transform.translate(motion.position);
		transform.rotate(motion.angle);
		transform.scale(motion.scale);
	}
	else if(registry.staticMotions.has(entity)) {
		StaticMotion& motion = registry.staticMotions.get(entity);
		transform.translate(motion.position);
		transform.rotate(motion.angle);
		transform.scale(motion.scale);
	}

	assert(registry.renderRequests.has(entity));
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();
	
	// set attributes for textured meshes
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		gl_has_errors();
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // stride to skip position data
		gl_has_errors();

		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Getting uniform locations for glUniform* calls
		GLint colour_uloc = glGetUniformLocation(program, "entity_colour");
		const vec3 colour = registry.colours.has(entity) ? registry.colours.get(entity) : vec3(1);
		glUniform3fv(colour_uloc, 1, (float*)&colour);
		gl_has_errors();
	}
	// set attributes for untextured meshes
	else if(render_request.used_effect == EFFECT_ASSET_ID::UNTEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint color_uloc = glGetUniformLocation(program, "color");
		gl_has_errors();
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(UntexturedVertex), (void*)0);
		gl_has_errors();
		const vec3 color = registry.meshColours.has(entity) ? registry.meshColours.get(entity).vec3 : vec3(1);
		glUniform3fv(color_uloc, 1, (float *)&color);
	} else {
		assert(false && "Type of render request not supported");
	}

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h); // Note, this will be 2x the resolution given to glfwCreateWindow on retina displays

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(GLfloat(117. / 255), GLfloat(138. / 255), GLfloat(101. / 255), 1.0);
	glClearDepth(10.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
	// and alpha blending, one would have to sort
	// sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	//Draw all textured meshes that have a position and size component
	for (Entity entity : registry.renderRequests.entities)
	{
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawMesh(entity, projection_2D);
	}

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}


void RenderSystem::step(float elapsed_ms)
{
	for (Entity entity : registry.damageds.entities) {
		Damaged& timer = registry.damageds.get(entity);
		timer.timer -= elapsed_ms;
		if (timer.timer < 0) {
			registry.damageds.remove(entity);
			if (registry.colours.has(entity)) {
				registry.colours.remove(entity);
			}
		}
	}

	update_hpbar_color();
}

void RenderSystem::turn_damaged_red(std::vector<Entity>& was_damaged)
{
	const float DAMAGE_TIME = 400;
	const vec3 DAMAGE_COLOUR = { 1, 0, 0 };
	for (Entity entity : was_damaged) {
		if (registry.damageds.has(entity)) {
			registry.damageds.get(entity).timer = DAMAGE_TIME;
		}
		else {
			registry.damageds.insert(entity, { DAMAGE_TIME });
			if (registry.colours.has(entity)) {
				registry.colours.get(entity) = DAMAGE_COLOUR;
			}
			else {
				registry.colours.insert(entity, DAMAGE_COLOUR);
			}
		}
	}
}

void RenderSystem::update_hpbar_color() {
	for (Entity entity : registry.players.entities) {
		Player& player = registry.players.get(entity);
		HealthBar& hpbar = registry.healthBars.get(entity);
		MeshColour& colour = registry.meshColours.get(hpbar.meshEntity);

		if(player.health < 100) {
			colour.vec3 = vec3(1.0f, 0.0f, 0.0f);
		}
	}
}

mat3 RenderSystem::createProjectionMatrix() 
{
	float left;
	float right;
	float bottom;
	float top;

	if (camera->isToggled()) {
		// viewing screen is based on camera position and its dimensions
		left = camera->getPosition().x - (camera->getWidth() / 2);
		right = camera->getPosition().x + (camera->getWidth() / 2);
		bottom = camera->getPosition().y + (camera->getHeight() / 2);
		top = camera->getPosition().y - (camera->getHeight() / 2);
	}
	else {
		top = 0;
		left = 0;
		right = world_size_x;
		bottom = world_size_y;
	}

	gl_has_errors();

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}