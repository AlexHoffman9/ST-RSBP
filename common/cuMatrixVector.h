#ifndef __CU_MATRIX_VECTOR_H_
#define __CU_MATRIX_VECTOR_H_


#include <vector>
#include "cuMatrix.h"
#include "cuda_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MemoryMonitor.h"

using namespace std;

template <class T>
class cuMatrixVector
{
public:
	cuMatrixVector(): m_hstPoint(0), m_devPoint(0){}
	~cuMatrixVector(){
		MemoryMonitor::instance()->freeCpuMemory(m_hstPoint);
		MemoryMonitor::instance()->freeGpuMemory(m_devPoint);
		m_vec.clear();
	}
	cuMatrix<T>* operator[](size_t index){
		if(index >= m_vec.size()){
			printf("cuMatrix Vector operator[] error: try to access %zu_th element, but the vector has %zu elements\n", index, m_vec.size());
			exit(0);
		}
		return m_vec[index];
	}
	size_t size(){return m_vec.size();}
    
    bool empty(){return m_vec.empty();}

	void push_back(cuMatrix<T>* m){m_vec.push_back(m);}

	void toGpu()
	{
		cudaError_t cudaStat;

		m_hstPoint = (T**)MemoryMonitor::instance()->cpuMalloc(m_vec.size() * sizeof(T*));
		if(!m_hstPoint){
			printf("cuMatrixVector<T> malloc m_hstPoint fail\n");
			exit(0);
		}

		cudaStat = MemoryMonitor::instance()->gpuMalloc((void**)&m_devPoint, sizeof(T*) * m_vec.size());
		if(cudaStat != cudaSuccess){
			printf("cuMatrixVector<T> cudaMalloc m_devPoint fail\n");
			exit(0);
		}

		for(int p = 0; p < (int)m_vec.size(); p++){
			m_hstPoint[p] = m_vec[p]->getDev();
		}

		cudaStat = cudaMemcpy(m_devPoint, m_hstPoint, sizeof(T*) * m_vec.size(), cudaMemcpyHostToDevice);
		if(cudaStat != cudaSuccess){
			printf("cuMatrixVector::toGpu cudaMemcpy w fail\n");
			exit(0);
		}
	}


	void shuffle(int times, cuMatrix<int>*&labels, cuMatrix<float>*&weights){
		cudaError_t cudaStat;

		for(int i = 0; i < times; i++){
			int x = rand() % m_vec.size();
			int y = rand() % m_vec.size();
			swap(m_vec[x], m_vec[y]);
			if(m_hstPoint)
				swap(m_hstPoint[x], m_hstPoint[y]);
			swap(labels->getHost()[x], labels->getHost()[y]);
            swap(weights->getHost()[x], weights->getHost()[y]);
		}
		if(NULL != m_hstPoint && NULL != m_devPoint)
		{
			cudaStat = cudaMemcpy(m_devPoint, m_hstPoint, sizeof(T*) * m_vec.size(), cudaMemcpyHostToDevice);
			if(cudaStat != cudaSuccess){
				printf("shuffle: cudaMemcpy w fail\n");
				exit(0);
			}
		}

		labels->toGpu();
        weights->toGpu();
	}

	vector<cuMatrix<T>*>m_vec;
	T** m_hstPoint;
	T** m_devPoint;
};
#endif
