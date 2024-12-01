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
#include <glm/gtx/string_cast.hpp>
#include "world_init.hpp"

void RenderSystem::drawText(Entity entity, const mat4& projection_screen) {
	const Text& text = registry.texts.get(entity);
	const Foreground& fg = registry.foregrounds.get(entity);
	const RenderRequest& render_request = registry.renderRequests.get(entity);
	const vec4 colour = registry.colours.has(entity) ? registry.colours.get(entity) : vec4(1.0f, 1.0f, 1.0f, 1.0f);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);

	float startX = fg.position.x;
	float startY = fg.position.y;
	const float scale = fg.scale.x;

	std::string::const_iterator c;
    for (c = text.value.begin(); c != text.value.end(); c++)
    {
        TextChar ch = registry.textChars[*c];

        float xpos = startX + ch.bearing.x * scale;
		float ypos = startY - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
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
		glUniform4fv(textColor_loc, 1, (float*)&colour);
		gl_has_errors();
		
		GLuint projection_loc = glGetUniformLocation(program, "projection");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_screen));
		gl_has_errors();

		glm::mat4 trans = glm::mat4(1.0f);
		unsigned int transformLoc = glGetUniformLocation(program, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(trans));
		gl_has_errors();

        glDrawArrays(GL_TRIANGLES, 0, 6);
        startX += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
}

void RenderSystem::drawMesh(Entity entity, const mat3& projection, const mat4& projection_screen)
{
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Transform transform;
	Transform3D modelMatrix;
	if (registry.motions.has(entity)) {
		Motion& motion = registry.motions.get(entity);
		if (registry.midgrounds.has(entity) || registry.backgrounds.has(entity)) {
			vec2 visualPos = worldToVisual(vec3(motion.position.x, motion.position.y, motion.position.z));
			if (registry.meshPtrs.has(entity)) {
				visualPos.y += motion.scale.y / 20; // corrects the render location of the tree sprite
			}
			transform.translate(visualPos);
		}
		else {
			transform.translate(motion.position);
		}
		transform.rotate(motion.angle);
		transform.scale(motion.scale);

		modelMatrix.translate(motion.position);
		modelMatrix.rotate(motion.angle);
		// TODO: Add a flat component for determining this
		bool flat = registry.mapTiles.has(entity);
		modelMatrix.scale(vec2(motion.scale.x, motion.scale.y / yConversionFactor), flat);
	}
	//else {
	//	registry.list_all_components_of(entity);
	//}

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
        bindTextureAttributes(program, entity, used_effect_enum);
		bindLightingAttributes(program, entity);
		if (registry.motions.has(entity)) {
			bindPointLights(program, entity, registry.motions.get(entity), used_effect_enum);
		}
        bindModelMatrix(program, modelMatrix);
    }
	// set attributes for textured meshes
	else if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED_NORMAL)
	{
		GLint textureSamplerLocation = glGetUniformLocation(program, "sampler0");
		GLint normalSamplerLocation  = glGetUniformLocation(program, "normalSampler");

		glUniform1i(textureSamplerLocation, 0);
		glUniform1i(normalSamplerLocation,  1);
        bindTextureAttributes(program, entity, used_effect_enum);
		bindNormalMap(program, entity);
		bindLightingAttributes(program, entity);
		if (registry.motions.has(entity)) {
			bindPointLights(program, entity, registry.motions.get(entity), used_effect_enum);
		}
        bindModelMatrix(program, modelMatrix);
    }
	else if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED_FLAT) {
		bindTextureAttributes(program, entity, used_effect_enum);
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::ANIMATED)
	{
		bindTextureAttributes(program, entity, used_effect_enum);
        bindAnimationAttributes(program, entity);
		bindLightingAttributes(program, entity);
		if (registry.motions.has(entity)) {
			bindPointLights(program, entity, registry.motions.get(entity), used_effect_enum);
		}
		bindModelMatrix(program, modelMatrix);
    }
	else if (render_request.used_effect == EFFECT_ASSET_ID::ANIMATED_NORMAL)
	{
		GLint textureSamplerLocation = glGetUniformLocation(program, "sampler0");
		GLint normalSamplerLocation  = glGetUniformLocation(program, "normalSampler");
		glUniform1i(textureSamplerLocation, 0);
		glUniform1i(normalSamplerLocation,  1);
		
		bindTextureAttributes(program, entity, used_effect_enum);
        bindAnimationAttributes(program, entity);
		bindLightingAttributes(program, entity);
		bindNormalMap(program, entity);
		if (registry.motions.has(entity)) {
			bindPointLights(program, entity, registry.motions.get(entity), used_effect_enum);
		}
		bindModelMatrix(program, modelMatrix);
    }
	// set attributes for untextured meshes
	else if(render_request.used_effect == EFFECT_ASSET_ID::UNTEXTURED)
	{
		bindLightingAttributes(program, entity);
		GLint in_position_loc = in_position_locations[used_effect_enum];
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
	const vec4 colour = registry.colours.has(entity) ? registry.colours.get(entity) : vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glUniform4fv(colour_uloc, 1, (float*)&colour);
	gl_has_errors();

	GLuint toScreen = to_screen_locations[used_effect_enum];

	if(registry.foregrounds.has(entity)) { // screen space
		glUniform1i(toScreen, 1);
		GLuint projection_loc = glGetUniformLocation(program, "projection4");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_screen));
		gl_has_errors();

		// apply transformations
		Foreground& fg = registry.foregrounds.get(entity);
		glm::mat4 transform4 = glm::translate(glm::mat4(1.0f), glm::vec3(fg.position.x, fg.position.y, 0.0f));
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(fg.scale.x, fg.scale.y, 1.0f));
		transform4 = transform4 * scale;

		unsigned int transformLoc = glGetUniformLocation(program, "transform4");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value_ptr(transform4));
		gl_has_errors();
	} else {
		glUniform1i(toScreen, 0);
		// Setting uniform values to the currently bound program
		GLuint transform_loc = glGetUniformLocation(program, "transform");
		glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);
		GLuint projection_loc = glGetUniformLocation(program, "projection");
		glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
		gl_has_errors();
	}

	if (render_request.primitive_type == PRIMITIVE_TYPE::LINES) {
		GLint buffer_size = 0;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
		gl_has_errors();
		GLsizei num_vertices = buffer_size / sizeof(UntexturedVertex);
		glDrawArrays(GL_LINE_LOOP, 0, num_vertices);
	}
	else {
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

void RenderSystem::bindModelMatrix(const GLuint program, Transform3D &modelMatrix)
{
    GLint modelLoc = glGetUniformLocation(program, "modelMatrix");
    if (modelLoc != -1)
    {
        // std::cout << "gsl::to_string(modelMatrix)" << std::endl;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)&modelMatrix.mat);
    }
    else
    {
        printf("Failed to find modelMatrix uniform!\n");
    }
}

void RenderSystem::bindAnimationAttributes(const GLuint program, const Entity &entity)
{
    // Pass frame information to shaders
    GLint numFrames_loc = glGetUniformLocation(program, "num_frames");
    GLint currentFrame_loc = glGetUniformLocation(program, "current_frame");

    AnimationController animationController = registry.animationControllers.get(entity);
    Animation currentAnimation = animationController.animations[animationController.currentState];
    glUniform1f(numFrames_loc, currentAnimation.numFrames);       // Set numFrames value
    glUniform1f(currentFrame_loc, currentAnimation.currentFrame); // Set currentFrame value
    gl_has_errors();
}

void RenderSystem::bindTextureAttributes(const GLuint program, const Entity &entity, const GLuint effect_id)
{
    GLint in_position_loc = in_position_locations[effect_id];
    GLint in_texcoord_loc = in_texcoord_locations[effect_id];
    gl_has_errors();
    assert(in_texcoord_loc >= 0);

    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
    gl_has_errors();

    glEnableVertexAttribArray(in_texcoord_loc);
    glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3)); // stride to skip position data

    // Enabling and binding texture to slot 0
    glActiveTexture(GL_TEXTURE0);
    gl_has_errors();

    assert(registry.renderRequests.has(entity));
    GLuint texture_id = texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

    glBindTexture(GL_TEXTURE_2D, texture_id);
    gl_has_errors();
}

void RenderSystem::bindNormalMap(const GLuint program, const Entity &entity) {
	// Enabling and binding texture to slot 1
	glActiveTexture(GL_TEXTURE1);
    gl_has_errors();

	assert(registry.renderRequests.has(entity));
	RenderRequest renderRequest = registry.renderRequests.get(entity);
    GLuint normal_id = normal_gl_handles[(GLuint)renderRequest.used_texture];
	glBindTexture(GL_TEXTURE_2D, normal_id);
    gl_has_errors();
}

void RenderSystem::bindLightingAttributes(const GLuint program, const Entity &entity)
{
	// Pass lighting information to shaders
	GLint ambientLight_loc = glGetUniformLocation(program, "ambient_light");
	if (registry.foregrounds.has(entity)) {
		glUniform1f(ambientLight_loc, 1.0);       // Set ambientlight value
	} else {
		glUniform1f(ambientLight_loc, AMBIENT_LIGHT);       // Set ambientlight value
	}
}

void RenderSystem::bindPointLights(const GLuint program, const Entity& entity, const Motion& motion, const GLuint effect_id)
{
	PointLight validPointLights[MAX_POINT_LIGHTS]{};
	int num_point_lights = 0;
	for (auto& pointLight : registry.pointLights.components) {
		float distance = glm::distance(motion.position, pointLight.position);
		if (distance > pointLight.max_distance) {
			continue;
		}
		validPointLights[num_point_lights] = pointLight;
		num_point_lights++;
	}
	GLint location = glGetUniformLocation(program, "num_point_lights");
	if (location == -1) {
		std::cerr << "Uniform 'num_point_lights' not found or optimized out!" << std::endl;
	}
	else {
		glUniform1i(location, max(0, num_point_lights)); // Use glUniform1i for integer uniforms
	}
	for (size_t i = 0; i < MAX_POINT_LIGHTS; ++i) {
		glUniform3fv(point_light_uniform_locations[effect_id][i * 7 + 0], 1, glm::value_ptr(validPointLights[i].position));
		glUniform4fv(point_light_uniform_locations[effect_id][i * 7 + 1], 1, glm::value_ptr(validPointLights[i].ambient));
		glUniform4fv(point_light_uniform_locations[effect_id][i * 7 + 2], 1, glm::value_ptr(validPointLights[i].diffuse));
		glUniform1f (point_light_uniform_locations[effect_id][i * 7 + 3], validPointLights[i].max_distance);
		glUniform1f (point_light_uniform_locations[effect_id][i * 7 + 4], validPointLights[i].constant);
		glUniform1f (point_light_uniform_locations[effect_id][i * 7 + 5], validPointLights[i].linear);
		glUniform1f (point_light_uniform_locations[effect_id][i * 7 + 6], validPointLights[i].quadratic);
	}
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
		if(entity == registry.fpsTracker.textEntity && !registry.fpsTracker.toggled) {
			continue; //skip rendering fps if not toggled
		} else if(registry.texts.has(entity)) {
			drawText(entity, projection_screen);
		} else {
			drawMesh(entity, projection_2D, projection_screen);
		}
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

				if(projectile.type == PROJECTILE_TYPE::TRAP) {
					createDamageTrap({motion.position.x, motion.position.y});
				} else if(projectile.type == PROJECTILE_TYPE::PHANTOM_TRAP) {
					createPhantomTrap({motion.position.x, motion.position.y});
				}

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
	updateCollectedPosition();
}

void RenderSystem::update_animations() {
	update_jeff_animation();
	update_bow_animations();
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

void RenderSystem::update_bow_animations() {
	if(registry.animationControllers.has(registry.inventory.equippedEntity)) {
		Entity entity = registry.inventory.equippedEntity;
		AnimationController& ac = registry.animationControllers.get(entity);
		if(ac.currentState == AnimationState::Attack && 
			ac.animations[ac.currentState].currentFrame == 0 &&
			ac.animations[ac.currentState].elapsedTime == 0) {
			ac.changeState(entity, AnimationState::Default);
		}	
	}
}

void RenderSystem::turn_damaged_red(std::vector<Entity>& was_damaged)
{
	const float DAMAGE_TIME = 400;
	const vec4 DAMAGE_COLOUR = { 1, 0, 0, 1 };
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

void RenderSystem::updateCollectedPosition() {
	Entity& playerE = registry.players.entities[0];
	Motion& playerM = registry.motions.get(playerE);

    for (Entity entity : registry.collected.entities) {
        Motion& collectedM =  registry.motions.get(entity);
        // place above character
        float topOffset = 40;
		collectedM.position.x = playerM.position.x;
        collectedM.position.y = playerM.position.y;
		collectedM.position.z = playerM.position.z + visualToWorldY(playerM.scale.y) / 2 + topOffset;
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

		if(player.health <= 30.0f) {
			vec4 red = {1.0f, 0.0f, 0.0f, 1.0f};
			registry.colours.get(hpbar.meshEntity) = red;
			registry.colours.get(hpbar.frameEntity) = red;
		}
		else if(player.health <= 60.0f) {
			vec4 orange = {1.0f, 0.45f, 0.0f, 1.0f};
			registry.colours.get(hpbar.meshEntity) = orange;
			registry.colours.get(hpbar.frameEntity) = orange;
		} else {
			vec4 green = {0.0f, 1.0f, 0.0f, 1.0f};
			registry.colours.get(hpbar.meshEntity) = green;
			registry.colours.get(hpbar.frameEntity) = green;
		}
	}
	for (Entity entity : registry.enemies.entities) {
		Enemy& enemy = registry.enemies.get(entity);
		HealthBar& hpbar = registry.healthBars.get(entity);
		Motion& motion = registry.motions.get(hpbar.meshEntity);
		motion.scale.x = hpbar.width * enemy.health/enemy.maxHealth;
	}
}

void updateHpBarPosition() {
    updateHpBarPositionHelper(registry.enemies.entities);
}

void RenderSystem::update_hpbars() {
	updateHpBarMeter();
	updateHpBarPosition();
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

vec2 RenderSystem::mouseToScreen(vec2 pos) {
	float screenPosX;
	float screenPosY;

	if (camera->isToggled()) {
		// convert mouse position to screen position
		screenPosX = pos.x;
		screenPosY = camera->getSize().y - pos.y;
	} else {
		int window_width;
		int window_height;
		glfwGetWindowSize(window, &window_width, &window_height);
 
		screenPosX = pos.x ;
		screenPosY = window_height - pos.y;
	}

	return { screenPosX, screenPosY };
}

vec3 RenderSystem::mouseToWorld(vec2 mousePos) {
    float worldPosX;
    float worldPosY;

    if (camera->isToggled()) {
        // top-left corner of the screen in world coordinates
        float cameraLeft = camera->getPosition().x - camera->getSize().x / 2;
        float cameraTop = visualToWorldY(camera->getPosition().y) - visualToWorldY(camera->getSize().y) / 2;

        // Convert mouse position to world position
        worldPosX = cameraLeft + mousePos.x;
        worldPosY = cameraTop + (mousePos.y / yConversionFactor);
    } else {
        int window_width;
        int window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
 
        worldPosX = (mousePos.x * world_size_x) / window_width;
        worldPosY = (mousePos.y * world_size_y) / window_height;
    }

    return { worldPosX, worldPosY, 0.0f };
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