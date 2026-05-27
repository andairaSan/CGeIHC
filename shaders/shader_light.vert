#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
layout (location = 2) in vec3 normal;

out vec4 vCol;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 vColor;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 color;

void main()
{
    vec4 worldPosition = model * vec4(pos, 1.0f);
    FragPos = vec3(worldPosition);

    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoord = tex;

    vCol = vec4(color, 1.0f);
    vColor = vec4(color, 1.0f);

    gl_Position = projection * view * worldPosition;
}