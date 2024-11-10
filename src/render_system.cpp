// internal
#include "render_system.hpp"
#include "tiny_ecs_registry.hpp"

// external
#include <SDL.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STD
#include <algorithm>
#include <sstream>

void RenderSystem::drawText(Entity entity) {
	const Text& text = registry.texts.get(entity);
	const RenderRequest& render_request = registry.renderRequests.get(entity);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);

	float startX = text.position.x;
	float startY = text.position.y;

	std::string::const_iterator c;
    for (c = text.value.begin(); c != text.value.end(); c++)
    {
        TextChar ch = registry.textChars[*c];

        float xpos = startX + ch.bearing.x * text.scale;
		float ypos = startY - (ch.size.y - ch.bearing.y) * text.scale;

        float w = ch.size.x * text.scale;
        float h = ch.size.y * text.scale;
        // update VBO for each character
	  	float vertices[6][4] = {
            	{ xpos,     ypos + h,   0.0f, 0.0f },            
            	{ xpos,     ypos,       0.0f, 1.0f },
            	{ xpos + w, ypos,       1.0f, 1.0f },

            	{ xpos,     ypos + h,   0.0f, 0.0f },
            	{ xpos + w, ypos,       1.0f, 1.0f },
            	{ xpos + w, ypos + h,   1.0f, 0.0f }           
        	};

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

		GLuint textColor_loc = glGetUniformLocation(program, "textColor");
		glUniform3f(textColor_loc, text.colour.x, text.colour.y, text.colour.z);
		gl_has_errors();
		
		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(camera->getSize().x), 0.0f, static_cast<float>(camera->getSize().y));
		GLuint projection_loc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection));
		gl_has_errors();

		glm::mat4 trans = glm::mat4(1.0f);
		unsigned int transformLoc = glGetUniformLocation(program, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(trans));
		gl_has_errors();

        glDrawArrays(GL_TRIANGLES, 0, 6);
        startX += (ch.advance >> 6) * text.scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

void RenderSystem::drawMesh(Entity entity, const mat3& projection, const mat4& projection_screen)
{
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	if (registry.motions.has(entity)) {
		Motion& motion = registry.motions.get(entity);
		if (registry.midgrounds.has(entity)) {
			vec2 visualPos = worldToVisual(vec3(motion.position.x, motion.position.y, motion.position.z));
			transform.translate(visualPos);
		}
		else {
			transform.translate(motion.position);
		}
		transform.rotate(motion.angle);
		transform.scale(motion.scale);
	}
	else if(registry.mapTiles.has(entity)) {
		MapTile& tile = registry.mapTiles.get(entity);
		transform.translate(tile.position);
		transform.scale(tile.scale);
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
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // stride to skip position data
		
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::ANIMATED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // stride to skip position data
		
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Pass frame information to shaders
		GLint numFrames_loc = glGetUniformLocation(program, "num_frames");
    	GLint currentFrame_loc = glGetUniformLocation(program, "current_frame");
		
		AnimationController animationController = registry.animationControllers.get(entity);
		Animation currentAnimation = animationController.animations[animationController.currentState];
		glUniform1f(numFrames_loc, currentAnimation.numFrames);  // Set numFrames value
    	glUniform1f(currentFrame_loc, currentAnimation.currentFrame);  // Set currentFrame value
    	gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::ANIMATED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3)); // stride to skip position data
		
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id = texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		// Pass frame information to shaders
		GLint numFrames_loc = glGetUniformLocation(program, "num_frames");
    	GLint currentFrame_loc = glGetUniformLocation(program, "current_frame");
		
		AnimationController animationController = registry.animationControllers.get(entity);
		Animation currentAnimation = animationController.animations[animationController.currentState];
		glUniform1f(numFrames_loc, currentAnimation.numFrames);  // Set numFrames value
    	glUniform1f(currentFrame_loc, currentAnimation.currentFrame);  // Set currentFrame value
    	gl_has_errors();
	}
	// set attributes for untextured meshes
	else if(render_request.used_effect == EFFECT_ASSET_ID::UNTEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		gl_has_errors();
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(UntexturedVertex), (void*)0);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::TREE) {
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();
	}
	else {
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint colour_uloc = glGetUniformLocation(program, "entity_colour");	
	const vec3 colour = registry.colours.has(entity) ? registry.colours.get(entity) : vec3(1);
	glUniform3fv(colour_uloc, 1, (float*)&colour);
	gl_has_errors();

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);

	GLuint toScreen = glGetUniformLocation(currProgram, "toScreen");

	if(registry.foregrounds.has(entity)) { // screen space
		glUniform1i(toScreen, 1);
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection4");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_screen));
		gl_has_errors();

		// apply transformations
		Foreground& fg = registry.foregrounds.get(entity);
		glm::mat4 transform4 = glm::translate(glm::mat4(1.0f), glm::vec3(fg.position.x, fg.position.y, 0.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(fg.scale.x, fg.scale.y, 1.0f));
		transform4 = transform4 * scale;

		unsigned int transformLoc = glGetUniformLocation(currProgram, "transform4");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(transform4));
		gl_has_errors();
	} else {
		glUniform1i(toScreen, 0);
		// Setting uniform values to the currently bound program
		GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
		glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
		glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
		gl_has_errors();
	}

	if(render_request.primitive_type == PRIMITIVE_TYPE::LINES) {
		GLint buffer_size = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
		gl_has_errors();
		GLsizei num_vertices = buffer_size / sizeof(UntexturedVertex);
		glDrawArrays(GL_LINE_LOOP, 0, num_vertices);
	} else {
		// Get number of indices from index buffer, which has elements uint16_t
		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		gl_has_errors();

		// Drawing of num_indices/3 triangles specified in the index buffer
		GLsizei num_indices = size / sizeof(uint16_t);
		glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	}

	gl_has_errors();
}

// Returns true if entity a is further from the camera
bool renderComparison(Entity a, Entity b)
{
	if (!registry.motions.has(a)) {
		return false;
	}
	if (!registry.motions.has(b)) {
		return true;
	}
	Motion& motionA = registry.motions.get(a);
	Motion& motionB = registry.motions.get(b);
	if (motionA.position.y == motionB.position.y) {
		return a < b;
	}
	return motionA.position.y < motionB.position.y;
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
	mat4 projection_screen = createProjectionToScreenSpace();

	// Draw all background textures
	for (Entity entity : registry.backgrounds.entities) {
		drawMesh(entity, projection_2D, projection_screen);
	}
	
	// Copy entities and sort
	std::vector<Entity> renderOrder = registry.midgrounds.entities;
	std::sort(renderOrder.begin(), renderOrder.end(), renderComparison);
	// Draw all midground textured meshes that have a position and size component
	for (Entity entity : renderOrder) {
		drawMesh(entity, projection_2D, projection_screen);
	}

	// Draw all foreground textures
	for (Entity entity : registry.foregrounds.entities) {
		drawMesh(entity, projection_2D, projection_screen);
	}

	// Draw all text
	for (Entity entity : registry.texts.entities) {
		if(entity == registry.fpsTracker.textEntity && !registry.fpsTracker.toggled) {
			continue; //skip rendering fps if not toggled
		}

		drawText(entity);
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


	// Update animation frames
	for (auto& animationController : registry.animationControllers.components) {
		updateAnimation(animationController.animations[animationController.currentState], elapsed_ms);
	}
	
	for (Entity entity : registry.projectiles.entities) {
		Motion& motion = registry.motions.get(entity);
		if (length(motion.velocity) == 0) {
			Projectile& projectile = registry.projectiles.get(entity);
			projectile.sticksInGround -= elapsed_ms;
			if (projectile.sticksInGround <= 0) {
				registry.remove_all_components_of(entity);
			}
			continue;
		}
		vec2 direction = normalize(worldToVisual(motion.velocity));
		motion.angle = atan2(direction.y, direction.x);
	}
	update_animations();
	update_hpbars();
	update_staminabars();
	updateEntityFacing();
}

void RenderSystem::update_animations() {
	update_jeff_animation();
}

void RenderSystem::update_jeff_animation() {
	for (Entity entity : registry.players.entities) {
		Player& player = registry.players.get(entity);
		Motion& motion = registry.motions.get(entity);
		AnimationController& animationController = registry.animationControllers.get(entity);
		
		// Determine if player is moving
		player.isMoving = player.goingUp || player.goingDown || player.goingLeft || player.goingRight;

		// Determine the player's facing direction
		if (player.goingRight) {
			motion.scale = vec2(std::abs(motion.scale.x), std::abs(motion.scale.y));  // Right
		} else if (player.goingLeft) {
			motion.scale = vec2(-std::abs(motion.scale.x), std::abs(motion.scale.y)); // Left
		}

		Jumper& playerJumper = registry.jumpers.get(entity);

		// Animation state logic
		if (playerJumper.isJumping) {
			animationController.changeState(entity, AnimationState::Jumping);
		} else if (player.isMoving) {
			animationController.changeState(entity, AnimationState::Running);
		} else {
			// Player is idle if no movement keys are pressed
			animationController.changeState(entity, AnimationState::Idle);
		}
	}
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

void handleHpBarBoundsCheck() {
	for(Entity& entity : registry.enemies.entities) {
		HealthBar& hpbar = registry.healthBars.get(entity);
		Motion& motion = registry.motions.get(hpbar.meshEntity);
		float halfScaleX = motion.scale.x / 2;
		float halfScaleY = visualToWorldY(motion.scale.y) / 2;

		if(motion.position.x - halfScaleX  < 0) {
			motion.position.x = halfScaleX;
		} else if(motion.position.x + halfScaleX > world_size_x) {
			motion.position.x = world_size_x - halfScaleX;
		}

		if(motion.position.y - halfScaleY - motion.position.z < 0) {
			motion.position.y = visualToWorldY(motion.scale.y);
			motion.position.z = -5;
		} else if(motion.position.y + halfScaleY > world_size_y) {
			motion.position.y = world_size_y - halfScaleY;
		}

		Motion& hpFrameMotion = registry.motions.get(hpbar.frameEntity);
		hpFrameMotion.position = motion.position;
	}
}

void updateHpBarPositionHelper(const std::vector<Entity>& entities) {
    for (Entity entity : entities) {
	    HealthBar& healthBar = registry.healthBars.get(entity);
        Motion& motion = registry.motions.get(entity);
        Motion& healthBarMotion =  registry.motions.get(healthBar.meshEntity);
         // place above character
        float topOffset = 25;
		healthBarMotion.position.x = motion.position.x - (healthBarMotion.scale.x / 2);
        healthBarMotion.position.y = motion.position.y;
		healthBarMotion.position.z = motion.position.z + visualToWorldY(motion.scale.y) / 2 + topOffset;
		Motion& hpFrameMotion =  registry.motions.get(healthBar.frameEntity);
		hpFrameMotion.position = healthBarMotion.position;
    }   
}

void updateHpBarMeter() {
	for (Entity entity : registry.players.entities) {
		Player& player = registry.players.get(entity);
		HealthBar& hpbar = registry.healthBars.get(entity);
		Foreground& fg = registry.foregrounds.get(hpbar.meshEntity);
		fg.scale.x = hpbar.width * player.health/100.f;
		Text& text = registry.texts.get(hpbar.textEntity);
		std::stringstream ss;
		ss << "HP" << std::string(8, ' ') << std::to_string(player.health) << "/100";
		text.value = ss.str();
	}
	for (Entity entity : registry.enemies.entities) {
		Enemy& enemy = registry.enemies.get(entity);
		HealthBar& hpbar = registry.healthBars.get(entity);
		Motion& motion = registry.motions.get(hpbar.meshEntity);
		motion.scale.x = hpbar.width * enemy.health/100.f;
	}
}

void updateHpBarPosition() {
    updateHpBarPositionHelper(registry.enemies.entities);
}

void RenderSystem::update_hpbars() {
	updateHpBarMeter();
	updateHpBarPosition();
	handleHpBarBoundsCheck();
}

void RenderSystem::update_staminabars() {
	for (Entity entity : registry.players.entities) {
		Player& player = registry.players.get(entity);
		Stamina& stamina = registry.staminas.get(entity);
		StaminaBar& staminabar = registry.staminaBars.get(entity);
		Foreground& fg = registry.foregrounds.get(staminabar.meshEntity);
		fg.scale.x = staminabar.width * stamina.stamina/100.f;
		Text& text = registry.texts.get(staminabar.textEntity);
		std::stringstream ss;
		ss << "Stamina" << std::string(8, ' ') << std::to_string((int)stamina.stamina) << "/100";
		text.value = ss.str();
	}
}

void RenderSystem::updateEntityFacing() {
	for (Motion& motion : registry.motions.components) {
    if (motion.facing.x > 0) {
      motion.scale.x = abs(motion.scale.x);
    }
    else if (motion.facing.x < 0) {
      motion.scale.x = -1.0f * abs(motion.scale.x);
    }
	}
}

mat4 RenderSystem::createProjectionToScreenSpace()  {
	return glm::ortho(0.0f, static_cast<float>(camera->getSize().x), 0.0f, static_cast<float>(camera->getSize().y));
}

mat3 RenderSystem::createProjectionMatrix() 
{
	float left;
	float right;
	float bottom;
	float top;

	if (camera->isToggled()) {
		// viewing screen is based on camera position and its dimensions
		left = camera->getPosition().x - (camera->getSize().x / 2);
		right = camera->getPosition().x + (camera->getSize().x / 2);
		bottom = camera->getPosition().y + (camera->getSize().y / 2);
		top = camera->getPosition().y - (camera->getSize().y / 2);
	}
	else {
		top = 0;
		left = 0;
		right = world_size_x;
		bottom = worldToVisualY(world_size_y, 0);
	}

	gl_has_errors();

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return { {sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f} };
}

float worldToVisualY(float y, float z) 
{
	return y * yConversionFactor - z * zConversionFactor;
}

float visualToWorldY(float y)
{
	return y / yConversionFactor;
}

vec2 worldToVisual(vec3 pos) 
{
	return vec2(pos.x, worldToVisualY(pos.y, pos.z));
}