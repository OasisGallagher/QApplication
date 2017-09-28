#shader vertex
in vec3 c_position;
in vec2 c_texCoord;
in vec3 c_normal;

out vec2 texCoord;
out vec3 normal;
out vec3 worldPos;

uniform mat4 c_localToClipSpaceMatrix;
uniform mat4 c_localToWorldSpaceMatrix;

void main() {
	texCoord = c_texCoord;

	normal = (c_localToWorldSpaceMatrix * vec4(c_normal, 0)).xyz;
	worldPos = (c_localToWorldSpaceMatrix * vec4(c_position, 1)).xyz;

	gl_Position = c_localToClipSpaceMatrix * vec4(c_position, 1);
}

#shader fragment
out vec3 c_fragColor;

in vec2 texCoord;
in vec3 normal;
in vec3 worldPos;

uniform sampler2D c_mainTexture;

uniform vec3 c_ambientLightColor;
uniform vec3 c_cameraPosition;
uniform vec3 c_lightColor;
uniform vec3 c_lightDirection;

void main() {
	const float gloss = 3;
	vec3 normal0 = normalize(normal);
	vec3 albedo = texture(c_mainTexture, texCoord).rgb;

	vec3 ambient = c_ambientLightColor * albedo;
	
	float factor = clamp(dot(normal0, -c_lightDirection), 0, 1);
	vec3 diffuse = c_lightColor * factor * albedo;

	vec3 toEye = normalize(c_cameraPosition - worldPos);
	vec3 reflectDir = normalize(reflect(c_lightDirection, normal0));
	factor = clamp(dot(toEye, reflectDir), 0, 1);
	factor = pow(factor, gloss);
	vec3 specular = c_lightColor * factor * albedo;
	
	c_fragColor = ambient + diffuse + specular;
}
