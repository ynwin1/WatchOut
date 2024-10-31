#pragma once

#include <array>
#include <utility>

#include "camera.hpp"
#include "common.hpp"
#include "render_components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions = {
		ivec2(20, 28),
		ivec2(20, 34)
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = 
	{ textures_path("jeff/jeff.png"),
	  textures_path("barbarian/barbarian.png"),
	  textures_path("boar/boar.png"),
	  textures_path("archer/archer.png"),
	  textures_path("archer/arrow.png"),
	  textures_path("game/game_over.png"),
	  textures_path("collectables/heart.png"),
	  textures_path("collectables/trapbottle.png"),
	  textures_path("collectables/trap.png"),
	  textures_path("grass_tile/grass_tile.png"),
	  textures_path("tree/tree.png"),
	  textures_path("shrub/shrub.png"),
	  textures_path("rock/rock.png"),
	  textures_path("border/cliff.png")};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {shader_path("textured"), shader_path("untextured"), shader_path("font") };

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;

public:
	GLFWwindow* create_window();

	// Initialize the window
	bool init(Camera* camera);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlGeometryBuffers();

	void initHealthBarBuffer();
	void initStaminaBarBuffer();

	void initText();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	void turn_damaged_red(std::vector<Entity>& was_damaged);

	void step(float elapsed_ms);

	mat3 createProjectionMatrix();

private:
	Camera* camera;

	// Internal drawing functions for each entity type
	void drawMesh(Entity entity, const mat3& projection);

	void drawText(Entity entity);

	void update_hpbars();

	void update_staminabars();

	void initMapTileBuffer();

	void updateEntityFacing();

	// Window handle
	GLFWwindow* window;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);

float worldToVisualY(float y, float z);
float visualToWorldY(float y);
vec2 worldToVisual(vec3 pos);
static const float yConversionFactor = 1 / sqrt(2);
static const float zConversionFactor = 1 / sqrt(2);