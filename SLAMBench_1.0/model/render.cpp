#include "render.h"
#include "kf_helper.h"
#include "heatmap.h"
#include <vector>

extern float tsdf[];
extern config_param config_pm;

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

