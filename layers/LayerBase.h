#ifndef __LAYERS_BASE_CU_H__
#define __LAYERS_BASE_CU_H__

#include "../common/cuMatrix.h"
#include "../common/cuMatrixVector.h"
#include "../common/util.h"

class LayerBase
{
public:
	LayerBase():cost(new cuMatrix<float>(1, 1, 1)){}
	virtual void feedforward() = 0;
	virtual void backpropagation() = 0;
	virtual void getGrad() = 0;
	virtual void updateWeight(int epoch) = 0;
	virtual void clearMomentum() = 0;
	virtual void save(FILE* file) = 0;
	virtual void initFromCheckpoint(FILE* file) = 0;
	virtual void calCost() = 0;

	virtual cuMatrix<float>* getOutputs() = 0;
	virtual cuMatrix<float>* getCurDelta() = 0;
    virtual cuMatrix<int>* getFireCount() = 0;

	virtual void printParameter() = 0;

	float getCost(){
		if(cost != NULL){
			cost->toCpu();
			return cost->get(0, 0, 0);
		}
		return 0.0;
	}
	void printCost(){
		if(cost != NULL){
			cost->toCpu();
			char logStr[1024];
			sprintf(logStr, "%10s cost = %f\n", m_name.c_str(), cost->get(0, 0, 0));
			LOG(logStr, "Result/log.txt");
		}
	}
	~LayerBase(){
		delete cost;
	}
	std::string m_name;
	std::vector<std::string> m_preLayer;
	cuMatrix<float>* cost;
};


class SpikingLayerBase: public LayerBase
{
public:
    virtual cuMatrix<bool>* getSpikingOutputs() = 0;
    virtual cuMatrix<int>*  getSpikingTimeOutputs() = 0;
	virtual void printFireCount() = 0;
    virtual void setPredict(int * p) = 0;
    virtual void setSampleWeight(float* s_weights) = 0;
    virtual void verify(const std::string& phrase) = 0;
	int inputDim;
	int outputDim;
    int endTime;
	int inputAmount;
	int outputAmount;
};

class Layers
{
public:
	static Layers* instance(){
		static Layers* layers= new Layers();
		return layers;
	}
	LayerBase* get(std::string name);
	void set(std::string name, LayerBase* layer);
    Layers(){
      checkCudaErrors(cudaStreamCreate(&stream_update_weight));  
    } 
    cudaStream_t& get_stream(){
        return stream_update_weight;
    }
private:
    cudaStream_t stream_update_weight;
	std::map<std::string, LayerBase*>m_maps;
};
#endif
