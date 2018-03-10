#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <cstdint>

class BranchPredictor {
    protected:
        uint32_t memoryLimit;
        uint32_t numberOfBranch;
        uint32_t numberOfMisprediction;
    public:
        BranchPredictor(uint32_t);
        void printResult();
};

class BimodelPredictor : public BranchPredictor {
    private:
        uint32_t predictionTableAddrBits;
        uint32_t pcMask;
        uint32_t *predictionTable;
        bool makePrediction(uint32_t);
        void trainPredictor(uint32_t, bool);
    public:
        BimodelPredictor(uint32_t);
        ~BimodelPredictor();
        void runPredictor();
};

class LocalHistPredictor : public BranchPredictor {
    private:
        uint32_t predictionTableAddrBits;
        uint32_t pcMask;
        uint32_t *localHistTable;
        uint32_t *predictionTable;
        bool makePrediction(uint32_t);
        void trainPredictor(uint32_t, bool);
    public:
        LocalHistPredictor(uint32_t);
        ~LocalHistPredictor();
        void runPredictor();
};

class GsharePredictor : public BranchPredictor {
    private:
        uint32_t predictionTableAddrBits;
        uint32_t pcMask;
        uint32_t globalHistReg;
        uint32_t *predictionTable;
        bool makePrediction(uint32_t);
        void trainPredictor(uint32_t, bool);
    public:
        GsharePredictor(uint32_t);
        ~GsharePredictor();
        void runPredictor();
};

class TournamentPredictor : public BranchPredictor {
    private:
        uint32_t predictionTableAddrBits;
        uint32_t pcMask;
        uint32_t globalHistReg;
        uint32_t *localHistTable;
        uint32_t *localHistPredictionTable;
        uint32_t *globalHistPredictionTable;
        uint32_t *predictorChoiceTable;
        bool makePrediction(uint32_t);
        void trainPredictor(uint32_t, bool);
    public:
        TournamentPredictor(uint32_t);
        ~TournamentPredictor();
        void runPredictor();
};

#endif
