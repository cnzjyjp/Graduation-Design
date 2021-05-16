__kernel void kernel0(__global const float *a,
	__global const float *b,
	__global float *result)
{
	int gid = get_global_id(0);
 
	result[gid] = a[gid] + b[gid];
}

__kernel void kernel1(__global const float *a1,
	__global const float *b1,
	__global float *result1)
{
	int gid = get_global_id(0);
 
	result1[gid] = a1[gid] - b1[gid];
}

__kernel void kernel2(__global const float *a2,
	__global const float *b2,
	__global float *result2)
{
	int gid = get_global_id(0);
 
	result2[gid] = a2[gid] * b2[gid];
}

__kernel void kernel3(__global const float *a3,
	__global const float *b3,
	__global float *result3)
{
	int gid = get_global_id(0);
 
	result3[gid] = a3[gid] / b3[gid];
}

__kernel void kernel4(__global const float *a4,
	__global const float *b4,
	__global float *result4)
{
	int gid = get_global_id(0);
 
	result4[gid] = 100;
}

__kernel void kernel5(__global const float *a5,
	__global const float *b5,
	__global float *result5)
{
	int gid = get_global_id(0);
 
	result5[gid] = a5[gid] + b5[gid];
}

__kernel void kernel6(__global const float *a6,
	__global const float *b6,
	__global float *result6)
{
	int gid = get_global_id(0);
 
	result6[gid] = a6[gid] - b6[gid];
}

__kernel void kernel7(__global const float *a7,
	__global const float *b7,
	__global float *result7)
{
	int gid = get_global_id(0);
 
	result7[gid] = a7[gid] * b7[gid];
}
