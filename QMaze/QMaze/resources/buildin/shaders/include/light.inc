uniform vec3 c_ambientLightColor;
uniform vec3 c_cameraPosition;
uniform vec3 c_lightColor;
uniform vec3 c_lightDirection;

vec3 calculateDirectionalLight(vec3 worldPos, vec3 worldNormal) {
	const float gloss = 20;

	vec3 ambient = vec3(c_ambientLightColor);

	float factor = clamp(dot(worldNormal, -c_lightDirection), 0, 1);
	vec3 diffuse = vec3(c_lightColor * factor);

	vec3 toEye = normalize(c_cameraPosition - worldPos);
	vec3 reflectDir = normalize(reflect(c_lightDirection, worldNormal));
	factor = clamp(dot(toEye, reflectDir), 0, 1);
	factor = pow(factor, gloss);
	vec3 specular = vec3(c_lightColor * factor);

	return ambient + diffuse + specular;
}
