#pragma once 

#include <vector>

class Interpolation {
public:
    enum Emode{
        NONE,
        LINEAR,
        CUBIC
    };

protected:
    Emode mode;
    std::vector<float> *tablePointer; ///< a pointer to an external wave table. 
    int tableSize;
public:
    Interpolation(std::vector<float> *tbp, Emode md = LINEAR): tablePointer(tbp), mode(md){
        tableSize = tablePointer->size();
        switch(mode){
            case LINEAR:
                tableSize -= 1;
                break;
            case CUBIC:
                tableSize -= 2;
                break;
            case NONE:
                break;
            default:
                break;
        }
    };

    /**
     * @brief calculate interpolation between two values x[n] and x[n+1] in a vector
     * @details if the mode is LINEAR one extra sample should be added at the end of the vector
     * if the mode is CUBIC two extra sample should be added at the begining and end of the vector.
     * for the preparation of the table, use static functions, prepareLinear and prepareCubic.
     * 
     * @param tbp pointer to the table
     * @param index of the array
     * @param weight weight between x[n] and  x[n+1]
     * @return interpolated value
     */
        float interpolate(float phase);
        static void prepareLinear(std::vector<float> *table);
        static void prepareCubic(std::vector<float> *table);	

    
};

inline float Interpolation::interpolate(float phase){
    float value = 0.0;
    switch(mode){
        case LINEAR:{
            float findex = static_cast<float>(tableSize) * (phase/CYCLE); // calc accurate position scale 0 - 1
            float truncatedIndex = floor(findex);
            int index = static_cast<int>(truncatedIndex);
            float weight = findex - truncatedIndex; // get fractional part
            value = ((*tablePointer)[index+1] - (*tablePointer)[index]) * weight + (*tablePointer)[index]; //
            break;
        }case CUBIC:{
            // to be implemented


            break;
        }case NONE:{

            break;
        }default:{
            std::cout << "Interpolation: error. invalid mode";
            break;
        }
    }
    return value;
}

inline void Interpolation::prepareLinear(std::vector<float> *table){
    float first = (*table)[0];
    table->push_back(first);
}

inline void Interpolation::prepareCubic(std::vector<float> *table){
    //to be implemented
    float first = (*table)[0];
    float second = (*table)[1];
    table->push_back(first);
    table->push_back(second);
}

