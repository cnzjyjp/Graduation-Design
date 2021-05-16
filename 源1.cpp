//该样例为正常功能的 OpenCL 程序，如需将其适配给 myopencl.h 进行分析，则需要按照注释对代码进行修改。 
#include <iostream>
#include <fstream>
#include <sstream>
//#include "myopencl.h"			//如需将其适配给 myopencl.h，则将该行注释取消
#include <CL/cl.h>				//如需将其适配给 myopencl.h，则将该行注释
const int ARRAY_SIZE = 10;

#pragma warning( disable : 4996 )

//一、 选择OpenCL平台并创建一个上下文
cl_context CreateContext()
{
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;
	cl_context context = NULL;

	//选择可用的平台中的第一个
	errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if (errNum != CL_SUCCESS || numPlatforms <= 0)
	{
		std::cerr << "Failed to find any OpenCL platforms." << std::endl;
		return NULL;
	}

	//创建一个OpenCL上下文环境
	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)firstPlatformId,
		0
	};
	context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_ALL,
		NULL, NULL, &errNum);

	return context;
}


//二、 创建设备并创建命令队列
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device, int num,int order)
{
	cl_int errNum;
	cl_device_id *devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;

	// 获取设备缓冲区大小
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
	
	if (deviceBufferSize <= 0)
	{
		std::cerr << "No devices available.";
		return NULL;
	}

	// 为设备分配缓存空间
	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
	//选取可用设备中的第一个
	if(order==1){
		commandQueue = clCreateCommandQueue(context, devices[num],CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, NULL);
	}
	else commandQueue = clCreateCommandQueue(context, devices[num], 0, NULL);

	*device = devices[num];
	delete[] devices;
	return commandQueue;
}

// 三、创建和构建程序对象
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName)
{
	cl_int errNum;
	cl_program program;

	std::ifstream kernelFile(fileName, std::ios::in);
	if (!kernelFile.is_open())
	{
		std::cerr << "Failed to open file for reading: " << fileName << std::endl;
		return NULL;
	}

	std::ostringstream oss;
	oss << kernelFile.rdbuf();

	std::string srcStdStr = oss.str();
	const char *srcStr = srcStdStr.c_str();
	program = clCreateProgramWithSource(context, 1,
		(const char**)&srcStr,
		NULL, NULL);

	errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	return program;
}

//创建和构建程序对象
bool CreateMemObjects(cl_context context, cl_mem memObjects[3],
	float *a, float *b)
{
	memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * ARRAY_SIZE, a, NULL);
	memObjects[1] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * ARRAY_SIZE, b, NULL);
	memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE,
		sizeof(float) * ARRAY_SIZE, NULL, NULL);
	return true;
}


// 释放OpenCL资源
void Cleanup(cl_context context, cl_command_queue commandQueue,
	cl_program program, cl_kernel kernel, cl_mem memObjects[3])
{
	for (int i = 0; i < 3; i++)
	{
		if (memObjects[i] != 0)
			clReleaseMemObject(memObjects[i]);
	}
	if (commandQueue != 0)
		clReleaseCommandQueue(commandQueue);
	
	if	(kernel != 0)
		clReleaseKernel(kernel);

	if (program != 0)
		clReleaseProgram(program);

	if (context != 0)
		clReleaseContext(context);
}

int main(int argc, char** argv)
{
	//opencl_start();			//如需将其适配给 myopencl.h，则将该行注释取消
	cl_context context = 0;
	cl_command_queue commandQueue = 0;
	cl_command_queue commandQueue2 = 0;
	cl_program program = 0;
	cl_device_id device = 0;
	cl_device_id device2 = 0;
	cl_kernel kernel[8];
	cl_mem memObjects[8][3] = {{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 }};
	cl_int errNum;
	cl_event event[20];			//如需将其适配给 myopencl.h，则将该行注释
	// 一、选择OpenCL平台并创建一个上下文
	context = CreateContext();
	
	// 二、 创建设备并创建命令队列
	commandQueue = CreateCommandQueue(context, &device,0,1);
	commandQueue2 = CreateCommandQueue(context, &device2,1,0);
	//创建和构建程序对象
	program = CreateProgram(context, device, "C:\\Users\\Administrator\\Desktop\\code 1.17\\HelloWorld.cl");   //HelloWorld.cl的路径

	// 四、 创建OpenCL内核并分配内存空间
	kernel[0] = clCreateKernel(program, "kernel0", NULL);
	kernel[1] = clCreateKernel(program, "kernel1", NULL);
	kernel[2] = clCreateKernel(program, "kernel2", NULL);
	kernel[3] = clCreateKernel(program, "kernel3", NULL);
	kernel[4] = clCreateKernel(program, "kernel4", NULL);
	kernel[5] = clCreateKernel(program, "kernel5", NULL);
	kernel[6] = clCreateKernel(program, "kernel6", NULL);
	kernel[7] = clCreateKernel(program, "kernel7", NULL);
	//创建要处理的数据
	float result[8][ARRAY_SIZE];
	float a[8][ARRAY_SIZE];
	float b[8][ARRAY_SIZE];
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < ARRAY_SIZE; j++){
			a[i][j] = (float)j;
			b[i][j] = (float)(ARRAY_SIZE - j);
		}
	}
	//创建内存对象
	if (!CreateMemObjects(context, memObjects[0], a[0], b[0]))
	{
		Cleanup(context, commandQueue, program, kernel[0], memObjects[0]);
		return 1;
	}
	if (!CreateMemObjects(context, memObjects[1], a[1], b[1]))
	{
		Cleanup(context, commandQueue2, program, kernel[1], memObjects[1]);
		return 1;
	}
	if (!CreateMemObjects(context, memObjects[2], a[2], b[2]))
	{
		Cleanup(context, commandQueue, program, kernel[2], memObjects[2]);
		return 1;
	}
	if (!CreateMemObjects(context, memObjects[3], a[3], b[3]))
	{
		Cleanup(context, commandQueue2, program, kernel[3], memObjects[3]);
		return 1;
	}
	if (!CreateMemObjects(context, memObjects[4], a[4], b[4]))
	{
		Cleanup(context, commandQueue, program, kernel[4], memObjects[4]);
		return 1;
	}
	if (!CreateMemObjects(context, memObjects[5], a[5], b[5]))
	{
		Cleanup(context, commandQueue2, program, kernel[5], memObjects[5]);
		return 1;
	}
	if (!CreateMemObjects(context, memObjects[6], a[6], b[6]))
	{
		Cleanup(context, commandQueue, program, kernel[6], memObjects[6]);
		return 1;
	}
	if (!CreateMemObjects(context, memObjects[7], a[7], b[7]))
	{
		Cleanup(context, commandQueue2, program, kernel[7], memObjects[7]);
		return 1;
	}
	// 五、 设置内核数据并执行内核
	for(int i=0;i<8;i++){
		errNum = clSetKernelArg(kernel[i], 0, sizeof(cl_mem), &memObjects[i][0]);
		errNum |= clSetKernelArg(kernel[i], 1, sizeof(cl_mem), &memObjects[i][1]);
		errNum |= clSetKernelArg(kernel[i], 2, sizeof(cl_mem), &memObjects[i][2]);
	}
	/*clSetKernelArg(kernel[0], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_ADD);
	clSetKernelArg(kernel[1], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_SUB);
	clSetKernelArg(kernel[2], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_MUL);
	clSetKernelArg(kernel[3], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_DIV);
	clSetKernelArg(kernel[4], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_ASSIGN);
	clSetKernelArg(kernel[5], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_ADD);
	clSetKernelArg(kernel[6], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_SUB);
	clSetKernelArg(kernel[7], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_MUL);*/
	//如需将其适配给 myopencl.h，则将该块注释取消
	size_t globalWorkSize[1] = { ARRAY_SIZE };
	size_t localWorkSize[1] = { 1 };

	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[0], 1, NULL,
		globalWorkSize, localWorkSize,
		0, NULL, &event[0]);

	errNum = clEnqueueNDRangeKernel(commandQueue2, kernel[1], 1, NULL,
		globalWorkSize, localWorkSize,
		0, NULL, &event[1]);
	
	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[2], 1, NULL,
		globalWorkSize, localWorkSize,
		1, &event[0], &event[2]);

	clFinish(commandQueue);

	errNum = clEnqueueNDRangeKernel(commandQueue2, kernel[3], 1, NULL,
		globalWorkSize, localWorkSize,
		2, &event[0], &event[3]);

	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[4], 1, NULL,
		globalWorkSize, localWorkSize,
		1, &event[1], &event[4]);

	clEnqueueBarrier(commandQueue);

	errNum = clEnqueueNDRangeKernel(commandQueue2, kernel[5], 1, NULL,
		globalWorkSize, localWorkSize,
		2, &event[2], &event[5]);

	errNum = clEnqueueNDRangeKernel(commandQueue, kernel[6], 1, NULL,
		globalWorkSize, localWorkSize,
		2, &event[3], &event[6]);

	errNum = clEnqueueNDRangeKernel(commandQueue2, kernel[7], 1, NULL,
		globalWorkSize, localWorkSize,
		0, NULL, NULL);

	// 六、 读取执行结果并释放OpenCL资源
	errNum = clEnqueueReadBuffer(commandQueue, memObjects[0][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[0],
		0, NULL, NULL);

	errNum = clEnqueueReadBuffer(commandQueue2, memObjects[1][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[1],
		0, NULL, NULL);

	errNum = clEnqueueReadBuffer(commandQueue, memObjects[2][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[2],
		0, NULL, NULL);

	errNum = clEnqueueReadBuffer(commandQueue2, memObjects[3][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[3],
		0, NULL, NULL);

	errNum = clEnqueueReadBuffer(commandQueue, memObjects[4][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[4],
		0, NULL, NULL);

	errNum = clEnqueueReadBuffer(commandQueue2, memObjects[5][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[5],
		0, NULL, NULL);

	errNum = clEnqueueReadBuffer(commandQueue, memObjects[6][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[6],
		0, NULL, NULL);

	errNum = clEnqueueReadBuffer(commandQueue2, memObjects[7][2], CL_TRUE,
		0, ARRAY_SIZE * sizeof(float), result[7],
		0, NULL, NULL);

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < ARRAY_SIZE; j++)
		{
			std::cout << result[i][j] << " ";
		}
		std::cout << std::endl;
		std::cout << "Executed kernel"<< i << " successfully." << std::endl;
	}
	//如需将其适配给 myopencl.h，则将该 for 循环块注释

	//opencl_end();		//如需将其适配给 myopencl.h，则将该行注释取消
		
	return 0;

}
