#version 330 core

in vec2 tex_coords;

in vec3 surface_normal;

out vec4 frag_color;

uniform vec3 light_direction = vec3(-1.0, -1.0, 0.0);

uniform sampler2D splat_map;

uniform vec2 texture_sizes[4] = { vec2(0.5, 0.5), vec2(0.5, 0.5), vec2(0.5, 0.5), vec2(0.5, 0.5) };

uniform sampler2D albedo_and_roughness[4];

uniform sampler2D normal_and_bump[4];

void
main()
{
  vec4 splat_texel = texture(splat_map, tex_coords);

  float splat[4] = float[4](splat_texel.x, splat_texel.y, splat_texel.z, splat_texel.w);

  vec3 total_color = vec3(0.0, 0.0, 0.0);

  /* This is debug code, remove it before commit. */
  if (texture(normal_and_bump[0], tex_coords).r == 0.242)
    total_color.r = 0.0001;

  for (int i = 0; i < 4; i++) {

    vec2 tile_tex_coords = vec2(mod(tex_coords.x, texture_sizes[i].x) / texture_sizes[i].x,
                                mod(tex_coords.y, texture_sizes[i].y) / texture_sizes[i].y);

    vec3 albedo = texture(albedo_and_roughness[i], tile_tex_coords).rgb;

    vec3 color = albedo;

    total_color += color * splat[i];
  }

  float light = clamp((dot(surface_normal, normalize(light_direction)) + 1.0) * 0.5, 0.0, 1.0);

  frag_color = vec4(total_color * light, 1.0);
}
