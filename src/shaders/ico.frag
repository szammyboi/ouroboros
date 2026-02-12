#version 330 core

out vec4 FragColor;
  
in vec4 vertexColor;
in vec3 vertexPos;

void main()
{
	vec3 norm = normalize(vertexPos);
	vec3 base = vertexPos * 0.5 + 0.5;

	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0)); // light coming from top-front-right
	float diff = max(dot(normalize(norm), lightDir), 0.0);
	vec3 color = base * diff;
	color += base * 0.2;

    FragColor = vertexColor;
} 