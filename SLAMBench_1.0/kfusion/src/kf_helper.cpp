#include "kf_helper.h"


static std::string frame_number(const unsigned &frame)
{
	char file_num[32];

	sprintf(file_num, "%04u", frame);

	return std::string(file_num);
}

std::string tsdf_dat(const unsigned &frame)
{
	std::string file_name = output_folder;
	file_name.append(tsdf_folder);	
	file_name.append(frame_number(frame));
	file_name.append(".dat");

	return file_name;
}

std::string config_filename()
{
	std::string file_name = output_folder;
	file_name.append(config_folder);	
	file_name.append(".dat");

	return file_name;
}


void save_config(config_param &config_pm)
{
	std::string config_filenm = config_filename();

	FILE* config_file = NULL;

	config_file = fopen(config_filenm.c_str(),"wb");

	if (config_file)
	{
		fwrite(&config_pm, sizeof(config_pm), 1, config_file);
		fclose(config_file);
	}
}

void load_config(config_param &config_pm)
{
	std::string config_filenm = config_filename();

	FILE* config_file = NULL;

	config_file = fopen(config_filenm.c_str(), "rb");

	if (config_file)
	{
		fread(&config_pm, sizeof(config_pm), 1, config_file);
		fclose(config_file);
	}
}

void load_tsdf(const unsigned &frame, uint16_t* p_tsdf, unsigned &vol_size)
{
	std::string tsdf_filename = tsdf_dat(frame);

	FILE* tsdf_file = NULL;

	tsdf_file = fopen(tsdf_filename.c_str(), "rb");
	vol_size = 0;
	const static size_t v128 = 8388608;
	const static size_t v256 = 67108864;
	const static size_t v512 = 536870912;

	if (tsdf_file)
	{
		fseek(tsdf_file, 0, SEEK_END);
		size_t file_size = ftell(tsdf_file);
		fseek(tsdf_file, 0, SEEK_SET);

		fread(p_tsdf, 1, file_size, tsdf_file);

		switch (file_size)
		{
		case v128:
			vol_size = 128;
			break;
		case v256:
			vol_size = 256;
			break;
		case v512:
			vol_size = 512;
			break;
		default:
			vol_size = 0;
			break;
		};

		fclose(tsdf_file);
	}
}

void save_tsdf(const unsigned &frame, uint16_t* p_tsdf, const unsigned &voxels)
{
	std::string tsdf_filename = tsdf_dat(frame);

	FILE* tsdf_file = NULL;

	tsdf_file = fopen(tsdf_filename.c_str(), "wb");

	if (tsdf_file)
	{
		fwrite(p_tsdf, 2 * sizeof(short), voxels, tsdf_file);
		fclose(tsdf_file);
	}

}

std::string vert_norm_dat(const unsigned &frame)
{
	std::string file_name = output_folder;
	file_name.append(vert_norm_folder);
	file_name.append(frame_number(frame));
	file_name.append(".dat");

	return file_name;
}

void save_vertex_normal(const unsigned &frame, float* p_vertex, float* p_normal, const unsigned &pixels_)
{
	std::string vert_norm_filename = vert_norm_dat(frame);

	FILE* vert_norm_file = NULL;

	vert_norm_file = fopen(vert_norm_filename.c_str(), "wb");
	size_t byte_count = 3 * sizeof(float)*pixels_;
	

	if (vert_norm_file)
	{
		fwrite(p_vertex, 1, byte_count, vert_norm_file);
		fwrite(p_normal, 1, byte_count, vert_norm_file);
		fclose(vert_norm_file);
	}
}

void load_vertex_normal(const unsigned &frame, float* p_vertex, float* p_normal, const unsigned &pixels_)
{
	std::string vert_norm_filename = vert_norm_dat(frame);

	FILE* vert_norm_file = NULL;

	vert_norm_file = fopen(vert_norm_filename.c_str(), "rb");
	size_t byte_count = 3 * sizeof(float)*pixels_;


	if (vert_norm_file)
	{
		fread(p_vertex, 1, byte_count, vert_norm_file);
		fread(p_normal, 1, byte_count, vert_norm_file);
		fclose(vert_norm_file);
	}
}

std::string pose_dat(const unsigned &frame,bool prior)
{
	std::string file_name = output_folder;
	file_name.append(pose_folder);
	if (prior) file_name.append("_pr");
	file_name.append(frame_number(frame));
	file_name.append(".dat");

	return file_name;
}

void save_pose(const unsigned &frame, float* pose[4], bool prior)
{
	std::string pose_file = pose_dat(frame, prior);

	FILE* h_pose = NULL;

	h_pose = fopen(pose_file.c_str(), "wb");

	if (h_pose)
	{
		fwrite(pose[0], sizeof(float), 4, h_pose);
		fwrite(pose[1], sizeof(float), 4, h_pose);
		fwrite(pose[2], sizeof(float), 4, h_pose);
		fwrite(pose[3], sizeof(float), 4, h_pose);

		fclose(h_pose);
	}
}

void load_pose(const unsigned &frame, float* pose[4], bool prior)
{
	std::string pose_file = pose_dat(frame, prior);

	FILE* h_pose = NULL;

	h_pose = fopen(pose_file.c_str(), "rb");

	if (h_pose)
	{
		fread(pose[0], sizeof(float), 4, h_pose);
		fread(pose[1], sizeof(float), 4, h_pose);
		fread(pose[2], sizeof(float), 4, h_pose);
		fread(pose[3], sizeof(float), 4, h_pose);

		fclose(h_pose);
	}
}

std::string depth_render_bmp(const unsigned &frame)
{
	std::string file_name = output_folder;
	file_name.append(render_depth_folder);
	file_name.append(frame_number(frame));
	file_name.append(".bmp");

	return file_name;
}

std::string track_render_bmp(const unsigned &frame)
{
	std::string file_name = output_folder;
	file_name.append(render_track_folder);
	file_name.append(frame_number(frame));
	file_name.append(".bmp");

	return file_name;
}

std::string volume_render_bmp(const unsigned &frame)
{
	std::string file_name = output_folder;
	file_name.append(render_volume_folder);
	file_name.append(frame_number(frame));
	file_name.append(".bmp");

	return file_name;
}

void write_bitmap(std::string filename, int _width, int _height, int planes,
	uint8_t *dataPtr) {
	FILE *bitmapFile;
	short Type = 19778;
	int Size;
	int Reserved = 0;
	int Offset = 54;
	int headerSize = 40;
	int Width = _width;
	int Height = _height;
	short Planes = 1;
	short BitsPerPixel = 8 * planes;
	int Compression = 0;
	int SizeImage;
	int XPixelsPerMeter = 0;
	int YPixelsPerMeter = 0;
	int ColorsUsed = 0;
	int ColorsImportant = 0;

	int stride = Width * (BitsPerPixel / 8);
	int bytesPerLine = Width * (BitsPerPixel / 8) * sizeof(unsigned char);
	int pad = stride % 4;

	if (pad != 0) {
		stride += (4 - pad);
	}

	SizeImage = stride * Height;
	Size = SizeImage + Offset;
	unsigned char *writeBuffer = new unsigned char[stride];

	bitmapFile = fopen(filename.c_str(), "wb");

	fwrite((const void *)&Type, sizeof(short), 1, bitmapFile);
	fwrite((const void *)&Size, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&Reserved, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&Offset, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&headerSize, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&Width, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&Height, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&Planes, sizeof(short), 1, bitmapFile);
	fwrite((const void *)&BitsPerPixel, sizeof(short), 1, bitmapFile);
	fwrite((const void *)&Compression, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&SizeImage, sizeof(int), 1, bitmapFile);

	fwrite((const void *)&XPixelsPerMeter, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&YPixelsPerMeter, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&ColorsUsed, sizeof(int), 1, bitmapFile);
	fwrite((const void *)&ColorsImportant, sizeof(int), 1, bitmapFile);

	unsigned char *linePtr = dataPtr + (bytesPerLine * (Height - 1));

	for (int row = 0; row < Height; row++) {
		memcpy((void *)writeBuffer, (const void *)linePtr, bytesPerLine);
		fwrite((void *)writeBuffer, sizeof(unsigned char), stride, bitmapFile);
		linePtr -= bytesPerLine;
	}

	fclose(bitmapFile);
}