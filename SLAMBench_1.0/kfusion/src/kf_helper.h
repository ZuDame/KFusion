#ifndef KF_HELPER_H
#define KF_HELPER_H
#include <stdint.h>
#include <stdio.h>
#include <string>

const std::string output_folder = "output\\";
const std::string render_volume_folder = "render_volume\\rv";
const std::string render_track_folder = "render_track\\rt";
const std::string render_depth_folder = "render_depth\\rd";
const std::string pose_folder = "pose\\p";
const std::string tsdf_folder = "tsdf\\dw";
const std::string vert_norm_folder = "vert_norm\\vn";

void write_bitmap(std::string filename, int _width, int _height, int planes,
	uint8_t *dataPtr);

std::string volume_render_bmp(const unsigned &frame);
std::string track_render_bmp(const unsigned &frame);
std::string depth_render_bmp(const unsigned &frame);

void save_pose(const unsigned &frame, float* pose[4], bool prior);
void save_tsdf(const unsigned &frame, uint16_t* p_tsdf, const unsigned &voxels);
void save_vertex_normal(const unsigned &frame, float* p_vertex, float* p_normal,const unsigned &pixels);
#endif