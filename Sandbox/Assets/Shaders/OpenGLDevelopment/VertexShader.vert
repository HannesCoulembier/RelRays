#version 430

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

layout (location = 0) out vec3 outColor;

layout (set = 0, binding = 0) uniform CameraBuffer {
	mat4 view;
	mat4 proj;
	mat4 invView;
	mat4 invProj;
} cameraData;

void main()
{
	//output the position of each vertex
	gl_Position = cameraData.proj * cameraData.view * vec4(inPos, 1.0f);
	outColor = inColor;
}