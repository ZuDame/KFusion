#ifndef RAYCAST_MATH_H
#define RAYCAST_MATH_H
#include <string.h>


typedef struct int3r {
	int3r(int v)
	{
		x = v; y = v; z = v;
	}

	int3r() : x(0),y(0),z(0)
	{}

	int3r(int x_, int y_, int z_)
	{
		x = x_; y = y_; z = z_;
	}

	int3r(float f_x, float f_y, float f_z)
	{
		x = (int)f_x; y = (int)f_y; z = (int)f_z;
	}

	int x;
	int y;
	int z;
}int3r;

typedef int3r pos3r;

typedef struct float3r {
	float3r()
	{ 
		x = 0.0f; y = 0.0f; z = 0.0f;
	}

	float3r(float a)
	{
		x = a; y = a; z = a;
	}

  float3r(float a,float b, float c)
  {
    x = a; y = b; z = c;
  }

	float3r(const pos3r p3r)
	{
		x = (float)p3r.x; y = (float)p3r.y; z = (float)p3r.z;
	}

	float x;
	float y;
	float z;
}float3r;


typedef struct float4r {
	float4r() : float4r(0.0f, 0.0f, 0.0f, 0.0f)
	{

	}

	float4r(const float3r &f3r)
	{
		x = f3r.x; y = f3r.y; z = f3r.z; w = 0.0;
	}

	float4r(float x_, float y_, float z_, float w_)
	{
		x = x_; y = y_; z = z_; w = w_;
	}

	float x;
	float y;
	float z;
	float w;
}float4r;

typedef struct matrix4r
{
	matrix4r()
	{
		memset(&r0, 0, sizeof(float4r));
		memset(&r1, 0, sizeof(float4r));
		memset(&r2, 0, sizeof(float4r));
		memset(&r3, 0, sizeof(float4r));
	}

	float4r r0;
	float4r r1;
	float4r r2;
	float4r r3;
}matrix4r;

void get_translation(float3r &trans, const matrix4r &view);
void get_direction(float3r &direction, const matrix4r &view, const float3r &pos);
void inverse(float3r &b, const float3r &a);

void get_inverse_camera(matrix4r &camera, const float4r &camera_intrinsic);
void gemm4x4(matrix4r &C, matrix4r &A, matrix4r &B);
void gemv4(float4r &c, const matrix4r &A, float4r &b);
float dot(const float4r &a, const float4r &b);

void mult3_ew(float3r &c, const float scalar, const float3r &a, const float3r &b);
void sub3_ew(float3r &c, const float3r &a, const float3r &b);
void minf3(float3r &c, const float3r &a, const float3r &b);
void maxf3(float3r &c, const float3r &a, const float3r &b);
void max3(int3r &c, const int3r &a, const int3r &b);
void min3(int3r &c, const int3r &a, const int3r &b);
void fracf3(float3r &f, const float3r &a);
void linear_step(float3r &y, float x, const float3r &m, const float3r &c);

void add3(int3r &b,const int3r &a, int scalar);
void copy(float3r &b, const float3r &a);

float normalise(float3r &a);

float error(const float3r &a, const float3r &b);
#endif