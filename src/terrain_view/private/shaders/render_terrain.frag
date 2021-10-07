#version 330 core

in vec2 tex_coords;

in vec3 ts_light_direction;

out vec4 frag_color;

uniform sampler2D splat_map;

uniform vec2 texture_sizes[4] = vec2[](vec2(0.25, 0.25), vec2(0.25, 0.25), vec2(0.25, 0.25), vec2(0.25, 0.25));

uniform sampler2D albedo_and_roughness[4];

uniform sampler2D normal_and_bump[4];

vec3
rgb_to_normal(vec3 rgb_normal)
{
  return (rgb_normal * 2.0) - 1.0;
}

float
get_light_attenuation(vec3 ts_normal)
{
  float attenuation = dot(ts_normal, normalize(ts_light_direction));

  float max_attenuation = 0.4;

  return clamp(attenuation, max_attenuation, 1.0);
}

vec2 to_tile_tex_coords(int idx)
{
    return vec2(mod(tex_coords.x, texture_sizes[idx].x) / texture_sizes[idx].x,
                mod(tex_coords.y, texture_sizes[idx].y) / texture_sizes[idx].y);
}

void
main()
{
  vec4 splat_texel = texture(splat_map, tex_coords);

  float splat[4] = float[4](splat_texel.x, splat_texel.y, splat_texel.z, splat_texel.w);

  vec2 tile_tex_coords[4] = vec2[](to_tile_tex_coords(0),
                                   to_tile_tex_coords(1),
                                   to_tile_tex_coords(2),
                                   to_tile_tex_coords(3));

  vec3 normals[4] = vec3[](rgb_to_normal(texture(normal_and_bump[0], tile_tex_coords[0]).rgb),
                           rgb_to_normal(texture(normal_and_bump[1], tile_tex_coords[1]).rgb),
                           rgb_to_normal(texture(normal_and_bump[2], tile_tex_coords[2]).rgb),
                           rgb_to_normal(texture(normal_and_bump[3], tile_tex_coords[3]).rgb));

  vec3 albedo[4] = vec3[](texture(albedo_and_roughness[0], tile_tex_coords[0]).rgb,
                          texture(albedo_and_roughness[1], tile_tex_coords[1]).rgb,
                          texture(albedo_and_roughness[2], tile_tex_coords[2]).rgb,
                          texture(albedo_and_roughness[3], tile_tex_coords[3]).rgb);

  vec3 total_color = vec3(0.0, 0.0, 0.0);

  for (int i = 0; i < 4; i++) {

    vec3 color = albedo[i];

    total_color += color * splat[i] * get_light_attenuation(normals[i]);
  }

  frag_color = vec4(total_color, 1.0);
}
