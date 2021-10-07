#version 330 core

uniform mat4 model = mat4(1.0);

uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec2 position;

uniform sampler2D elevation;

uniform vec3 camera_position = vec3(0.0, 0.0, 0.0);

uniform vec3 light_direction = normalize(vec3(-1.0, -1.0, 0.0));

out vec2 tex_coords;

out vec3 ts_light_direction;

vec2
uv_to_ndc(vec2 uv)
{
  return vec2((uv.x * 2.0) - 1.0, (uv.y * 2.0) - 1.0);
}

mat3
compute_world_to_tangent_transform()
{
  // 2 | a b c
  // 1 | d x e
  // 0 | f g h
  //    ------
  //     0 1 2

  vec2 elevation_texture_size = textureSize(elevation, 0);

  vec2 pixel_size = vec2(1.0 / elevation_texture_size.x,
                         1.0 / elevation_texture_size.y);

  vec2 uv[9];

  // a b c
  uv[0] = position + vec2(-pixel_size.x, -pixel_size.y);
  uv[1] = position + vec2(            0, -pixel_size.y);
  uv[2] = position + vec2( pixel_size.x, -pixel_size.y);

  // d x e
  uv[3] = position + vec2(-pixel_size.x, 0);
  uv[4] = position + vec2(            0, 0);
  uv[5] = position + vec2( pixel_size.x, 0);

  // f g h
  uv[6] = position + vec2(-pixel_size.x, pixel_size.y);
  uv[7] = position + vec2(            0, pixel_size.y);
  uv[8] = position + vec2( pixel_size.x, pixel_size.y);

  vec3 p[9];

  for (int i = 0; i < 9; i++) {

    float y = texture(elevation, uv[i]).r;

    vec2 ndc = uv_to_ndc(uv[i]);

    p[i] = vec3(ndc.x, texture(elevation, uv[i]).r, ndc.y);
  }

  vec3 center = p[4];

  vec3 edges[9];

  // a b c
  edges[0] = p[0] - center;
  edges[1] = p[1] - center;
  edges[2] = p[2] - center;

  // e
  edges[3] = p[5] - center;

  // h g f
  edges[4] = p[8] - center;
  edges[5] = p[7] - center;
  edges[6] = p[6] - center;

  // d
  edges[7] = p[3] - center;

  // a
  edges[8] = edges[0];

  // TODO : reject angles above a certain threshold angle.

  int accepted_normal_count = 0;

  int accepted_bitangent_count = 0;

  vec3 normal_sum = vec3(0, 0, 0);

  vec3 bitangent_sum = vec3(0, 0, 0);

  for (int i = 0; i < 8; i++) {

    vec3 normal = normalize(cross(edges[i], edges[i + 1]));

    if (i == 1) {
      bitangent_sum += normalize(edges[i]);
      accepted_bitangent_count++;
    } else if (i == 5) {
      bitangent_sum += normalize(-edges[i]);
      accepted_bitangent_count++;
    }

    normal_sum += normal;

    accepted_normal_count++;
  }

  vec3 normal = normalize(normal_sum / accepted_normal_count);

  vec3 bitangent = normalize(bitangent_sum / accepted_bitangent_count);

  vec3 tangent = normalize(cross(normal, bitangent));

  // To world space

  normal = normalize((model * vec4(normal, 0.0)).xyz);

  bitangent = normalize((model * vec4(bitangent, 0.0)).xyz);

  tangent = normalize((model * vec4(tangent, 0.0)).xyz);

  return transpose(mat3(tangent, bitangent, normal));
}

void
main()
{
  tex_coords = position;

  mat3 world_to_tangent = compute_world_to_tangent_transform();

  float ndc_x = (position.x * 2.0) - 1.0;
  float ndc_z = (position.y * 2.0) - 1.0;
  float ndc_y = texture(elevation, position).r;
  vec4 p = vec4(ndc_x, ndc_y, ndc_z, 1.0);

  if (camera_position.x == 0.4242424)
    p.x += 0.0001;

  ts_light_direction = world_to_tangent * normalize(light_direction);

  gl_Position = mvp * p;
}
