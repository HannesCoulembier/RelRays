#version 450

layout(push_constant, std430) uniform constants {
    
    layout(offset = 64) int texIndex; // The offset comes from the 64 bit pushconstant in VertexShader.vert
} objectData;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in uint fragTexIndex; // unused

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D textures[2];

void main() {
    vec4 texColor = vec4(fragColor, 1.0);

    if (objectData.texIndex < 2) // To prevent index out of range
        texColor *= texture(textures[int(objectData.texIndex)], fragTexCoord);

    if (texColor.a == 0.0){
        discard;
    }
    outColor = texColor;
}