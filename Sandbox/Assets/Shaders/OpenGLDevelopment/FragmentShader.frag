#version 430

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

layout (binding = 2) uniform sampler2D tex1;
layout (binding = 3) uniform sampler2D tex2;

void main()
{
	vec4 texColor = texture(tex1, inUV).xyzw;
	vec4 color = vec4(texColor.xyz*inColor, texColor.w);
	outFragColor = vec4(color);
}