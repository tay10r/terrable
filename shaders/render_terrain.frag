#version 330 core

in vec2 frag_texture_coordinates;

out vec4 frag_color;

uniform sampler2D terrain_color;

void
main()
{
  frag_color = texture(terrain_color, frag_texture_coordinates);
}
