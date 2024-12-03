#version 330

struct PointLight {
    vec3 position;  
  
    vec4 ambient;
    vec4 diffuse;
	
    float constant;
    float linear;
    float quadratic;
};

// From vertex shader
in vec2 texcoord;
in vec3 worldPos;

// Application data
uniform sampler2D sampler_0;
uniform sampler2D normalSampler;

uniform vec4 entity_colour;

// Lighting data
uniform float ambient_light;

// Animation data
 uniform float num_frames; 
 uniform float current_frame;

// Point lights data
#define MAX_POINT_LIGHTS 3
uniform int num_point_lights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

// Output colour
layout(location = 0) out  vec4 colour;

// Function declerations
vec4 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal);

void main()
{
	float frame_width = 1.0 / num_frames;
	vec2 frame_texcoord = vec2(texcoord.x * frame_width + current_frame*frame_width, texcoord.y);

    // Get normal in range [-1, 1]
    vec3 normal = texture(normalSampler, vec2(frame_texcoord.x, frame_texcoord.y)).rbg;
    normal = normalize(normal * 2.0 - 1.0);  

	// Colour of raw texture/ damage effect 
	vec4 initialColour = entity_colour * texture(sampler_0, vec2(frame_texcoord.x, frame_texcoord.y));
	// ambient light
	vec4 ambient = vec4(ambient_light * initialColour.rgb, initialColour.a);
	colour = ambient;

	// point lights 
	for (int i = 0; i < num_point_lights; i++) {
        colour += CalcPointLight(pointLights[i], worldPos, normal) * initialColour; 
    }
}


vec4 CalcPointLight(PointLight light, vec3 worldPos, vec3 normal)
{
    // Ambient light
    vec4 ambient  = light.ambient;
    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - worldPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * light.diffuse;
    // attenuation
    float d = distance(light.position, worldPos);
    float attenuation = 1.0 / (light.constant + light.linear * d + 
  			     light.quadratic * (d * d));  
    
    ambient  *= attenuation;
    diffuse  *= attenuation;

    // combine results
    return (ambient + diffuse);
} 