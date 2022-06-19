#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;
out vec2 uv;

void main()
{
   gl_Position = projection * view * model * vec4(aPos, 1.0);
   Normal = mat3(transpose(inverse(model))) * aNormal; 
   FragPos = vec3(model * vec4(aPos, 1.0));
   
   //uv is for computing the checkboard pattern, range [0,1]
   uv = (aPos.xz + vec2(2.0)) / vec2(4.0); 
}