#ifndef ANN_OQ_H
#define ANN_OQ_H

#include <tiny_dnn/tiny_dnn.h>

class AnnOq
{
    public:
        AnnOq();

        void trainDataset(const std::string& dirPath);


    private:
        tiny_dnn::network<tiny_dnn::sequential> net;
};


#endif // ANN_OQ_H
