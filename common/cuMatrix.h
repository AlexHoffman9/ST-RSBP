#ifndef _CU_MATRIX_H_
#define _CU_MATRIX_H_

#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <assert.h>
#include "helper_cuda.h"
#include "MemoryMonitor.h"

/*rows-major*/
template <class T>
class cuMatrix
{
public:
	/*constructed function with hostData*/
	cuMatrix(T *_data, int _n,int _m, int _c):rows(_n), cols(_m), channels(_c), hostData(NULL), devData(NULL), spikeTimes(NULL), rawImg(NULL){
		/*malloc host data*/
		mallocHost();
		/*deep copy */
		memcpy(hostData, _data, sizeof(*hostData) * cols * rows * channels);
	}
	
	/*constructed function with rows and cols*/
	cuMatrix(int _n,int _m, int _c):rows(_n), cols(_m), channels(_c), hostData(NULL), devData(NULL), spikeTimes(NULL), rawImg(NULL){
	}

    /*constructed function with spike times */
    cuMatrix(int _n, int _m, int _c, std::vector<std::vector<int> > * sp_time):rows(_n), cols(_m), channels(_c), hostData(NULL), devData(NULL), spikeTimes(sp_time), rawImg(NULL){
    }

    /*constructd function with spike times and raw image*/
    cuMatrix(int _n, int _m, int _c, std::vector<std::vector<int> > * sp_time, cuMatrix<float>* raw_img):rows(_n), cols(_m), channels(_c), hostData(NULL), devData(NULL), spikeTimes(sp_time), rawImg(raw_img){
    }

	/*free cuda memory*/
	void freeCudaMem(){
		if(NULL != devData){
			MemoryMonitor::instance()->freeGpuMemory(devData);
			devData = NULL;
		}
	}

    /*free cpu memory*/
    void freeCpuMem(){
        if(NULL != hostData){
            MemoryMonitor::instance()->freeCpuMemory(hostData);
            hostData = NULL;
        }
    }

	/*destruction function*/
	~cuMatrix(){
		if(NULL != hostData)
			MemoryMonitor::instance()->freeCpuMemory(hostData);
		if(NULL != devData)
			MemoryMonitor::instance()->freeGpuMemory(devData);
        if(NULL != spikeTimes)
            delete spikeTimes;
        if(NULL != rawImg)
            delete rawImg;
	}

    /* convert the spikes times (sparse) to the dense binary response matrix */
    void sparseToDense(){
        if(spikeTimes == NULL)  return;
        assert(hostData == NULL);
        mallocHost();
        for(int i = 0; i < spikeTimes->size(); ++i){
            for(int j = 0; j < (*spikeTimes)[i].size(); ++j){
                int time = (*spikeTimes)[i][j];
                if(time < rows && i < cols)
    		        hostData[time * cols + i] = true;
            }
        }
    }

    /* return the host ptr to the raw img */
    float *& getHostRawImg(){
        if(rawImg == NULL){
            printf("Fatal Error::Try to use the rawImg ptr, but it is not properly set!\n");
            assert(0);
        }
        return rawImg->getHost();
    }

    /* return the spike times*/
    std::vector<std::vector<int> > *& getSpikeTimes(){
        return spikeTimes;    
    }

	/*copy the device data to host data*/ 
	void toCpu(){
		cudaError_t cudaStat;
		mallocDev();
		mallocHost();
		cudaStat = cudaMemcpy (hostData, devData, sizeof(*devData) * cols * rows * channels, cudaMemcpyDeviceToHost);

		if(cudaStat != cudaSuccess) {
			printf("cuMatrix::toCPU data download failed\n");
			MemoryMonitor::instance()->freeGpuMemory(devData);
			exit(0);
		} 
	}

	/*copy the host data to device data*/
	void toGpu(){
		cudaError_t cudaStat;
		mallocDev();
		mallocHost();
		cudaStat = cudaMemcpy (devData, hostData, sizeof(*devData) * cols * rows * channels, cudaMemcpyHostToDevice);

		if(cudaStat != cudaSuccess) {
			printf("cuMatrix::toGPU data upload failed\n");
			MemoryMonitor::instance()->freeGpuMemory(devData);
			exit(0);
		}
	}

	/*copy the host data to device data with cuda-streams*/
	void toGpu(cudaStream_t stream1){
		mallocDev();
		checkCudaErrors(cudaMemcpyAsync(devData, hostData, sizeof(*devData) * cols * rows * channels, cudaMemcpyHostToDevice, stream1));
	}
	
	/*set all device memory to be zeros*/
	void gpuClear(){
		mallocDev();
		cudaError_t cudaStat;
		cudaStat = cudaMemset(devData,0,sizeof(*devData) * cols * rows * channels);
		if(cudaStat != cudaSuccess) {
			printf("device memory cudaMemset failed\n");
			exit(0);
		}
	}

	void cpuClear(){
		mallocHost();
		memset(hostData, 0, cols * rows * channels * sizeof(*hostData));
	}

	/*set  value*/
	void set(int i, int j, int k, T v){
		mallocHost();
		hostData[(i * cols + j) + cols * rows * k] = v;
	}

	/*get value*/
	T get(int i, int j, int k){
		mallocHost();
		return hostData[(i * cols + j) + cols * rows * k];
	}

	/*get the number of values*/
	int getLen(){
		return rows * cols * channels;
	}

	/*get rows * cols*/
	int getArea(){
		return rows * cols;
	}

	T  *& getHost(){
		mallocHost();
		return hostData;
	}

	T  *& getDev(){
		mallocDev();
		return devData;
	}

	/*column*/
	int cols;

	/*row*/
	int rows;

	/*channels*/
	int channels;
private:
	/*host data*/
	T *hostData;

	/*device data*/
	T *devData;

    /* spike time data*/
    std::vector<std::vector<int> > * spikeTimes;

    /* raw image before distortion*/
    cuMatrix<float>* rawImg;
private:
	void mallocHost(){
		if(NULL == hostData){
			/*malloc host data*/
			hostData = (T*)MemoryMonitor::instance()->cpuMalloc(cols * rows * channels * sizeof(*hostData));
			if(!hostData) {
				printf("cuMatrix:cuMatrix host memory allocation failed\n");
				exit(0);
			}
			memset(hostData, 0, cols * rows * channels * sizeof(*hostData));
		}
	}
	void mallocDev(){
		if(NULL == devData){
			cudaError_t cudaStat;
			/*malloc device data*/
			cudaStat = MemoryMonitor::instance()->gpuMalloc((void**)&devData, cols * rows * channels * sizeof(*devData));
			if(cudaStat != cudaSuccess) {
				printf("cuMatrix::cuMatrix device memory allocation failed\n");
				exit(0);
			}

			cudaStat = cudaMemset(devData, 0, sizeof(*devData) * cols * rows * channels);
			if(cudaStat != cudaSuccess) {
				printf("cuMatrix::cuMatrix device memory cudaMemset failed\n");
				exit(0);
			}
		}
	}
};


/*matrix multiply*/
/*z = x * y*/
void matrixMul   (cuMatrix<float>* x, cuMatrix<float>*y, cuMatrix<float>*z);
/*z = T(x) * y*/
void matrixMulTA (cuMatrix<float>* x, cuMatrix<float>*y, cuMatrix<float>*z);
/*z = x * T(y)*/
void matrixMulTB (cuMatrix<float>* x, cuMatrix<float>*y, cuMatrix<float>*z);
#endif
