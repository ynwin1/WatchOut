#version 330

struct PointLight {
    vec3 position;  
  
    vec4 ambient;

    float max_distance;
	
    float constant;
    float linear;
    float quadratic;
};


// From vertex shader
in vec2 texcoord;
in vec3 worldPos;

// Application data
uniform sampler2D sampler0;
uniform vec4 entity_colour;

// Lighting data
uniform float ambient_light;

#define MAX_POINT_LIGHTS 3
uniform int num_point_lights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// Output colour
layout(location = 0) out  vec4 colour;

// Function declerations
vec4 CalcPointLight(PointLight light, vec3 fragPos);

void main()
{
    // Used so num_point_lights doesn't get optimized out
    colour += vec4(num_point_lights*0.0, 0.0, 0.0, 0.0); 

	// Colour of raw texture/ damage effect 
	vec4 initialColour = entity_colour * texture(sampler0, vec2(texcoord.x, texcoord.y));
	// ambient light
	vec4 ambient = vec4(ambient_light * initialColour.rgb, initialColour.a);
    
    colour = ambient;
	// point lights 
	for (int i = 0; i < num_point_lights; i++) {
        colour += CalcPointLight(pointLights[i], worldPos) * initialColour; 
    }
}

vec4 CalcPointLight(PointLight light, vec3 worldPos)
{
    // attenuation
    float distance = distance(light.position, worldPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));  
    // combine results
    vec4 ambient  = light.ambient;
    ambient  *= attenuation;

    return (ambient);
} 

