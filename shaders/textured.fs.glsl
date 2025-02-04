#version 330

struct PointLight {
    vec3 position;  
  
    vec4 ambient;
	
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

// Point lights data
#define MAX_POINT_LIGHTS 3
uniform int num_point_lights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// Output colour
layout(location = 0) out  vec4 colour;

// Function declerations
vec4 CalcPointLight(PointLight light, vec3 fragPos);

void main()
{
	// Colour of raw texture/ damage effect 
	vec4 initialColour = entity_colour * texture(sampler0, vec2(texcoord.x, texcoord.y));
	// ambient light
	vec4 ambient = vec4(ambient_light * initialColour.rgb, initialColour.a);
    
    colour = ambient;
	// point lights 
	for (int i = 0; i < num_point_lights; i++) {
        colour.rgb += CalcPointLight(pointLights[i], worldPos).rgb * initialColour.rgb; 
    }
}

vec4 CalcPointLight(PointLight light, vec3 worldPos)
{
    // attenuation
    float d = distance(light.position, worldPos);
    float attenuation = 1.0 / (light.constant + light.linear * d + 
  			     light.quadratic * (d * d));  
    // combine results
    vec4 ambient  = light.ambient;
    ambient  *= attenuation;

    return (ambient);
} 

