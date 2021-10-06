#version 330 core

in vec2 tex_coords;

in vec3 surface_normal;

out vec4 frag_color;

uniform vec3 light_direction = vec3(-1.0, -1.0, 0.0);

uniform sampler2D albedo_and_roughness;

void
main()
{
  vec3 albedo = texture(albedo_and_roughness, tex_coords).rgb;

  float light = clamp((dot(surface_normal, -normalize(light_direction)) + 1.0) * 0.5, 0.0, 1.0);

  frag_color = vec4(albedo * light, 1.0);
}
