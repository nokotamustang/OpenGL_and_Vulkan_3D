#version 460 core

layout (location = 0) out vec4 fragColor;

in vec2 uv_0;
in vec3 normal;
in vec3 fragPos;
in vec4 shadow_coord;

struct Light {
  vec3 position;
  vec3 direction;
  vec3 color;
  float strength;
};

struct PointLight {
  vec3 position;
  vec3 color;
  float strength;
};

struct SpotLight {
  vec3 position;
  vec3 color;
  float strength;
  vec3 direction;
  float cutoff;
  float softness;
};

struct Material {
  vec3 a;
  float d;
  float s;
};

const int max_lights = 99;

// uniform vec2 u_resolution;
uniform vec3 cam_pos;
uniform PointLight lights[max_lights];
uniform float num_lights;

uniform Light global_light;
uniform SpotLight flash_light;

uniform float texture_blend;
uniform float local_light_blend;
uniform Material material;
uniform sampler2D u_texture_0;
uniform sampler2DShadow shadow_map_tex;

const vec3 gamma = vec3(2.2);
const vec3 i_gamma = vec3(1 / 2.2);

// const vec3 fog_albedo = vec3(0.333);
// const float flog_Scale = 0.15 / 10; // Higher is stronger rescale [0.0 to 1.0] to [0.0 to 0.1] i.e 0.015;

vec3 directional_light(vec3 V, vec3 N, Light light) {
  // Direction vector
  const vec3 D = normalize(light.position - light.direction);

  // Shadow
  const float shadow = mix(textureProj(shadow_map_tex, shadow_coord), 1.0, 1.0 - step(1.0, shadow_coord.z));

  // Radiance for directional lights is the color of the light times its strength
  const vec3 radiance = light.color * light.strength;

  // Ambient
  const vec3 ambient = material.a;

  // Diffuse (Lambertian)
  const float diff = max(dot(N, D), 0.0);
  const float diffuse = material.d * diff;

  // Specular (Blinn-Phong)
  const vec3 R = reflect(-D, N);
  const float spec = pow(max(dot(V, R), 0.0), 32);
  const float specular = material.s * spec;

  // Composition
  return ambient * (diffuse + specular) * shadow * radiance;
}

vec3 point_light(vec3 V, vec3 N, PointLight light) {
  // Direction vector
  const vec3 D = normalize(light.position - fragPos);

  // Attenuation
  const float distance = length(light.position - fragPos);
  const float attenuation = light.strength / distance;  

  // Radiance is the product of the color and the attenuation
  const vec3 radiance = light.color * attenuation;

  // Ambient
  const vec3 ambient = material.a;

  // Diffuse (Lambertian)
  const float diff = max(dot(N, D), 0.0);
  const float diffuse = material.d * diff;

  // Specular (Blinn-Phong)
  const vec3 R = reflect(-D, N);
  const float spec = pow(max(dot(V, R), 0.0), 32);
  const float specular = material.s * spec;

  // Composition
  return ambient * (diffuse + specular) * radiance;
}

vec3 spot_light(vec3 V, vec3 N, SpotLight light) {
  // Direction vector
  const vec3 D = normalize(light.position - fragPos);

  // Ambient
  const vec3 ambient = material.a;

  // Cutoff angle for spot light
  const float theta = dot(D, -light.direction);
  const float epsilon = light.cutoff - light.softness;
  // const float intensity = clamp((theta - light.softness) / epsilon, 0.0, 1.0);
  const float intensity = smoothstep(0.0, 1.0, (theta - light.softness) / epsilon);

  // Attenuation
  const float distance = length(light.position - fragPos);
  const float attenuation = light.strength / distance;  

  // Radiance is the product of the color and the attenuation
  const vec3 radiance = light.color * attenuation;

  // Diffuse (Lambertian)
  const float diff = max(dot(N, D), 0.0);
  const float diffuse = material.d * diff;

  // Specular (Blinn-Phong)
  const vec3 R = reflect(-D, N);
  const float spec = pow(max(dot(V, R), 0.0), 32);
  const float specular = material.s * spec;

  // Composition
  return ambient * (diffuse + specular) * intensity * radiance;
}

vec3 light_colors(vec3 tex_color) {
  const vec3 N = normalize(normal);
  const vec3 V = normalize(cam_pos - fragPos);

  // Directional lights such as the global light
  vec3 Lo = directional_light(V, N, global_light);

  // Point lights such as local lights
  if (local_light_blend > 0.0) {
    for (int i = 0; i < max_lights; i++) {
      Lo += point_light(V, N, lights[i]);
      if (i == num_lights) {
        break;
      }
    }
  }

  // Spot light such as camera positioned flash light
  Lo += spot_light(V, N, flash_light);

  // Blend texture color with the combined illumination (if 0 there is none)
  return Lo * mix(vec3(1.0), tex_color, texture_blend);
}

void main() {
  vec3 color = texture(u_texture_0, uv_0).rgb;
  color = pow(color, gamma);
  color = light_colors(color);

  // Fog
  // const float fog = gl_FragCoord.z / gl_FragCoord.w; // Strength higher when far away from frag
  // color = mix(color, fog_albedo, (1.0 - exp2(-flog_Scale * fog)));

  color = pow(color, i_gamma);
  fragColor = vec4(color, 1.0);
}