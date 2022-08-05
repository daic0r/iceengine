#version 430

const int MAX_INSTANCES = 5000;
const int MAX_JOINTS = 50;

in vec3 vertexPos;
in vec2 texCoord;
in vec3 normal;
in mat4 worldTransform;
in ivec4 jointIds;
in vec4 jointWeights;

out vec2 coord;
out vec3 toLight;
out vec3 outNormal;
out vec2 shadowMapCoord;
out float distToLight;
out float distFromCam;

// Model Vertex Shader stuff
uniform mat4 projection;
uniform mat4 view;
uniform vec3 sunPosition;
// Animated Model stuff
uniform samplerBuffer jointTransforms_TBO;

layout(std140) uniform CommonMatrices
{
	mat4 shadowProjViewMatrix;
	float shadowDistance;
	float shadowMargin;
	float shadowTexelSize;
};

void main(void) {
    coord = texCoord;
	
    vec4 totalPos = vec4( 0.0 );
    //vec4 totalNormal( 0.0f );
    for (int i = 0; i < 4; ++i) {
		mat4 jointTransform = mat4(
			texelFetch(jointTransforms_TBO, gl_InstanceID * MAX_JOINTS * 4 + jointIds[i] * 4 + 0),
			texelFetch(jointTransforms_TBO, gl_InstanceID * MAX_JOINTS * 4 + jointIds[i] * 4 + 1),
			texelFetch(jointTransforms_TBO, gl_InstanceID * MAX_JOINTS * 4 + jointIds[i] * 4 + 2),
			texelFetch(jointTransforms_TBO, gl_InstanceID * MAX_JOINTS * 4 + jointIds[i] * 4 + 3)
			);
        totalPos += (jointTransform * vec4(vertexPos, 1.0)) * jointWeights[i];
        //totalNormal += (jointTransform * vec4(normal, 0.0f)) * jointWeights[i];
    }
    
	vec4 worldPosition = worldTransform * totalPos;
    gl_Position = projection * view * worldPosition;
	
	toLight = normalize(sunPosition - worldPosition.xyz);
	outNormal = normalize((worldTransform * vec4(normal, 0.0)).xyz);
	
	vec3 shadowMapVec = 0.5 + 0.5 * (shadowProjViewMatrix * worldPosition).xyz;
	shadowMapCoord = shadowMapVec.xy;
	distToLight = shadowMapVec.z;
	distFromCam = length((view * worldPosition).xyz);
}
