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

void tsdf_u16_to_float();

void raycast_init(const unsigned &frame)
{
	uint16_t* p_tsdf = (uint16_t*)tsdf;
	load_config(config_pm);

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
	if(p_vertex_ref) delete[] p_vertex_ref;
	if(p_normal_ref) delete[] p_normal_ref;
	if (p_vertex) delete[] p_vertex;
	if (p_normal) delete[] p_normal;
}

void tsdf_u16_to_float()
{
	tsdf_weight* p_tw = (tsdf_weight*)tsdf;
	float* p_tsdf = tsdf;
	float* p_weight = weight;

	const unsigned voxels = vol_size * vol_size * vol_size;
	uint16_t tf = 0;
	uint16_t wght = 0;

	for (unsigned v = 0; v < voxels; ++v)
	{
		tf = p_tw->tsdf;
		wght = p_tw->weight;
		*p_tsdf++ = 0.00003051944088f * (float)tf;
		*p_weight++ = (float)wght;
		p_tw++;
	}
}

void raycast_kernel(const pos3r &pos, const matrix4r &view)
{
	float3r origin, direction, posf3(pos), inv_dir, volume_dim(config_pm.vol_size_metric);
	float3r tbot,ttop,diff, tmin, tmax;

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

	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{			
			pos3r pos(x, y, 1);

			//view_ = raycastPose * get_inverse_camera_matrix(cam_k);
			//const float4 hit =
			//	raycast(integration, pos, view, nearPlane, farPlane, step, largestep);
			raycast_kernel(pos,view);
		}
	}
}