#version 330 core

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec4 material;
uniform vec3 color;

in vec3 fsPosition;
in vec3 fsNormal;

out vec4 outColor;

void main()
{
   vec3 toCamera = normalize(cameraPosition - fsPosition);
   vec3 toLight = normalize(lightPosition - fsPosition);
   vec3 normal = normalize(fsNormal);

   float ambient = material.x;
   float diffuse = material.y * max(dot(toLight, normal), 0.f);
   float specular = material.z * pow(max(dot(reflect(-toLight, normal), toCamera), 0.f), material.w);

   outColor = vec4(clamp((ambient + diffuse + specular) * color, 0.f, 1.f), 1.f);
}
