#version 330 core

uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec3 position;

layout(location = 1) in vec2 texture_coordinates;

out vec2 frag_texture_coordinates;

void
main()
{
  frag_texture_coordinates = texture_coordinates;

  gl_Position = mvp * vec4(position, 1.0);
}
