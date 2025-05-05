#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;  // Texture coordinates passed from the vertex shader

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture1; // The texture sampler

void main()
{
    vec3 lightColor = vec3(1.0);                  // White light
    vec3 objectColor = vec3(1.0, 0.5, 0.0);        // Orange (this can be removed if you only want to use texture)

    // Ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Texture
    vec3 textureColor = texture(texture1, TexCoords).rgb;

    // Combine the results
    vec3 result = (ambient + diffuse) * textureColor; // Use texture color instead of object color
    FragColor = vec4(result, 1.0);
}
