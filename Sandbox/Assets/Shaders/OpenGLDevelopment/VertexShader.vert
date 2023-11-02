#version 430

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUV;

layout (binding = 0) uniform CameraBuffer {
	mat4 view;
	mat4 proj;
	mat4 invView;
	mat4 invProj;
} cameraData;

layout (std140, binding = 1) readonly buffer ObjectTransformsBuffer {
	mat4 transforms[];
} objectTransforms;

void main()
{
	//output the position of each vertex
	gl_Position = cameraData.proj * cameraData.view * objectTransforms.transforms[1] * vec4(inPos, 1.0f);
	// gl_Position = cameraData.proj * cameraData.view * vec4(inPos, 1.0f);
	outColor = inColor;
	outUV = inUV;
}