#pragma once

#include <array>
#include <utility>

#include "camera.hpp"
#include "common.hpp"
#include "render_components.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "sound_system.hpp"
#include "particle_system.hpp"


const int MAX_POINT_LIGHTS = 3;

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
		textures_path("jeff/phantom-jeff.png"),		  // JEFF_PHANTOM_TRAP
		textures_path("collectables/heart.png"),      // HEART
		textures_path("collectables/heart_fade.png"),
		textures_path("collectables/trapbottle.png"), // TRAPCOLLECTABLE
		textures_path("collectables/trapbottle_fade.png"),
		textures_path("collectables/trap.png"),       // TRAP
		textures_path("collectables/bow.png"),
		textures_path("collectables/bow_fade.png"),
		textures_path("collectables/bow_draw.png"),
		textures_path("collectables/bow_drawn.png"),
		textures_path("collectables/phantom_trap_bottle.png"), // PHANTOM_TRAP_BOTTLE
		textures_path("collectables/phantom_trap_bottle_fade.png"),
		textures_path("collectables/phantom_trap_bottle_one.png"), // PHANTOM_TRAP_BOTTLE OF 1 FRAME
		textures_path("grass_tile/grass_tile.png"),   // GRASS_TILE
		textures_path("tree/tree.png"),               // TREE
		textures_path("shrub/shrub.png"),             // SHRUB
		textures_path("rock/rock.png"),               // ROCK
		textures_path("border/cliff.png"),            // BOTTOM CLIFF
	  	textures_path("border/cliff2.png"),           // SIDE CLIFF
	  	textures_path("border/cliffTop.png"),         // TOP CLIFF
	  	textures_path("menu/HelpMenu.png"),           // MENU_HELP
		textures_path("menu/PauseMenu.png"),          // MENU_PAUSED
		textures_path("tutorial/Tutorial1.png"),      // TUTORIAL SLIDE 1
		textures_path("tutorial/Tutorial2.png"),      // TUTORIAL SLIDE 2
		textures_path("tutorial/Tutorial3.png"),      // TUTORIAL SLIDE 3
		textures_path("tutorial/Tutorial4.png"),      // TUTORIAL SLIDE 4
		textures_path("enemy_intros/boar.png"),       // BOAR INTO
		textures_path("enemy_intros/bird.png"),       // BIRD INTRO
		textures_path("enemy_intros/wizard.png"),     // WIZARD INTRO
		textures_path("enemy_intros/troll.png"),      // TROLL INTRO
		textures_path("enemy_intros/archer.png"),     // ARCHER INTRO
		textures_path("enemy_intros/barbarian.png"),  // BARBARIAN INTRO
		textures_path("enemy_intros/bomber.png"),  // BAOMBER INTRO
		textures_path("enemy_intros/target.png"),     // ENEMY TARGET AREA
		textures_path("collectible_intros/heart.png"),// HEART INTRO
		textures_path("collectible_intros/trap.png"), // TRAP INTRO
		textures_path("collectible_intros/phantom_trap.png"), // PHANTOM TRAP INTRO
		textures_path("collectible_intros/bow.png"), // BOW INTRO
		textures_path("collectible_intros/bomb.png"), // BOMB INTRO
		textures_path("bird/bird_fly.png"),			  	      // BIRD FLY
		textures_path("bird/bird_swoop.png"),		          // BIRD SWOOP
		textures_path("bird/bird_dead.png"),		          // BIRD DEAD
		textures_path("troll/Troll-6f-48x64.png"),
		textures_path("troll/Troll-1f-48x64.png"),
		textures_path("misc/crosshair.png"),
		textures_path("bomber/Idle.png"),
		textures_path("bomber/Run.png"),
		textures_path("bomber/Dead.png"),
		textures_path("bomb/bomb.png"),
		textures_path("bomb/bomb_fused.png"),
		textures_path("bomb/bomb_fade.png"),
		textures_path("explosion/explosion.png"),		// https://craftpix.net/freebies/free-animated-explosion-sprite-pack/
		textures_path("title_screen/titleBackground.png"), // TITLE SCREEN BACKGROUND
		textures_path("title_screen/titleText.png"), // TITLE SCREEN TEXT
		textures_path("particles/smoke_01.png")
	};

	// This should be in the same order as texture_paths
	std::array<GLuint, texture_count> normal_gl_handles;

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("textured"),
		shader_path("textured_basic"), 
		shader_path("textured_normal"),
		shader_path("untextured"), 
		shader_path("animated"), 
		shader_path("animated_normal"), 
		shader_path("font"), 
		shader_path("tree"),
		shader_path("particle")
	};
	std::array<GLuint, effect_count> in_position_locations;
	std::array<GLuint, effect_count> in_texcoord_locations;
	std::array<GLuint, effect_count> to_screen_locations;
	std::array<std::array<GLuint, MAX_POINT_LIGHTS * 7>, effect_count> point_light_uniform_locations;

	std::array<Mesh, geometry_count> meshes;

	void update_animations();
	void update_jeff_animation();
	void update_bow_animations();

public:
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;

	GLFWwindow* create_window();

	// Initialize the window
	bool init(Camera* camera, ParticleSystem* particles, SoundSystem* sound);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

    void initializeGlNormals();

    void initializeGlEffects();

	void initializeGlGeometryBuffers();

	void initializeGlAttributeLocations();

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

	vec2 worldToScreen(vec3 worldPos);
	vec2 mouseToScreen(vec2 mousePos);
	vec3 mouseToWorld(vec2 mousePos);

private:
	SoundSystem* sound;
	Camera* camera;
	ParticleSystem* particles;
	const float AMBIENT_LIGHT = 0.2;

	// Internal drawing functions for each entity type
	void drawMesh(Entity entity, const mat3& projection, const mat4& projection_screen);

    void bindModelMatrix(const GLuint program, Transform3D &modelMatrix);

    void bindAnimationAttributes(const GLuint program, const Entity &entity);

    void bindTextureAttributes(const GLuint program, const Entity &entity, const GLuint effect_id);

    void bindNormalMap(const GLuint program, const Entity &entity);

    void bindLightingAttributes(const GLuint program, const Entity &entity);

	void bindPointLights(const GLuint program, const Entity& entity, const Motion& motion, const GLuint effect_id);

	void drawText(Entity entity, const mat4& projection_screen);

	void update_hpbars();

	void update_staminabars();

	void initMapTileBuffer();

	void updateEntityFacing();

    void updateCollectedPosition();

	void updateSlideUps(float elapsed_ms);
	void updateExplosions(float elapsed_ms);

	// Window handle
	GLFWwindow* window;
};

std::vector<vec2> getTextRenderPositions(std::string textValue, float scale, float lineSpacing, TEXT_ALIGNMENT alignment, vec2 alignmentPos);

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, GLuint& out_program);

float worldToVisualY(float y, float z);
float visualToWorldY(float y);
vec2 worldToVisual(vec3 pos);
static const float yConversionFactor = 1 / sqrt(2);
static const float zConversionFactor = 1 / sqrt(2);