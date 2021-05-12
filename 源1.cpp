#include <iostream>
#include <fstream>
#include <sstream>
#include "myopencl.h"
//#include <CL/cl.h>
const int ARRAY_SIZE = 10;

#pragma warning( disable : 4996 )

//һ�� ѡ��OpenCLƽ̨������һ��������
cl_context CreateContext()
{
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;
	cl_context context = NULL;

	//ѡ����õ�ƽ̨�еĵ�һ��
	errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if (errNum != CL_SUCCESS || numPlatforms <= 0)
	{
		std::cerr << "Failed to find any OpenCL platforms." << std::endl;
		return NULL;
	}

	//����һ��OpenCL�����Ļ���
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


//���� �����豸�������������
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device, int num,int order)
{
	cl_int errNum;
	cl_device_id *devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;

	// ��ȡ�豸��������С
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
	
	if (deviceBufferSize <= 0)
	{
		std::cerr << "No devices available.";
		return NULL;
	}

	// Ϊ�豸���仺��ռ�
	devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
	//ѡȡ�����豸�еĵ�һ��
	if(order==1){
		commandQueue = clCreateCommandQueue(context, devices[num],CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, NULL);
	}
	else commandQueue = clCreateCommandQueue(context, devices[num], 0, NULL);

	*device = devices[num];
	delete[] devices;
	return commandQueue;
}

// ���������͹����������
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

//�����͹����������
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


// �ͷ�OpenCL��Դ
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
	opencl_start();
	cl_context context = 0;
	cl_command_queue commandQueue = 0;
	cl_command_queue commandQueue2 = 0;
	//cl_command_queue commandQueue3 = 0;
	cl_program program = 0;
	cl_device_id device = 0;
	cl_device_id device2 = 0;
	//cl_device_id device3 = 0;
	cl_kernel kernel[8];
	cl_mem memObjects[8][3] = {{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 },{ 0, 0, 0 }};
	cl_int errNum;
	//cl_event event[20];
	// һ��ѡ��OpenCLƽ̨������һ��������
	context = CreateContext();
	
	// ���� �����豸�������������
	commandQueue = CreateCommandQueue(context, &device,0,1);
	commandQueue2 = CreateCommandQueue(context, &device2,1,0);
	//commandQueue3 = CreateCommandQueue(context, &device3,2,0);
	//�����͹����������
	program = CreateProgram(context, device, "C:\\Users\\Administrator\\Desktop\\����\\code 1.17\\HelloWorld.cl");

	// �ġ� ����OpenCL�ں˲������ڴ�ռ�
	kernel[0] = clCreateKernel(program, "kernel0", NULL);
	kernel[1] = clCreateKernel(program, "kernel1", NULL);
	kernel[2] = clCreateKernel(program, "kernel2", NULL);
	kernel[3] = clCreateKernel(program, "kernel3", NULL);
	kernel[4] = clCreateKernel(program, "kernel4", NULL);
	kernel[5] = clCreateKernel(program, "kernel5", NULL);
	kernel[6] = clCreateKernel(program, "kernel6", NULL);
	kernel[7] = clCreateKernel(program, "kernel7", NULL);
	//����Ҫ���������
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
	//�����ڴ����
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
	// �塢 �����ں����ݲ�ִ���ں�
	for(int i=0;i<8;i++){
		errNum = clSetKernelArg(kernel[i], 0, sizeof(cl_mem), &memObjects[i][0]);
		errNum |= clSetKernelArg(kernel[i], 1, sizeof(cl_mem), &memObjects[i][1]);
		errNum |= clSetKernelArg(kernel[i], 2, sizeof(cl_mem), &memObjects[i][2]);
	}
	clSetKernelArg(kernel[0], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_ADD);
	clSetKernelArg(kernel[1], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_SUB);
	clSetKernelArg(kernel[2], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_MUL);
	clSetKernelArg(kernel[3], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_DIV);
	clSetKernelArg(kernel[4], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_ASSIGN);
	clSetKernelArg(kernel[5], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_ADD);
	clSetKernelArg(kernel[6], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_SUB);
	clSetKernelArg(kernel[7], 0, sizeof(cl_mem), ARG_KERNEL_TYPE_MUL);
	
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

	// ���� ��ȡִ�н�����ͷ�OpenCL��Դ
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

	/*for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < ARRAY_SIZE; j++)
		{
			std::cout << result[i][j] << " ";
		}
		std::cout << std::endl;
		std::cout << "Executed kernel"<< i << " successfully." << std::endl;
	}*/

	opencl_end();
		
	return 0;

}
