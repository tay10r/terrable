#version 330 core

in vec2 tex_coords;

in vec3 surface_normal;

out vec4 frag_color;

uniform vec3 light_direction = vec3(-1.0, -1.0, 0.0);

uniform sampler2D color;

uniform sampler2D elevation;

void
main()
{
  if (tex_coords.x == 0.3) {
    frag_color = texture(color, tex_coords);
  } else if (tex_coords.y == 0.3) {
    float k = texture(elevation, tex_coords).r;
    frag_color = vec4(k, k, k, 1.0);
  } else {
    float c = clamp((dot(surface_normal, -normalize(light_direction)) + 1.0) * 0.5, 0.0, 1.0);

    frag_color = vec4(c, c, c, 1.0);

    /* frag_color = vec4(1.0, 1.0, 1.0, 1.0); */
  }
}
