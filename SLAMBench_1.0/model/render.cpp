#include "render.h"
#include "kf_helper.h"
#include "heatmap.h"
#include <vector>

extern float tsdf[];
extern float weight[];
extern config_param config_pm;
extern float3r* p_vertex;
extern float3r* p_normal;
extern float3r volume_scale;

void model_top_view_from_vertices()
{
  const unsigned vol_size = config_pm.vol_size;
  const unsigned width = config_pm.width;
  const unsigned height = config_pm.height;

  std::vector<pixel> top_heat_map(vol_size*vol_size);
  pixel* p_pixel = &top_heat_map[0];

  float3r* p_vert = p_vertex;
  float vx, vy, vz;
  float3r scaled_pos, half(0.5f), scaled_pos_;

  unsigned index;
  unsigned mag;

  for (unsigned y = 0; y < height; ++y)
  {
    for (unsigned x = 0; x < width; ++x)
    {
      vx = p_vert->x;
      vy = p_vert->y;
      vz = p_vert->z;

      mag = vx*vx + vy*vy + vz*vz;
      
      if (!mag)
      {
        p_vert++;
        continue;
      }

      mult3_ew(scaled_pos, 1.0f, *p_vert, volume_scale);
      sub3_ew(scaled_pos, scaled_pos, half);
      floor3_ew(scaled_pos_, scaled_pos);
      int3r base(scaled_pos_.x, scaled_pos_.y, scaled_pos_.z);

      index = base.x + base.z * vol_size;

      if (p_pixel[index].a <= base.y)
      {
        p_pixel[index].a = (uint8_t)base.y;
      }
      p_vert++;
    }
  }

  // render
  unsigned z_offset = 0;
  pixel* p_pixel_z;
  unsigned y_r;
  float pix_lut[3];
  uint8_t rgb[3];

  for (unsigned z = 0; z < vol_size; ++z) {
    z_offset = (vol_size - z - 1) * vol_size;
    p_pixel_z = &top_heat_map[z_offset];

    for (unsigned x = 0; x < vol_size; ++x)
    {
      y_r = p_pixel_z->a;
      y_r = vol_size - y_r - 1;

      pix_lut[0] = (float)depth_heatmap_lut[y_r << 4][0];
      pix_lut[1] = (float)depth_heatmap_lut[y_r << 4][1];
      pix_lut[2] = (float)depth_heatmap_lut[y_r << 4][2];

      rgb[0] = pix_lut[0] + (1.4065f * (pix_lut[1] - 128.0f));
      rgb[1] = pix_lut[0] - (0.3455f * (pix_lut[2] - 128.0f)) - (0.7169f * (pix_lut[1] - 128));
      rgb[2] = pix_lut[0] + (1.7790f * (pix_lut[2] - 128.0f));
      p_pixel->r = (uint8_t)(rgb[0]);
      p_pixel->g = (uint8_t)(rgb[1]);
      p_pixel->b = (uint8_t)(rgb[2]);

      p_pixel++;
      p_pixel_z++;
    }
  }

  // output image
  std::string filename = "top_view_vertices.bmp";

  write_bitmap(filename, vol_size, vol_size, sizeof(pixel), (uint8_t *)&top_heat_map[0]);
}

void tsdf_visualise()
{
  const unsigned vol_size = config_pm.vol_size;
  const unsigned width = vol_size*16;
  const unsigned height = width;
  std::vector<pixel> top_heat_map(width*height);
  pixel* p_pixel_top = &top_heat_map[0];
  pixel* p_pixel = p_pixel_top;

  float pix_lut[3];
  uint8_t rgb[3];
  float* p_tsdf = tsdf;
  float* p_weight = weight;
  float wght = 0.0f;
  float dst = 0.0f;
  unsigned colour_index = HEATMAP_COLOURS - 1;
  float f_colour_index = 0.0f;
  float max_colour = (float)colour_index;
  //HEATMAP_COLOURS
  for (unsigned z = 0; z < vol_size; ++z)
  {
    p_pixel = p_pixel_top + ((z >> 4) * 256 * width) + (z % 16) * 256;
    for (unsigned y = 0; y < vol_size; ++y)
    {
      for (unsigned x = 0; x < vol_size; ++x)
      {
        wght = *p_weight++;
        dst = *p_tsdf++;

        if (wght > 0.0f)
        {
          // dst = 0.0f; orange
          // dst = -1.0f; green          
          f_colour_index = 0.5f*(1.0f - dst);
          f_colour_index *= f_colour_index;
          f_colour_index *= (float)max_colour;
          colour_index = (unsigned)f_colour_index;
          pix_lut[0] = (float)depth_heatmap_lut[colour_index][0];
          pix_lut[1] = (float)depth_heatmap_lut[colour_index][1];
          pix_lut[2] = (float)depth_heatmap_lut[colour_index][2];

          if (colour_index) {
            rgb[0] = pix_lut[0] + (1.4065f * (pix_lut[1] - 128.0f));
            rgb[1] = pix_lut[0] - (0.3455f * (pix_lut[2] - 128.0f)) - (0.7169f * (pix_lut[1] - 128));
            rgb[2] = pix_lut[0] + (1.7790f * (pix_lut[2] - 128.0f));
          }
          else
          {
            rgb[0] = 255;
            rgb[1] = 255;
            rgb[2] = 255;
          }
          p_pixel[x].r = (uint8_t)(rgb[0]);
          p_pixel[x].g = (uint8_t)(rgb[1]);
          p_pixel[x].b = (uint8_t)(rgb[2]);
        }
      }

      p_pixel += width;
    }
  }

  // output image
  std::string filename = "tsdf_slice.bmp";

  write_bitmap(filename, width, height, sizeof(pixel), (uint8_t *)&top_heat_map[0]);
}

void weight_visualise()
{
  const unsigned vol_size = config_pm.vol_size;
  const unsigned width = vol_size * 16;
  const unsigned height = width;
  std::vector<pixel> top_heat_map(width*height);
  pixel* p_pixel_top = &top_heat_map[0];
  pixel* p_pixel = p_pixel_top;

  float pix_lut[3];
  uint8_t rgb[3];
  float* p_tsdf = tsdf;
  float* p_weight = weight;
  float wght = 0.0f;
  float dst = 0.0f;
  unsigned colour_index = HEATMAP_COLOURS - 1;
  float f_colour_index = 0.0f;
  float max_colour = (float)colour_index;
  //HEATMAP_COLOURS
  for (unsigned z = 0; z < vol_size; ++z)
  {
    p_pixel = p_pixel_top + ((z >> 4) * 256 * width) + (z % 16) * 256;
    for (unsigned y = 0; y < vol_size; ++y)
    {
      for (unsigned x = 0; x < vol_size; ++x)
      {
        wght = *p_weight++;
        dst = *p_tsdf++;

        if (wght > 0.0f)
        {                   
          f_colour_index = wght / 100.0f;
          f_colour_index *= f_colour_index;
          f_colour_index *= (float)max_colour;
          colour_index = (unsigned)f_colour_index;
          pix_lut[0] = (float)depth_heatmap_lut[colour_index][0];
          pix_lut[1] = (float)depth_heatmap_lut[colour_index][1];
          pix_lut[2] = (float)depth_heatmap_lut[colour_index][2];

          if (colour_index) {
            rgb[0] = pix_lut[0] + (1.4065f * (pix_lut[1] - 128.0f));
            rgb[1] = pix_lut[0] - (0.3455f * (pix_lut[2] - 128.0f)) - (0.7169f * (pix_lut[1] - 128));
            rgb[2] = pix_lut[0] + (1.7790f * (pix_lut[2] - 128.0f));
          }
          else
          {
            rgb[0] = 255;
            rgb[1] = 255;
            rgb[2] = 255;
          }
          p_pixel[x].r = (uint8_t)(rgb[0]);
          p_pixel[x].g = (uint8_t)(rgb[1]);
          p_pixel[x].b = (uint8_t)(rgb[2]);
        }
      }

      p_pixel += width;
    }
  }

  // output image
  std::string filename = "weight_slice.bmp";

  write_bitmap(filename, width, height, sizeof(pixel), (uint8_t *)&top_heat_map[0]);
}

void model_top_view()
{
  const unsigned vol_size = config_pm.vol_size;
  std::vector<pixel> top_heat_map(vol_size*vol_size);
  pixel* p_pixel = &top_heat_map[0];
  pixel* p_pixel_z;

  float* p_tsdf = tsdf;
  float df = 0.0f;
  unsigned z_offset = 0;
  float y_scale = 1.0f / (float)(vol_size - 1);
  float f_y = 0.0f,red,green,blue;
  float pix_lut[3], rgb[3];
  unsigned y_r;

  for (unsigned z = 0; z < vol_size; ++z) {
    z_offset = z * vol_size;
    p_pixel_z = &top_heat_map[z_offset];

    for (unsigned y = 0; y < vol_size; ++y)
    {
      p_pixel = p_pixel_z;
      //f_y = (float)y * y_scale;
      y_r = vol_size - y;

      for (unsigned x = 0; x < vol_size; ++x)
      {
        df = *p_tsdf++;

        if (df <= 0.7f && df >= -0.7f)
        {
          //get_heatmap_colour(f_y*2.0f, &red, &green, &blue);
          pix_lut[0] = (float)depth_heatmap_lut[y_r << 4][0];
          pix_lut[1] = (float)depth_heatmap_lut[y_r << 4][1];
          pix_lut[2] = (float)depth_heatmap_lut[y_r << 4][2];

          rgb[0] = pix_lut[0] + (1.4065f * (pix_lut[1] - 128.0f));
          rgb[1] = pix_lut[0] - (0.3455f * (pix_lut[2] - 128.0f)) - (0.7169f * (pix_lut[1] - 128));
          rgb[2] = pix_lut[0] + (1.7790f * (pix_lut[2] - 128.0f));

          p_pixel->r = (uint8_t)(rgb[0]);
          p_pixel->g = (uint8_t)(rgb[1]);
          p_pixel->b = (uint8_t)(rgb[2]);

          /*p_pixel->r = (uint8_t)(red * 255.0f);
          p_pixel->g = (uint8_t)(green * 255.0f);
          p_pixel->b = (uint8_t)(blue * 255.0f);*/
     
        }
        
        p_pixel++;
      }
    }
  }

  // output image
  std::string filename = "top_view.bmp";

  write_bitmap(filename, vol_size, vol_size, sizeof(pixel), (uint8_t *)&top_heat_map[0]);
}

