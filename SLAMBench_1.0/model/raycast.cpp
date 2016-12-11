#include "raycast.h"
#include "kf_helper.h"

float3r* p_vertex;
float3r* p_normal;
unsigned vol_size;
float tsdf[MAX_VOXELS];


void tsdf_u16_to_float();

void raycast_init(const unsigned &frame)
{
	uint16_t* p_tsdf = (uint16_t*)tsdf;

	load_tsdf(frame, p_tsdf, vol_size);
	tsdf_u16_to_float();
}

void tsdf_u16_to_float()
{
	tsdf_weight* p_tw = (tsdf_weight*)tsdf;
	float* p_tsdf = tsdf;

	const unsigned voxels = vol_size * vol_size * vol_size;
	uint16_t tf = 0;

	for (unsigned v = 0; v < voxels; ++v)
	{
		tf = p_tw->tsdf;
		*p_tsdf++ = 0.00003051944088f * (float)tf;
		p_tw++;
	}
}
