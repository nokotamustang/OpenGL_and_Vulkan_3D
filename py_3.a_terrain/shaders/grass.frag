#version 460 core

layout (location = 0) out vec4 fragColor;

in GS_OUT {
  vec2 uv_0;
  float color_variation;
  vec3 normal;
  vec3 fragPos;
  vec4 shadow_coord;
} fs_in;

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
  vec3 direction;
  vec3 color;
  float strength;
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

const float PI = 3.14159265359;
const vec3 gamma = vec3(2.2);
const vec3 i_gamma = vec3(1 / 2.2);

const float alpha_discard_level = 0.25; // 0.75 for higher res grass

// const vec3 fog_albedo = vec3(0.333);
// const float flog_Scale = 0.15 / 10; // Higher is stronger rescale [0.0 to 1.0] to [0.0 to 0.1] i.e 0.015;

/* Percentage-closer filtering, softens the shadow edges */
// float lookup(float ox, float oy) {
//   vec2 pixelOffset = 1 / u_resolution;
//   return textureProj(shadow_map_tex, shadow_coord + vec4(ox * pixelOffset.x * shadow_coord.w, oy * pixelOffset.y * shadow_coord.w, 0.0, 0.0));
// }
// float get_shadow_pcf_4() {
//   float shadow;
//   const float spread = 1.5;  // shadow spread
//   const vec2 offset = mod(floor(gl_FragCoord.xy), 2.0) * spread;
//   shadow += lookup(-1.5 * spread + offset.x, 1.5 * spread - offset.y);
//   shadow += lookup(-1.5 * spread + offset.x, -0.5 * spread - offset.y);
//   shadow += lookup(0.5 * spread + offset.x, 1.5 * spread - offset.y);
//   shadow += lookup(0.5 * spread + offset.x, -0.5 * spread - offset.y);
//   return shadow * 0.25;
// }
// float get_shadow_pcf_16() {
//   float shadow;
//   const float spread = 1.0;
//   const float end_p = spread * 1.5;
//   for (float y = -end_p; y <= end_p; y += spread) {
//     for (float x = -end_p; x <= end_p; x += spread) {
//       shadow += lookup(x, y);
//     }
//   }
//   return shadow * 0.0625;
// }

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness) {
  const float a2 = pow(roughness, 4.0);
  const float n_dot_h = pow(max(dot(N, H), 0.0), 2.0);
  return a2 / (pow(n_dot_h * (a2 - 1.0) + 1.0, 2.0) * PI);
}
float GeometrySchlickGGX(float n_dot_v, float roughness) {
  const float k = pow(roughness + 1.0, 2.0) / 8.0;
  return n_dot_v / (n_dot_v * (1.0 - k) + k);
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  const float ggx2 = GeometrySchlickGGX(max(dot(N, V), 0.0), roughness);
  const float ggx1 = GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
  return ggx1 * ggx2;
}

vec3 directional_light(vec3 N, vec3 V, Light light, vec3 F0) {
  // Direction vector
  const vec3 D = normalize(light.position - light.direction);
  const vec3 H = normalize(V + D);

  // Shadow
  // Force shadow off if z is outside the far plane of the frustum
  const float shadow = mix(textureProj(shadow_map_tex, fs_in.shadow_coord), 1.0, 1.0 - step(1.0, fs_in.shadow_coord.z));
  // ... equivalent of: 
  // float shadow = textureProj(shadow_map_tex, fs_in.shadow_coord);
  // if (fs_in.shadow_coord.z < 0.0) {
  //   shadow = 1.0;
  // }

  // Radiance for directional lights is the color of the light times its strength
  const vec3 radiance = light.color * light.strength;

  // Calculate normal distribution for specular brdf.
  const float NDF = DistributionGGX(N, H, material.d);

  // Calculate geometric attenuation for specular brdf.
  const float G = GeometrySmith(N, V, D, material.d);

  // Calculate Fresnel term for direct lighting. 
  const vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

  // Diffuse scattering
  const vec3 kD = (vec3(1.0) - F) * (1.0 - material.s); 

  // Cook-torrance brdf
  const float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, D), 0.0) + 0.0001;
  const vec3 specular = (NDF * G * F) / denominator;
  const float n_dot_l = max(dot(N, D), 0.0);

  // Composition
  return ((kD * material.a / PI + specular) * shadow) * radiance * n_dot_l;
}

vec3 point_light(vec3 N, vec3 V, PointLight light, vec3 F0) {
  // Direction vector
  const vec3 D = normalize(light.position - fs_in.fragPos);
  const vec3 H = normalize(V + D);

  // Attenuation
  const float distance = length(light.position - fs_in.fragPos);
  const float strength = light.strength;
  const float attenuation = light.strength / distance; // Basic attenuation for now, usually this would be / pow(distance, 2.0)
  // More complex attenuation formula that uses a linear and quadratic term from the light; and the strength is the constant.
  // const float light_quadratic = 0.09;
  // const float light_linear = 0.032;
  // const float attenuation = 1.0 / (light.strength + light_linear * distance + light_quadratic * pow(distance, 2.0));  

  // Radiance is the product of the color and the attenuation
  const vec3 radiance = light.color * attenuation * strength;

  // Calculate normal distribution for specular brdf.
  const float NDF = DistributionGGX(N, H, material.d);

  // Calculate geometric attenuation for specular brdf.
  const float G = GeometrySmith(N, V, D, material.d);

  // Calculate Fresnel term for direct lighting. 
  const vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

  // Diffuse scattering
  const vec3 kD = (vec3(1.0) - F) * (1.0 - material.s);
  const vec3 numerator = NDF * G * F;

  // Cook-torrance brdf
  const float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, D), 0.0) + 0.0001;
  const vec3 specular = numerator / denominator;
  const float n_dot_l = max(dot(N, D), 0.0);

  // Composition
  return ((kD * material.a / PI + specular)) * radiance * n_dot_l;
}

vec3 spot_light(vec3 N, vec3 V, SpotLight light, vec3 F0) {
  // Direction vector
  const vec3 D = normalize(light.position - fs_in.fragPos);
  const vec3 H = normalize(V + D);

  // Cutoff angle for spot light
  const float theta = dot(D, -light.direction);
  const float epsilon = light.cutoff - light.softness;
  // const float intensity = clamp((theta - light.softness) / epsilon, 0.0, 1.0);
  const float intensity = smoothstep(0.0, 1.0, (theta - light.softness) / epsilon);

  // Attenuation
  const float distance = length(light.position - fs_in.fragPos);
  const float strength = light.strength;
  const float attenuation = light.strength / distance; // Basic attenuation for now, usually this would be / pow(distance, 2.0)
  // More complex attenuation formula that uses a linear and quadratic term from the light; and the strength is the constant.
  // const float light_quadratic = 0.09;
  // const float light_linear = 0.032;
  // const float attenuation = 1.0 / (light.strength + light_linear * distance + light_quadratic * pow(distance, 2.0));  

  // Radiance is the product of the color and the attenuation
  const vec3 radiance = light.color * attenuation * strength;

  // Calculate normal distribution for specular brdf.
  const float NDF = DistributionGGX(N, H, material.d);

  // Calculate geometric attenuation for specular brdf.
  const float G = GeometrySmith(N, V, D, material.d);

  // Calculate Fresnel term for direct lighting. 
  const vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

  // Diffuse scattering
  const vec3 kD = (vec3(1.0) - F) * (1.0 - material.s);
  const vec3 numerator = NDF * G * F;

  // Cook-torrance brdf
  const float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, D), 0.0) + 0.0001;
  const vec3 specular = numerator / denominator;
  const float n_dot_l = max(dot(N, D), 0.0);

  // Composition
  return ((kD * material.a / PI + specular)) * intensity * radiance * n_dot_l;
}

vec3 light_colors(vec3 tex_color) {
  const vec3 N = normalize(fs_in.normal);
  const vec3 V = normalize(cam_pos - fs_in.fragPos);

  // Precompute the surface response at normal incidence
  const vec3 F0 = mix(vec3(0.04), material.a, material.s);

  // Directional lights
  vec3 Lo = directional_light(N, V, global_light, F0);

  if (local_light_blend > 0.0) {
    for (int i = 0; i < max_lights; i++) {
      Lo += point_light(N, V, lights[i], F0);
      if (i == num_lights) {
        break;
      }
    }
  }

  // Spot light such as camera positioned flash light
  Lo += spot_light(N, V, flash_light, F0);

  // Blend texture color with the combined illumination (if 0 there is none)
  return Lo * mix(vec3(1.0), tex_color, texture_blend);
}

void main() {
  const vec4 color_full = texture(u_texture_0, fs_in.uv_0);
  // Crop out alpha areas that are weak
  if (color_full.a < alpha_discard_level) {
    discard;
  }

  vec3 color = color_full.rgb;
  color = pow(color, gamma);
  color = light_colors(color);

  // Fog
  // const float fog = gl_FragCoord.z / gl_FragCoord.w; // Strength higher when far away from frag
  // color = mix(color, fog_albedo, (1.0 - exp2(-flog_Scale * fog)));

  // Add color variation
  color.xyz = mix(color.rgb, 0.5 * color.rgb, fs_in.color_variation);

  color = pow(color, i_gamma);
  fragColor = vec4(color, 1.0);
}