#include "raycast_math.h"
#include <string.h>
#include <math.h>

void get_inverse_camera(matrix4r &camera, const float4r &camera_intrinsic)
{
	memset(&camera.r0, 0, sizeof(float4r));
	memset(&camera.r1, 0, sizeof(float4r));
	memset(&camera.r2, 0, sizeof(float4r));
	memset(&camera.r3, 0, sizeof(float4r));

	camera.r0.x = 1.0f / camera_intrinsic.x;
	camera.r1.y = 1.0f / camera_intrinsic.y;
	camera.r0.z = -camera_intrinsic.z / camera_intrinsic.x;
	camera.r1.z = -camera_intrinsic.w / camera_intrinsic.y;
	camera.r2.z = 1.0;
	camera.r3.w = 1.0;	
}

inline float dot(const float4r &a, const float4r &b)
{	
	return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}
void gemm4x4(matrix4r &C, matrix4r &A, matrix4r &B)
{
	float4r b_col0(B.r0.x, B.r1.x, B.r2.x, B.r3.x);
	float4r b_col1(B.r0.y, B.r1.y, B.r2.y, B.r3.y);
	float4r b_col2(B.r0.z, B.r1.z, B.r2.z, B.r3.z);
	float4r b_col3(B.r0.w, B.r1.w, B.r2.w, B.r3.w);

	C.r0.x = dot(A.r0, b_col0);
	C.r1.x = dot(A.r1, b_col0);
	C.r2.x = dot(A.r2, b_col0);
	C.r3.x = dot(A.r3, b_col0);

	C.r0.y = dot(A.r0, b_col1);
	C.r1.y = dot(A.r1, b_col1);
	C.r2.y = dot(A.r2, b_col1);
	C.r3.y = dot(A.r3, b_col1);

	C.r0.z = dot(A.r0, b_col2);
	C.r1.z = dot(A.r1, b_col2);
	C.r2.z = dot(A.r2, b_col2);
	C.r3.z = dot(A.r3, b_col2);

	C.r0.w = dot(A.r0, b_col3);
	C.r1.w = dot(A.r1, b_col3);
	C.r2.w = dot(A.r2, b_col3);
	C.r3.w = dot(A.r3, b_col3);
}

void gemv4(float4r &c, const matrix4r &A, float4r &b)
{
	c.x = dot(A.r0, b);
	c.y = dot(A.r1, b);
	c.z = dot(A.r2, b);
	c.w = dot(A.r3, b);
}

void get_translation(float3r &trans, const matrix4r &view)
{	
	trans.x = view.r0.w;
	trans.y = view.r1.w;
	trans.z = view.r2.w;	
}

void inverse(float3r &b, const float3r &a)
{
	b.x = 1.0f / a.x;
	b.y = 1.0f / a.y;
	b.z = 1.0f / a.z;
}

void minf3(float3r &c, const float3r &a, const float3r &b)
{
	c.x = a.x < b.x ? a.x : b.x;
	c.y = a.y < b.y ? a.y : b.y;
	c.z = a.z < b.z ? a.z : b.z;
}

void maxf3(float3r &c, const float3r &a, const float3r &b)
{
	c.x = a.x > b.x ? a.x : b.x;
	c.y = a.y > b.y ? a.y : b.y;
	c.z = a.z > b.z ? a.z : b.z;
}

void max3(int3r &c, const int3r &a, const int3r &b)
{
	c.x = a.x > b.x ? a.x : b.x;
	c.y = a.y > b.y ? a.y : b.y;
	c.z = a.z > b.z ? a.z : b.z;
}

void min3(int3r &c, const int3r &a, const int3r &b)
{
	c.x = a.x < b.x ? a.x : b.x;
	c.y = a.y < b.y ? a.y : b.y;
	c.z = a.z < b.z ? a.z : b.z;
}

void copy(float3r &b, const float3r &a)
{
  b.x = a.x; b.y = a.y; b.z = a.z;
}

void add3(int3r &b, const int3r &a, int scalar)
{
  b.x = a.x + scalar;
  b.y = a.y + scalar;
  b.z = a.z + scalar;
}


void sub3_ew(float3r &c, const float3r &a, const float3r &b)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

void mult3_ew(float3r &c, const float scalar, const float3r &a, const float3r &b)
{
	c.x = scalar * a.x * b.x;
	c.y = scalar * a.y * b.y;
	c.z = scalar * a.z * b.z;
}
void get_direction(float3r &direction, const matrix4r &view, const float3r &pos)
{
	float4r pos4(pos);
	float4r dir4;

	gemv4(dir4, view, pos4);
	direction.x = dir4.x; direction.y = dir4.y; direction.z = dir4.z;
	//direction = view_(1:3, 1 : 3)*[pos(1) pos(2) 1]';
}

void linear_step(float3r &y, float x, const float3r &m, const float3r &c)
{
	y.x = x * m.x + c.x;
	y.y = x * m.y + c.y;
	y.z = x * m.z + c.z;
}

void fracf3(float3r &f, const float3r &a)
{
	int3r i_a(a.x, a.y, a.z);

	f.x = a.x - (float)i_a.x;
	f.y = a.y - (float)i_a.y;
	f.z = a.z - (float)i_a.z;
}

float normalise(float3r &a)
{
  float mag = a.x*a.x + a.y*a.y + a.z*a.z;

  if (mag > 0.0f)
  {
    mag = sqrtf(mag);
    a.x /= mag;
    a.y /= mag;
    a.z /= mag;
  }
  
  return mag;
}