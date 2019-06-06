#include "Config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;


bool Config::get_word_bool(string &str, string name){
    size_t pos = str.find(name+"=");
    if(pos == string::npos){
        cout<<"Warning::No key word: "<<name<<" found in the configuration file"<<endl;
        return false;
    }
    int i = pos + 1;
    bool res = true;
    while(1){
        if(i == str.length()) break;
        if(str[i] == ';') break;
        ++ i;
    }
    string sub = str.substr(pos, i - pos + 1);
    if(sub[sub.length() - 1] == ';'){
        string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
        if(!content.compare("true")) res = true;
        else res = false;
    }
    str.erase(pos, i - pos + 1);
    return res;
}

int Config::get_word_int(string &str, string name){
    size_t pos = str.find(name);    
    if(pos == string::npos){
        cout<<"Warning::No key word: "<<name<<" found in the configuration file"<<endl;
        return -1;
    }
    int i = pos + 1;
    int res = 1;
    while(1){
        if(i == str.length()) break;
        if(str[i] == ';') break;
        ++ i;
    }
    string sub = str.substr(pos, i - pos + 1);
    if(sub[sub.length() - 1] == ';'){
        string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
        res = atoi(content.c_str());
    }
    str.erase(pos, i - pos + 1);
    return res;
}

float Config::get_word_float(string &str, string name){
    size_t pos = str.find(name+"=");  
    if(pos == string::npos){
        cout<<"Warning::No key word: "<<name<<" found in the configuration file"<<endl;
        return 0.0f;
    }
    int i = pos + 1;
    float res = 0.0f;
    while(1){
        if(i == str.length()) break;
        if(str[i] == ';') break;
        ++ i;
    }
    string sub = str.substr(pos, i - pos + 1);
    if(sub[sub.length() - 1] == ';'){
        string content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
        res = atof(content.c_str());
    }
    str.erase(pos, i - pos + 1);
    return res;
}

string Config::get_word_type(string &str, string name){
    size_t pos = str.find(name+"=");    
    if(pos == str.npos){
        return "NULL";
    }

    int i = pos + 1;
    int res = 0;
    while(1){
        if(i == str.length()) break;
        if(str[i] == ';') break;
        ++ i;
    }
    string sub = str.substr(pos, i - pos + 1);
    string content;
    if(sub[sub.length() - 1] == ';'){
        content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
    }
    str.erase(pos, i - pos + 1);
    return content;
}

std::vector<string> Config::get_name_vector(string &str, string name){
    std::vector<std::string>result;

    size_t pos = str.find(name+"=");    
    if(pos == str.npos){
        return result;
    }

    int i = pos + 1;
    int res = 0;
    while(1){
        if(i == str.length()) break;
        if(str[i] == ';') break;
        ++ i;
    }
    string sub = str.substr(pos, i - pos + 1);
    string content;
    if(sub[sub.length() - 1] == ';'){
        content = sub.substr(name.length() + 1, sub.length() - name.length() - 2);
    }
    str.erase(pos, i - pos + 1);

    while(content.size()){
        size_t pos = content.find(',');
        if(pos == str.npos){
            result.push_back(content);
            break;
        }else{
            result.push_back(content.substr(0, pos));
            content.erase(0, pos + 1);
        }
    }

    return result;
}


void Config:: get_layers_config(string &str){
    vector<string> layers;
    if(str.empty()) return;
    int head = 0;
    int tail = 0;
    while(1){
        if(head == str.length()) break;
        if(str[head] == '['){
            tail = head + 1;
            while(1){
                if(tail == str.length()) break;
                if(str[tail] == ']') break;
                ++ tail;
            }
            string sub = str.substr(head, tail - head + 1);
            if(sub[sub.length() - 1] == ']'){
                sub.erase(sub.begin() + sub.length() - 1);
                sub.erase(sub.begin());
                layers.push_back(sub);
            }
            str.erase(head, tail - head + 1);
        }else ++ head;
    }
    for(int i = 0; i < layers.size(); i++){
        string type = get_word_type(layers[i], "LAYER");
        std::string name = get_word_type(layers[i], "NAME");
        std::string input = get_word_type(layers[i], "INPUT");
        std::string subInput = get_word_type(layers[i], "SUBINPUT");

        ConfigBase* layer;
        if(type == std::string("DATASPIKING")){
            int input_neurons = get_word_int(layers[i], "NUM_NEURONS");
            layer = new ConfigDataSpiking(name, type, input_neurons);
            char logStr[256];
            sprintf(logStr, "\n\n********data spiking Layer********\n"); LOG(logStr, "Result/log.txt");
            sprintf(logStr, "NAME          : %s\n", name.c_str()); LOG(logStr, "Result/log.txt");
            sprintf(logStr, "INPUT_NEURONS : %d\n", input_neurons); LOG(logStr, "Result/log.txt");
        }
        else if(type == std::string("SPIKING")){ 
            int num_classes = get_word_int(layers[i], "NUM_CLASSES");

            int num_neurons = get_word_int(layers[i], "NUM_NEURONS");
            float vth = get_word_float(layers[i], "VTH");
            int t_ref = get_word_int(layers[i], "T_REFRAC");
            float tau_m = get_word_float(layers[i], "TAU_M");
            float tau_s = get_word_float(layers[i], "TAU_S");
            float initW= get_word_float(layers[i], "initW");
            std::string weight_connect_str = get_word_type(layers[i], "weightConnect");
            int weight_connect = weight_connect_str == std::string("NULL") ? 0 : atoi(weight_connect_str.c_str());
            std::string initType = get_word_type(layers[i], "initType");
            std::string weight_path = get_word_type(layers[i], "weightPath");
            std::string lweight_path = get_word_type(layers[i], "lweightPath");
            std::string laterial_type = get_word_type(layers[i], "laterialType");
            std::string reservoir_dim = get_word_type(layers[i], "reservoirDim");
            std::string local_inb_strength_str = get_word_type(layers[i], "localInbStrength");
            float local_inb_strength = local_inb_strength_str == std::string("NULL") ? 0 : atof(local_inb_strength_str.c_str());
            float undesired_level = 0, desired_level = 0, margin = 0;
            undesired_level = get_word_float(layers[i], "UNDESIRED_LEVEL");
            desired_level = get_word_float(layers[i], "DESIRED_LEVEL");
            margin = get_word_float(layers[i], "MARGIN");    
			float selfLoopStrength = get_word_float(layers[i], "selfLoopStrength");
			float selfLoopRatio = get_word_float(layers[i], "selfLoopRatio");
			float lrate=get_word_float(layers[i], "LearningRate");
        
            std::map<std::string, std::string> ref_paths;
            ref_paths[std::string("refWeightPath")] = get_word_type(layers[i], "refWeightPath");
            ref_paths[std::string("refLWeightPath")] = get_word_type(layers[i], "refLWeightPath");
            ref_paths[std::string("refOutputTrainPath")] = get_word_type(layers[i], "refOutputTrainPath");
            ref_paths[std::string("refOutputTestPath")] = get_word_type(layers[i], "refOutputTestPath");
            bool has_bias = get_word_bool(layers[i], "ADD_BIAS");
            string df = get_word_type(layers[i], "BIAS_FREQ");
            int dummy_freq = df == string("NULL") ? 100000 : atoi(df.c_str());
            layer = new ConfigSpiking(name, type, input, num_classes, num_neurons,
                         vth, t_ref, tau_m, tau_s, 
                         initW, weight_connect, initType, weight_path,
                         lweight_path, laterial_type, reservoir_dim, local_inb_strength,
                         undesired_level, desired_level, margin, selfLoopStrength, selfLoopRatio, lrate, ref_paths, has_bias, dummy_freq);
            m_classes = num_classes;
            char logStr[256];
            sprintf(logStr, "\n\n********%s Layer********\n", type.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "NAME               : %s\n", name.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "NUM_NEURONS        : %d\n", num_neurons);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "INPUT              : %s\n", input.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "VTH                : %f\n", vth);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "T_REFRAC           : %d\n", t_ref);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "TAU_M              : %f\n", tau_m);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "TAU_S              : %f\n", tau_s);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "initW              : %f\n", initW);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "weightConnect      : %d\n", weight_connect);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "initType           : %s\n", initType.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "weightPath         : %s\n", weight_path.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "lweightPath        : %s\n", lweight_path.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "laterialType       : %s\n", laterial_type.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "reservoirDim       : %s\n", reservoir_dim.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "localInbStrength   : %f\n", local_inb_strength);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "UNDESIRED_LEVEL    : %f\n", undesired_level);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "DESIRED_LEVEL      : %f\n", desired_level);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "MARGIN             : %f\n", margin);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "selfLoopStrength	: %f\n", selfLoopStrength);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "selfLoopRatio	    : %f\n", selfLoopRatio);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "LearingRate        : %f\n", lrate);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refWeightPath      : %s\n", ref_paths[std::string("refWeightPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refLWeightPath     : %s\n", ref_paths[std::string("refLWeightPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refOuputTrainPath  : %s\n", ref_paths[std::string("refOutputTrainPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refOutputTestPath  : %s\n", ref_paths[std::string("refOutputTestPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "ADD_BIAS           : %d\n", has_bias);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "BIAS_FREQ          : %d\n", dummy_freq);LOG(logStr, "Result/log.txt");
        }
        else if(type == std::string("RESERVOIR")){ 
            int num_classes = get_word_int(layers[i], "NUM_CLASSES");

            int num_neurons = get_word_int(layers[i], "NUM_NEURONS");
            float vth = get_word_float(layers[i], "VTH");
            int t_ref = get_word_int(layers[i], "T_REFRAC");
            float tau_m = get_word_float(layers[i], "TAU_M");
            float tau_s = get_word_float(layers[i], "TAU_S");
            float initW= get_word_float(layers[i], "initW");
            bool train_reservoir = get_word_bool(layers[i], "Train_Reservoir");
            std::string weight_connect_str = get_word_type(layers[i], "weightConnect");
            int weight_connect = weight_connect_str == std::string("NULL") ? 0 : atoi(weight_connect_str.c_str());
            std::string initType = get_word_type(layers[i], "initType");
            std::string weight_path = get_word_type(layers[i], "weightPath");
            std::string lweight_path = get_word_type(layers[i], "lweightPath");
            std::string laterial_type = get_word_type(layers[i], "laterialType");
            std::string reservoir_dim = get_word_type(layers[i], "reservoirDim");
            std::string local_inb_strength_str = get_word_type(layers[i], "localInbStrength");
            float local_inb_strength = local_inb_strength_str == std::string("NULL") ? 0 : atof(local_inb_strength_str.c_str());
            float undesired_level = 0, desired_level = 0, margin = 0;
            undesired_level = get_word_float(layers[i], "UNDESIRED_LEVEL");
            desired_level = get_word_float(layers[i], "DESIRED_LEVEL");
            margin = get_word_float(layers[i], "MARGIN");    
			float selfLoopStrength = get_word_float(layers[i], "selfLoopStrength");
			float selfLoopRatio = get_word_float(layers[i], "selfLoopRatio");
			float lrate=get_word_float(layers[i], "LearningRate");
        
            std::map<std::string, std::string> ref_paths;
            ref_paths[std::string("refWeightPath")] = get_word_type(layers[i], "refWeightPath");
            ref_paths[std::string("refLWeightPath")] = get_word_type(layers[i], "refLWeightPath");
            ref_paths[std::string("refOutputTrainPath")] = get_word_type(layers[i], "refOutputTrainPath");
            ref_paths[std::string("refOutputTestPath")] = get_word_type(layers[i], "refOutputTestPath");
            bool has_bias = get_word_bool(layers[i], "ADD_BIAS");
            string df = get_word_type(layers[i], "BIAS_FREQ");
            int dummy_freq = df == string("NULL") ? 100000 : atoi(df.c_str());
            layer = new ConfigReservoir(name, type, input, num_classes, num_neurons,
                         vth, t_ref, tau_m, tau_s, 
                         initW, train_reservoir, weight_connect, initType, weight_path,
                         lweight_path, laterial_type, reservoir_dim, local_inb_strength,
                         undesired_level, desired_level, margin, selfLoopStrength, selfLoopRatio,lrate, ref_paths, has_bias, dummy_freq);
            m_classes = num_classes;
            char logStr[256];
            sprintf(logStr, "\n\n********%s Layer********\n", type.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "NAME               : %s\n", name.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "NUM_NEURONS        : %d\n", num_neurons);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "INPUT              : %s\n", input.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "VTH                : %f\n", vth);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "T_REFRAC           : %d\n", t_ref);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "TAU_M              : %f\n", tau_m);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "TAU_S              : %f\n", tau_s);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "initW              : %f\n", initW);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "Train_Reservoir    : %d\n", train_reservoir);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "weightConnect      : %d\n", weight_connect);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "initType           : %s\n", initType.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "weightPath         : %s\n", weight_path.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "lweightPath        : %s\n", lweight_path.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "laterialType       : %s\n", laterial_type.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "reservoirDim       : %s\n", reservoir_dim.c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "localInbStrength   : %f\n", local_inb_strength);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "UNDESIRED_LEVEL    : %f\n", undesired_level);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "DESIRED_LEVEL      : %f\n", desired_level);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "MARGIN             : %f\n", margin);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "selfLoopStrength	: %f\n", selfLoopStrength);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "selfLoopRatio	    : %f\n", selfLoopRatio);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "LearingRate        : %f\n", lrate);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refWeightPath      : %s\n", ref_paths[std::string("refWeightPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refLWeightPath     : %s\n", ref_paths[std::string("refLWeightPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refOuputTrainPath  : %s\n", ref_paths[std::string("refOutputTrainPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "refOutputTestPath  : %s\n", ref_paths[std::string("refOutputTestPath")].c_str());LOG(logStr, "Result/log.txt");
            sprintf(logStr, "ADD_BIAS           : %d\n", has_bias);LOG(logStr, "Result/log.txt");
            sprintf(logStr, "BIAS_FREQ          : %d\n", dummy_freq);LOG(logStr, "Result/log.txt");
        }

        insertLayerByName(name, layer);
        if(type == std::string("DATASPIKING")){
            m_firstLayers.push_back(layer);
        }
        else{
            ConfigBase* preLayer = getLayerByName(layer->m_input);
            preLayer->m_next.push_back(layer);
        }
    }
}

void Config::init(std::string path)
{
    char logStr[256];
    sprintf(logStr, "\n\n*******************CONFIG*******************\n");
    LOG(logStr, "Result/log.txt");

    /*read the string from file "Config.txt"*/
    /*delete the comment and spaces*/
    m_configStr = read_2_string(path);
    deleteComment();
    deleteSpace();

    /*IS_GRADIENT_CHECKING*/
    bool is_gradient_checking = get_word_bool(m_configStr, "IS_GRADIENT_CHECKING");
    m_isGradientChecking = new ConfigGradient(is_gradient_checking);
    sprintf(logStr, "Is Gradient Checking : %d\n", is_gradient_checking);
    LOG(logStr, "Result/log.txt");
    
    /*BOOST_METHOD*/
    bool has_boost_weight = get_word_bool(m_configStr, "BOOST_METHOD");
    m_hasBoostWeightTrain = new ConfigBoostWeight(has_boost_weight);
    sprintf(logStr, "Has Boost Weight Train: %d\n", has_boost_weight);
    LOG(logStr, "Result/log.txt");

    /*EFFECT RATIO*/
    bool use_effect_ratio = get_word_bool(m_configStr, "EFFECT_RATIO");
    m_useEffectRatio = new ConfigEffectRatio(use_effect_ratio);
    sprintf(logStr, "Use Effect Ratio      : %d\n", use_effect_ratio);
    LOG(logStr, "Result/log.txt");

    /*OPTIMIZER*/
    std::string optim_t = get_word_type(m_configStr, "OPTIMIZER");
    optim_t = optim_t == std::string("NULL") ? std::string("sgd") : optim_t;
    m_optimizer = new ConfigOptimizer(optim_t);
    sprintf(logStr, "Use Optimizer         : %s\n", optim_t.c_str());
    LOG(logStr, "Result/log.txt");

    /*weight regularization*/
    float lambda_reg = get_word_float(m_configStr, "LAMBDA_REG");
    float beta_reg = get_word_float(m_configStr, "BETA_REG");
    m_weightReg = new ConfigWeightReg(lambda_reg, beta_reg);
    sprintf(logStr, "lambda                : %f\n", lambda_reg);
    LOG(logStr, "Result/log.txt");
    sprintf(logStr, "beta                  : %f\n", beta_reg);
    LOG(logStr, "Result/log.txt");

    /*weight limit*/
    std::string wl = get_word_type(m_configStr, "WEIGHT_LIMIT");
    float weight_limit = wl == std::string("NULL") ? 8.0f : atof(wl.c_str());
    m_weightLimit = new ConfigWeightLimit(weight_limit);
    sprintf(logStr, "weight limit          : %f\n", weight_limit);
    LOG(logStr, "Result/log.txt");

    /*BATCH_SIZE*/
    int batch_size = get_word_int(m_configStr, "BATCH_SIZE");
    m_batchSize = new ConfigBatchSize(batch_size);
    sprintf(logStr, "batch Size            : %d\n", batch_size);
    LOG(logStr, "Result/log.txt");

    /*CHANNELS*/
    int channels = get_word_int(m_configStr, "CHANNELS");
    m_channels = new ConfigChannels(channels);
    sprintf(logStr, "channels              : %d\n", channels);
    LOG(logStr, "Result/log.txt");

    /*crop*/
    int crop = get_word_int(m_configStr, "CROP");
    m_crop = new ConfigCrop(crop);
    sprintf(logStr, "crop                  : %d\n", crop);
    LOG(logStr, "Result/log.txt");

    /*scale*/
    float scale = get_word_float(m_configStr, "SCALE");
    m_scale = new ConfigScale(scale);
    sprintf(logStr, "scale                 : %f\n", scale);
    LOG(logStr, "Result/log.txt");

    /*rotation*/
    float rotation = get_word_float(m_configStr, "ROTATION");
    m_rotation = new ConfigRotation(rotation);
    sprintf(logStr, "rotation              : %f\n", rotation);
    LOG(logStr, "Result/log.txt");

    /*distortion*/
    float distortion = get_word_float(m_configStr, "DISTORTION");
    m_distortion = new ConfigDistortion(distortion);
    sprintf(logStr, "distortion            : %f\n", distortion);
    LOG(logStr, "Result/log.txt");

    /*ImageShow*/
    bool imageShow = get_word_bool(m_configStr, "SHOWIMAGE");
    m_imageShow = new ConfigImageShow(imageShow);
    sprintf(logStr, "imageShow             : %d\n", imageShow);
    LOG(logStr, "Result/log.txt");

    /*Horizontal*/
    bool horizontal = get_word_bool(m_configStr, "HORIZONTAL");
    m_horizontal = new ConfigHorizontal(horizontal);
    sprintf(logStr, "HORIZONTAL            : %d\n", horizontal);
    LOG(logStr, "Result/log.txt");

    /*Test Epoch*/
    int test_epoch = get_word_int(m_configStr, "TEST_EPOCH");
    m_test_epoch = new ConfigTestEpoch(test_epoch);
    sprintf(logStr, "Test_Epoch            : %d\n", test_epoch);
    LOG(logStr, "Result/log.txt");

    /*WHITE_NOISE*/
    float stdev = get_word_float(m_configStr, "WHITE_NOISE");
    m_white_noise = new ConfigWhiteNoise(stdev);
    sprintf(logStr, "White Noise           : %f\n", stdev);
    LOG(logStr, "Result/log.txt");

    /*END_TIME*/
    int end_time = get_word_int(m_configStr, "END_TIME");
    m_endTime = end_time;
    sprintf(logStr, "Spike end time        : %d\n", end_time);
    LOG(logStr, "Result/log.txt");

    /*DATASET*/
    std::string train_path = get_word_type(m_configStr, "TRAIN_DATA_PATH");
    std::string test_path = get_word_type(m_configStr, "TEST_DATA_PATH");
    int train_samples = get_word_int(m_configStr, "TRAIN_SAMPLES");
    int test_samples  = get_word_int(m_configStr, "TEST_SAMPLES");
    int train_per_class = get_word_int(m_configStr, "TRAIN_PER_CLASS");
    int test_per_class = get_word_int(m_configStr, "TEST_PER_CLASS");
    m_dataset = new ConfigDataset(train_path, test_path, train_samples, test_samples, train_per_class, test_per_class);
    sprintf(logStr, "Train data path       : %s\n", train_path.c_str());
    LOG(logStr, "Result/log.txt");
    sprintf(logStr, "Test data path        : %s\n", test_path.c_str());
    LOG(logStr, "Result/log.txt");
    sprintf(logStr, "Train samples         : %d\n", train_samples);
    LOG(logStr, "Result/log.txt");
    sprintf(logStr, "Test samples          : %d\n", test_samples);
    LOG(logStr, "Result/log.txt");
    sprintf(logStr, "Train samples per class : %d\n", train_per_class);
    LOG(logStr, "Result/log.txt");
    sprintf(logStr, "Test samples per class  : %d\n", test_per_class);
    LOG(logStr, "Result/log.txt");
  

    /*Layers*/
    get_layers_config(m_configStr);
    sprintf(logStr, "\n\n\n");
    LOG(logStr, "Result/log.txt");
}

void Config::deleteSpace() {
    if (m_configStr.empty())
        return;
    size_t pos1, pos2,e,t,n;
    while (1) {
        e = m_configStr.find(' ');
        t = m_configStr.find('\t');
        n = m_configStr.find('\n');
        if(e==std::string::npos && n==std::string::npos && t==std::string::npos)break;
        if(e<t || t ==std::string::npos)pos1 = e;
        else pos1 = t;
        if(n < pos1 || pos1 ==std::string::npos)pos1 = n;
        for (pos2 = pos1 + 1; pos2 < m_configStr.size(); pos2++) {
            if (!(m_configStr[pos2] == '\t' || m_configStr[pos2] == '\n'
                        || m_configStr[pos2] == ' '))
                break;
        }
        m_configStr.erase(pos1, pos2 - pos1);
    }

}

void Config::deleteComment() {
    size_t pos1, pos2;
    while (1) {
        pos1 = m_configStr.find("#");
        if (pos1 == std::string::npos)
            break;
        for (pos2 = pos1 + 1; pos2 < m_configStr.size(); pos2++) {
            if (m_configStr[pos2] == '#')
                break;
        }
        m_configStr.erase(pos1, pos2 - pos1 + 1);
    }
}

string 
Config::read_2_string(string File_name){
    char *pBuf;
    FILE *pFile = NULL;   
    char logStr[1025];
    if(!(pFile = fopen(File_name.c_str(),"r"))){
        sprintf(logStr, "Cannot find the Config file: %s!", File_name.c_str());
        LOG(logStr, "Result/log.txt");
        exit(EXIT_FAILURE);
    }
    //move pointer to the end of the file
    fseek(pFile, 0, SEEK_END);
    //Gets the current position of a file pointer.offset 
    size_t len = ftell(pFile);
    pBuf = new char[len];
    //Repositions the file pointer to the beginning of a file
    rewind(pFile);
    if(fread(pBuf, 1, len, pFile) != len){
        LOG("fread fail", "result/log.txt");
    }
    fclose(pFile);
    string res = pBuf;
    return res;
}
