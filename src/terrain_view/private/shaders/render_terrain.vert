#version 330 core

uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec2 position;

uniform vec2 pixel_size = vec2(1.0 / 64.0, 1.0 / 64.0);

uniform sampler2D elevation;

out vec2 tex_coords;

out vec3 surface_normal;

vec2
uv_to_ndc(vec2 uv)
{
  return vec2((uv.x * 2.0) - 1.0, (uv.y * 2.0) - 1.0);
}

vec3
compute_surface_normal()
{
  vec2 uv0 = position;
  vec2 uv1 = position + vec2(pixel_size.x, 0.0);
  vec2 uv2 = position + vec2(0.0, pixel_size.y);

  float y0 = texture(elevation, uv0).r;
  float y1 = texture(elevation, uv1).r;
  float y2 = texture(elevation, uv2).r;

  vec2 ndc0 = uv_to_ndc(uv0);
  vec2 ndc1 = uv_to_ndc(uv1);
  vec2 ndc2 = uv_to_ndc(uv2);

  vec3 p0 = vec3(ndc0.x, y0, ndc0.y);
  vec3 p1 = vec3(ndc1.x, y1, ndc1.y);
  vec3 p2 = vec3(ndc2.x, y2, ndc2.y);

  vec3 e0 = p1 - p0;
  vec3 e1 = p2 - p0;

  return normalize(cross(e0, e1));
}

void
main()
{
  tex_coords = position;

  surface_normal = compute_surface_normal();

  float ndc_x = (position.x * 2.0) - 1.0;
  float ndc_z = (position.y * 2.0) - 1.0;

  float ndc_y = texture(elevation, position).r;

  vec3 p = vec3(ndc_x, ndc_y, ndc_z);

  gl_Position = mvp * vec4(p, 1.0);
}
