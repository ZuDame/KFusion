#include "raycast.h"
#include "kf_helper.h"

float3r* p_vertex_ref = NULL;
float3r* p_normal_ref = NULL;
float3r* p_vertex = NULL;
float3r* p_normal = NULL;
unsigned vol_size;
float tsdf[MAX_VOXELS];
float weight[MAX_VOXELS];
config_param config_pm;
matrix4r pose;
pos3r volume_dim;
float3r volume_scale;
float3r volume_scale_inv;
void tsdf_u16_to_float();


inline float T(const unsigned &x, const unsigned &y, const unsigned &z)
{
  return tsdf[(x)+(y)*volume_dim.y + (z)*volume_dim.z];
}

inline void vertex_normal_ref(float3r &v, float3r &n, const unsigned &x, const unsigned &y)
{
  const static unsigned stride = config_pm.width;
  unsigned index = x + y*stride;

  v.x = p_vertex_ref[index].x;
  v.y = p_vertex_ref[index].y;
  v.z = p_vertex_ref[index].z;

  n.x = p_normal_ref[index].x;
  n.y = p_normal_ref[index].y;
  n.z = p_normal_ref[index].z;
}


void raycast_init(const unsigned &frame)
{

  uint16_t* p_tsdf = (uint16_t*)tsdf;
  load_config(config_pm);
  volume_dim.x = config_pm.vol_size;
  volume_dim.y = config_pm.vol_size;
  volume_dim.z = config_pm.vol_size * volume_dim.y;

  volume_scale.x = (float)config_pm.vol_size / config_pm.vol_size_metric;
  volume_scale.y = volume_scale.x;
  volume_scale.z = volume_scale.x;
  inverse(volume_scale_inv, volume_scale);

  load_tsdf(frame, p_tsdf, vol_size);
  tsdf_u16_to_float();

  unsigned rays = config_pm.width * config_pm.height;

  p_vertex_ref = new float3r[rays];
  p_normal_ref = new float3r[rays];

  p_vertex = new float3r[rays];
  p_normal = new float3r[rays];

  load_vertex_normal(frame, (float*)p_vertex_ref, (float*)p_normal_ref, rays);
  float* p_pose[4];
  p_pose[0] = (float*)&pose.r0;
  p_pose[1] = (float*)&pose.r1;
  p_pose[2] = (float*)&pose.r2;
  p_pose[3] = (float*)&pose.r3;

  load_pose(frame, p_pose, false);

}

void raycast_close()
{
  if (p_vertex_ref) delete[] p_vertex_ref;
  if (p_normal_ref) delete[] p_normal_ref;
  if (p_vertex) delete[] p_vertex;
  if (p_normal) delete[] p_normal;
}

void tsdf_u16_to_float()
{
  tsdf_weight* p_tw = (tsdf_weight*)tsdf;
  float* p_tsdf = tsdf;
  float* p_weight = weight;

  const unsigned voxels = vol_size * vol_size * vol_size;
  int16_t tf = 0;
  int16_t wght = 0;

  for (unsigned v = 0; v < voxels; ++v)
  {
    tf = p_tw->tsdf;
    wght = p_tw->weight;
    *p_tsdf++ = 0.00003051944088f * (float)tf;
    *p_weight++ = (float)wght;
    p_tw++;
  }
}

inline float interp(const float3r &pos) {
  float d = 0.0f;
  static const int3r vol_max(config_pm.vol_size - 1), zero;
  int3r lower, upper;

  float3r scaled_pos, half(0.5f), factor;
  mult3_ew(scaled_pos, 1.0f, pos, volume_scale);
  sub3_ew(scaled_pos, scaled_pos, half);

  int3r base(scaled_pos.x, scaled_pos.y, scaled_pos.z);
  int3r base_p1;
  base_p1.x = base.x + 1; base_p1.y = base.y + 1; base_p1.z = base.z + 1;

  fracf3(factor, scaled_pos);

  max3(lower, base, zero);
  min3(upper, base_p1, vol_max);

  d = (((T(lower.x, lower.y, lower.z) * (1.0f - factor.x) +
    T(upper.x, lower.y, lower.z) * factor.x) *
    (1.0 - factor.y) +
    (T(lower.x, upper.y, lower.z) * (1.0f - factor.x) +
      T(upper.x, upper.y, lower.z) * factor.x) *
    factor.y) *
    (1.0f - factor.z) +
    ((T(lower.x, lower.y, upper.z) * (1.0f - factor.x) +
      T(upper.x, lower.y, upper.z) * factor.x) *
      (1.0f - factor.y) +
      (T(lower.x, upper.y, upper.z) * (1.0f - factor.x) +
        T(upper.x, upper.y, upper.z) * factor.x) *
      factor.y) *
    factor.z);

  return d;
}

inline float grad(float3r &gradient, const float3r &pos) {
  static const int3r vol_max(config_pm.vol_size - 1), zero;

  float3r scaled_pos, half(0.5f), factor;
  mult3_ew(scaled_pos, 1.0f, pos, volume_scale);
  sub3_ew(scaled_pos, scaled_pos, half);

  int3r base(scaled_pos.x, scaled_pos.y, scaled_pos.z);
  int3r base_m1, base_p1, base_p2, lower_lower, lower_upper;
  int3r upper_lower, upper_upper;
  add3(base_m1, base, -1);
  add3(base_p1, base, 1);
  add3(base_p2, base, 2);

  max3(lower_lower, base_m1, zero);
  max3(lower_upper, base, zero);
  min3(upper_lower, base_p1, vol_max);
  min3(upper_upper, base_p2, vol_max);

  const int3r &lower = lower_upper;
  const int3r &upper = upper_lower;

  fracf3(factor, scaled_pos);

  gradient.x = (((T(upper_lower.x, lower.y, lower.z) -
    T(lower_lower.x, lower.y, lower.z)) *
    (1.0f - factor.x) +
    (T(upper_upper.x, lower.y, lower.z) -
      T(lower_upper.x, lower.y, lower.z)) *
    factor.x) *
    (1.0f - factor.y) +
    ((T(upper_lower.x, upper.y, lower.z) -
      T(lower_lower.x, upper.y, lower.z)) *
      (1.0f - factor.x) +
      (T(upper_upper.x, upper.y, lower.z) -
        T(lower_upper.x, upper.y, lower.z)) *
      factor.x) *
    factor.y) *
    (1.0f - factor.z) +
    (((T(upper_lower.x, lower.y, upper.z) -
      T(lower_lower.x, lower.y, upper.z)) *
      (1.0f - factor.x) +
      (T(upper_upper.x, lower.y, upper.z) -
        T(lower_upper.x, lower.y, upper.z)) *
      factor.x) *
      (1.0f - factor.y) +
      ((T(upper_lower.x, upper.y, upper.z) -
        T(lower_lower.x, upper.y, upper.z)) *
        (1.0f - factor.x) +
        (T(upper_upper.x, upper.y, upper.z) -
          T(lower_upper.x, upper.y, upper.z)) *
        factor.x) *
      factor.y) *
    factor.z;

  gradient.y = (((T(lower.x, upper_lower.y, lower.z) -
    T(lower.x, lower_lower.y, lower.z)) *
    (1.0f - factor.x) +
    (T(upper.x, upper_lower.y, lower.z) -
      T(upper.x, lower_lower.y, lower.z)) *
    factor.x) *
    (1.0f - factor.y) +
    ((T(lower.x, upper_upper.y, lower.z) -
      T(lower.x, lower_upper.y, lower.z)) *
      (1.0f - factor.x) +
      (T(upper.x, upper_upper.y, lower.z) -
        T(upper.x, lower_upper.y, lower.z)) *
      factor.x) *
    factor.y) *
    (1.0f - factor.z) +
    (((T(lower.x, upper_lower.y, upper.z) -
      T(lower.x, lower_lower.y, upper.z)) *
      (1.0f - factor.x) +
      (T(upper.x, upper_lower.y, upper.z) -
        T(upper.x, lower_lower.y, upper.z)) *
      factor.x) *
      (1.0f - factor.y) +
      ((T(lower.x, upper_upper.y, upper.z) -
        T(lower.x, lower_upper.y, upper.z)) *
        (1.0f - factor.x) +
        (T(upper.x, upper_upper.y, upper.z) -
          T(upper.x, lower_upper.y, upper.z)) *
        factor.x) *
      factor.y) *
    factor.z;

  gradient.z = (((T(lower.x, lower.y, upper_lower.z) -
    T(lower.x, lower.y, lower_lower.z)) *
    (1.0f - factor.x) +
    (T(upper.x, lower.y, upper_lower.z) -
      T(upper.x, lower.y, lower_lower.z)) *
    factor.x) *
    (1.0f - factor.y) +
    ((T(lower.x, upper.y, upper_lower.z) -
      T(lower.x, upper.y, lower_lower.z)) *
      (1.0f - factor.x) +
      (T(upper.x, upper.y, upper_lower.z) -
        T(upper.x, upper.y, lower_lower.z)) *
      factor.x) *
    factor.y) *
    (1.0f - factor.z) +
    (((T(lower.x, lower.y, upper_upper.z) -
      T(lower.x, lower.y, lower_upper.z)) *
      (1.0f - factor.x) +
      (T(upper.x, lower.y, upper_upper.z) -
        T(upper.x, lower.y, lower_upper.z)) *
      factor.x) *
      (1.0f - factor.y) +
      ((T(lower.x, upper.y, upper_upper.z) -
        T(lower.x, upper.y, lower_upper.z)) *
        (1.0f - factor.x) +
        (T(upper.x, upper.y, upper_upper.z) -
          T(upper.x, upper.y, lower_upper.z)) *
        factor.x) *
      factor.y) *
    factor.z;

  mult3_ew(gradient, 0.5f, gradient, volume_scale_inv);

  return normalise(gradient);
}

void raycast_kernel(float4r &hit, const pos3r &pos, const matrix4r &view)
{
  float3r origin, direction, posf3(pos), inv_dir, volume_dim(config_pm.vol_size_metric);
  float3r tbot, ttop, diff, tmin, tmax, rp;

  get_translation(origin, view);
  get_direction(direction, view, posf3);

  // intersect ray with a box
  // http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
  // compute intersection of ray with all six bbox planes
  inverse(inv_dir, direction);
  mult3_ew(tbot, -1.0f, inv_dir, origin);
  sub3_ew(diff, volume_dim, origin);
  mult3_ew(ttop, 1.0f, inv_dir, diff);

  // re-order intersections to find smallest and largest on each axis
  minf3(tmin, ttop, tbot);
  maxf3(tmax, ttop, tbot);

  // find the largest tmin and the smallest tmax
  const float largest_tmin =
    fmaxf(fmaxf(tmin.x, tmin.y), fmaxf(tmin.x, tmin.z));
  const float smallest_tmax =
    fminf(fminf(tmax.x, tmax.y), fminf(tmax.x, tmax.z));

  // check against near and far plane
  const float tnear = fmaxf(largest_tmin, config_pm.raycast.near_plane);
  const float tfar = fminf(smallest_tmax, config_pm.raycast.far_plane);

  memset(&hit, 0, sizeof(hit));

  if (tnear < tfar) {
    // first walk with largesteps until we found a hit
    float t = tnear;
    float stepsize = config_pm.raycast.large_step;
    linear_step(rp, t, direction, origin); // rp = origin + direction * t
    float f_t = interp(rp);
    float f_tt = 0.0f;

    if (f_t >
      0.0f) { // ups, if we were already in it, then don't render anything here
      for (; t < tfar; t += stepsize) {
        linear_step(rp, t, direction, origin);
        f_tt = interp(rp);
        if (f_tt < 0.0f) // got it, jump out of inner loop
          break;
        if (f_tt < 0.8f) // coming closer, reduce stepsize
          stepsize = config_pm.raycast.step;
        f_t = f_tt;
      }
      if (f_tt < 0) { // got it, calculate accurate intersection
        t = t + stepsize * f_tt / (f_t - f_tt);
        linear_step(rp, t, direction, origin);
        hit.x = rp.x; hit.y = rp.y; hit.z = rp.z; hit.w = t;
        return;
      }
    }
  }

}

void raycast()
{
  const unsigned width = config_pm.width;
  const unsigned height = config_pm.height;

  const float step = config_pm.raycast.step;
  const float large_step = config_pm.raycast.large_step;
  const float near_plane = config_pm.raycast.near_plane;
  const float far_plane = config_pm.raycast.far_plane;
  matrix4r camera_inv, view;
  float4r &camera_intrinsic = *((float4r*)&config_pm.camera);

  get_inverse_camera(camera_inv, camera_intrinsic);
  gemm4x4(view, pose, camera_inv);
  float4r hit;
  float3r surf_norm, hit3, vert_ref, norm_ref,zero;
  const float3r invalid(-2.0f);

  unsigned index = 0;
  const unsigned stride = config_pm.width;
  float norm_mag = 0.0f;

  for (unsigned y = 0; y < height; ++y)
  {
    for (unsigned x = 0; x < width; ++x)
    {
      pos3r pos(x, y, 1);
      index = x + stride * y;
      raycast_kernel(hit, pos, view);
      copy(p_vertex[index], zero);
      copy(p_normal[index], invalid);
      vertex_normal_ref(vert_ref, norm_ref, x, y);

      if (hit.w > 0.0f)
      {
        hit3.x = hit.x; hit3.y = hit.y; hit3.z = hit.z;
        copy(p_vertex[index], hit3);
        
        norm_mag = grad(surf_norm,hit3);

        if (norm_mag > 0.0f)
        {
          copy(p_normal[index], surf_norm);
        }        
      }
    }
  }
}