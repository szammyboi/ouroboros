#version 330 core

out vec4 FragColor;
  
in vec4 vertexColor;
in vec3 vertexPos;
flat in float vertexCount;
flat in int lodOut;

void main()
{
	vec3 norm = normalize(vertexPos);
	vec3 base = vertexPos * 0.5 + 0.5;

	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); 
	float diff = max(dot(normalize(norm), lightDir), 0.0);
	vec3 color = base * diff;
	color += base * 0.2;

	vec3 c;
	if (lodOut == 0) c = vec3(1.0, 0.0, 0.0);
	else if (lodOut == 1) c = vec3(1.0, 0.5, 0.0);
	else if (lodOut == 2) c = vec3(1.0, 1.0, 0.0);
	else if (lodOut == 3) c = vec3(0.0, 1.0, 0.0);
	else if (lodOut == 4) c = vec3(0.0, 0.0, 1.0);
	else c = vec3(0.5, 0.0, 0.5);
    FragColor = vec4(c, 1.0);
} 