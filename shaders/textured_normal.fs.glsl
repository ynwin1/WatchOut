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
uniform sampler2D sampler0;
uniform sampler2D normalSampler;

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
vec4 CalcPointLight(PointLight light, vec3 fragPos, vec3 normal, vec4 initialColour);

void main()
{
	// Colour of raw texture/ damage effect 
	vec4 initialColour = entity_colour * texture(sampler0, vec2(texcoord.x, texcoord.y));
    // Get normal in range [-1, 1]
    vec3 normal = texture(normalSampler, vec2(texcoord.x, texcoord.y)).rbg;
    normal = normalize(normal * 2.0 - 1.0);  
	// ambient light
	vec4 ambient = vec4(ambient_light * initialColour.rgb, initialColour.a);
    
    colour = ambient;
	// point lights 
	for (int i = 0; i < num_point_lights; i++) {
        colour.rgb += CalcPointLight(pointLights[i], worldPos, normal, initialColour).rgb; 
    }
}

vec4 CalcPointLight(PointLight light, vec3 worldPos, vec3 normal, vec4 initialColour)
{
    // Ambient light
    vec4 ambient  = light.ambient * initialColour;
    // Diffuse light
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - worldPos); 
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * light.diffuse * initialColour;
    // attenuation
    float d = distance(light.position, worldPos);
    float attenuation = 1.0 / (light.constant + light.linear * d + 
  			     light.quadratic * (d * d));  
    
    ambient  *= attenuation;
    diffuse  *= attenuation;

    // combine results
    return (ambient + diffuse);
} 

