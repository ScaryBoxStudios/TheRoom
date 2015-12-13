#version 330

in vec3 Normal;
in vec3 Color;
in vec2 UVCoords;
in vec3 FragPos;

out vec4 color;

uniform sampler2D tex;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main(void)
{
    float ambientStrength = 0.3f;
    vec3 lightColor = vec3(1.0f);
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular);
    color = texture(tex, UVCoords) * vec4(result, 1.0f);
}
