#pragma once

#include <array>
#include <utility>

#include "camera.hpp"
#include "common.hpp"
#include "render_components.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"

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

	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::TREE, mesh_path("tree.obj"))
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = 

	{ 
		textures_path("barbarian/Idle32x36.png"),     // BARBARIAN_IDLE
		textures_path("barbarian/Run32x36.png"),      // BARBARIAN_RUN
		textures_path("barbarian/Dead32x36.png"),     // BARBARIAN_DEAD
		textures_path("boar/idle1f28x19.png"),        // BOAR_IDLE
		textures_path("boar/run7f28x19.png"),         // BOAR_RUN
		textures_path("archer/Idle-4f-32x36.png"),    // ARCHER_IDLE
		textures_path("archer/Run-6f-33x34.png"),     // ARCHER_RUN
		textures_path("archer/Dead-1f-32x36.png"),    // ARCHER_DEAD
		textures_path("archer/BowDraw-10f-33x34.png"),// ARCHER_BOW_DRAW
		textures_path("archer/arrow.png"),            // ARROW
		textures_path("wizard/Idle-4f-96x35.png"),    // WIZARD_IDLE
		textures_path("wizard/Run-6f-96x35-Sheet.png"),// WIZARD_RUN
		textures_path("wizard/Death-Sheet-6f-96x35.png"),// WIZARD_DEAD
		textures_path("wizard/fireball-6f.png"),         // FIREBALL Source: https://nyknck.itch.io/pixelarteffectfx017
		textures_path("wizard/lightning.png"),       // LIGHTNING Source: https://sanctumpixel.itch.io/lightning-lines-pixel-art-effect
		textures_path("wizard/target.png"),           // TARGET AREA
		textures_path("jeff/32Run.png"),              // JEFF_RUN
		textures_path("jeff/32Idle.png"),             // JEFF_IDLE
		textures_path("jeff/32Jump.png"),             // JEFF_JUMP
		textures_path("collectables/heart.png"),      // HEART
		textures_path("collectables/heart_fade.png"),
		textures_path("collectables/trapbottle.png"), // TRAPCOLLECTABLE
		textures_path("collectables/trapbottle_fade.png"),
		textures_path("collectables/trap.png"),       // TRAP
		textures_path("grass_tile/grass_tile.png"),   // GRASS_TILE
		textures_path("tree/tree.png"),               // TREE
		textures_path("shrub/shrub.png"),             // SHRUB
		textures_path("rock/rock.png"),               // ROCK
		textures_path("border/cliff.png"),            // BOTTOM CLIFF
	  	textures_path("border/cliff2.png"),           // SIDE CLIFF
	  	textures_path("border/cliffTop.png"),         // TOP CLIFF
	  	textures_path("menu/HelpMenu.png"),           // MENU_HELP
		textures_path("menu/PauseMenu.png")           // MENU_PAUSED
	};


	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("textured"), 
		shader_path("untextured"), 
		shader_path("animated"), 
		shader_path("font"), 
		shader_path("tree")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

	void update_animations();
	void update_jeff_animation();

public:
	GLFWwindow* create_window();

	// Initialize the window
	bool init(Camera* camera);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlGeometryBuffers();

	void initRectangleBuffer();

	void initText();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	void turn_damaged_red(std::vector<Entity>& was_damaged);

	void step(float elapsed_ms);

	mat3 createProjectionMatrix();
	mat4 createProjectionToScreenSpace();

private:
	Camera* camera;

	// Internal drawing functions for each entity type
	void drawMesh(Entity entity, const mat3& projection, const mat4& projection_screen);

	void drawText(Entity entity);

	void update_hpbars();

	void update_staminabars();

	void initMapTileBuffer();

	void updateEntityFacing();

	void updateCollectedPosition();

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