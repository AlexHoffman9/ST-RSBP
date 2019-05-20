#include "dataPretreatment.cuh"
#include "cuda_runtime.h"
#include "cuda_runtime_api.h"
#include "../common/cuMatrix.h"
#include "../common/cuMatrixVector.h"

__global__ void g_getAver(float ** input1,
	float** input2, 
	float* aver,
	int num_of_input1, 
	int num_of_input2,
	int imgSize)
{
	for(int j = 0; j < num_of_input1; j++)
	{
		for(int i = 0; i < imgSize; i += blockDim.x)
		{
			int idx = threadIdx.x + i;
			if(idx < imgSize)
			{
				aver[idx] += input1[j][idx];
			}
		}
	}

	__syncthreads();

	for(int j = 0; j < num_of_input2; j++)
	{
		for(int i = 0; i < imgSize; i += blockDim.x)
		{
			int idx = threadIdx.x + i;
			if(idx < imgSize)
			{
				aver[idx] += input2[j][idx];
			}
		}
	}

	__syncthreads();

	for(int i = 0; i < imgSize; i += blockDim.x)
	{
		int idx = i + threadIdx.x;
		if(idx < imgSize)
		{
			aver[idx] /= (num_of_input1 + num_of_input2);
		}
	}

	__syncthreads();


	for(int j = 0; j < num_of_input1; j++)
	{
		for(int i = 0; i < imgSize; i += blockDim.x)
		{
			int idx = threadIdx.x + i;
			if(idx < imgSize)
			{
				input1[j][idx] -= aver[idx];
			}
		}
	}

	__syncthreads();

	for(int j = 0; j < num_of_input2; j++)
	{
		for(int i = 0; i < imgSize; i += blockDim.x)
		{
			int idx = threadIdx.x + i;
			if(idx < imgSize)
			{
				input2[j][idx] -= aver[idx];
			}
		}
	}
}

void 
	preProcessing(cuMatrixVector<float>&trainX, cuMatrixVector<float>&testX)
{
	int n_rows     = trainX[0]->rows;
	int n_cols     = trainX[0]->cols;
	int n_channels = trainX[0]->channels;

	cuMatrix<float>* aver(new cuMatrix<float>(n_rows, n_cols, n_channels));

	for (int imgId = 0; imgId < (int)trainX.size(); imgId++) {
		int len = trainX[0]->getLen();
		for (int i = 0; i < len; i++) {
			aver->getHost()[i] += trainX[imgId]->getHost()[i];
		}
	}

	for(int i = 0; i < aver->getLen(); i++){
		int len = trainX.size();
		aver->getHost()[i] /= len;
	}

	for (int imgId = 0; imgId < (int)trainX.size(); imgId++) {
		int len = trainX[0]->getLen();
		for (int i = 0; i < len; i++) {
			 trainX[imgId]->getHost()[i] -= aver->getHost()[i];
		}
	}

	aver->cpuClear();

	for (int imgId = 0; imgId < (int)testX.size(); imgId++) {
		int len = testX[0]->getLen();
		for (int i = 0; i < len; i++) {
			aver->getHost()[i] += testX[imgId]->getHost()[i];
		}
	}

	for(int i = 0; i < aver->getLen(); i++){
		int len = testX.size();
		aver->getHost()[i] /= len;
	}

	for (int imgId = 0; imgId < (int)testX.size(); imgId++) {
		int len = testX[0]->getLen();
		for (int i = 0; i < len; i++) {
			testX[imgId]->getHost()[i] -= aver->getHost()[i];
		}
	}

	delete aver;
}
