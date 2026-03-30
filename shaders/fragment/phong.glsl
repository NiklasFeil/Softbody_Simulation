#version 330 core

in vec3 fragPos;

out vec4 FragColor;

uniform vec3 color;
uniform vec3 viewPos;

void main()
{
    vec3 dx = dFdx(fragPos);
    vec3 dy = dFdy(fragPos);
    vec3 normal = normalize(cross(dx, dy));

    // Ambient
    float ambient = 0.1;
    //vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = vec3(0.19245009, 0.962250448, 0.19245009); // Towards sun/spotlight
    float diffuse = max(dot(normal, lightDir), 0.0);
    //vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, normal);

    float shininess = 32.0;
    vec3 viewDir = normalize(viewPos - fragPos);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    float specular = specularStrength * spec; // * lightColor;

    vec3 result = (ambient + diffuse + specular) * color;
    FragColor = vec4(result, 1.0);
}