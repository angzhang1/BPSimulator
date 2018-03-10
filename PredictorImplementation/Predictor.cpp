/**
 * use a uint32_t pointer for all table, add a destroyer for all class
 * 
 * for all 2-bit counter:
 * strong not taken = 00
 * weak not taken = 01
 * weak taken = 10
 * strong taken = 11
 * 
 * for tournament predictor choice:
 * strong local = 00
 * weak local = 01
 * weak global = 10
 * strong global = 11
 */ 

#include "Predictor.h"
#include <cmath>
#include <array>
#include <iostream>
#include <cstdint>

using namespace std;

BranchPredictor::BranchPredictor(uint32_t memoryLimit) {
    /**
     * parent class for all predictors, with data necessary for all predictors
     */
    this->memoryLimit = memoryLimit;
    this->numberOfBranch = 0;
    this->numberOfMisprediction = 0;
}

BimodelPredictor::BimodelPredictor(uint32_t memoryLimit) : BranchPredictor(memoryLimit) {
    /**
     * bimodel predictor:
     *      prediction table indexed by pc & pcMask
     */ 
    this->predictionTableAddrBits = log2(this->memoryLimit);
    this->pcMask = (1 << this->predictionTableAddrBits) - 1;
    this->predictionTable = new uint32_t[this->memoryLimit];
    for(uint32_t i=0; i<this->memoryLimit; i++) {
        predictionTable[i] = 0;
    }
}

BimodelPredictor::~BimodelPredictor() {
    delete this->predictionTable;
}

bool BimodelPredictor::makePrediction(uint32_t pc) {
    uint32_t index = pc & this->pcMask;
    return this->predictionTable[index] >= 2;
}

void BimodelPredictor::trainPredictor(uint32_t pc, bool outcome) {
    uint32_t index = pc & this->pcMask;
    if(outcome == true && this->predictionTable[index] < 3) {
        this->predictionTable[index]++;
    }
    if(outcome == false && this->predictionTable[index] > 0) {
        this->predictionTable[index]--;
    }
}

LocalHistPredictor::LocalHistPredictor(uint32_t memoryLimit) : BranchPredictor(memoryLimit) {
    /**
     * local history predictor: 
     *      a table of branch history for each branch indexed by pc & pcMask
     *      prediction table indexed by (pc & pcMask) ^ branch history
     */ 
    this->predictionTableAddrBits = log2(this->memoryLimit);
    this->pcMask = (1 << this->predictionTableAddrBits) - 1;
    this->localHistTable = new uint32_t[this->memoryLimit];
    this->predictionTable = new uint32_t[this->memoryLimit];
    for(uint32_t i=0; i<this->memoryLimit; i++) {
        localHistTable[i] = 0;
        predictionTable[i] = 0;
    }
}

LocalHistPredictor::~LocalHistPredictor() {
    delete this->localHistTable;
    delete this->predictionTable;
}

bool LocalHistPredictor::makePrediction(uint32_t pc) {
    uint32_t index = (pc ^ this->localHistTable[pc & this->pcMask]) & this->pcMask;
    return this->predictionTable[index] >= 2;
}

void LocalHistPredictor::trainPredictor(uint32_t pc, bool outcome) {
    uint32_t index = (pc ^ this->localHistTable[pc & this->pcMask]) & this->pcMask;;
    if(outcome == true && this->predictionTable[index] < 3) {
        this->predictionTable[index]++;
    }
    if(outcome == false && this->predictionTable[index] > 0) {
        this->predictionTable[index]--;
    }
    this->localHistTable[pc & this->pcMask] = (this->localHistTable[pc & this->pcMask] << 1) | outcome;
}

GsharePredictor::GsharePredictor(uint32_t memoryLimit) : BranchPredictor(memoryLimit) {
    /**
     * gshare predictor:
     *      a global history register record history of all branches
     *      prediction table indexed by (pc & pcMask) ^ (global history & pcMask)
     */ 
    this->predictionTableAddrBits = log2(this->memoryLimit);
    this->pcMask = (1 << this->predictionTableAddrBits) - 1;
    this->globalHistReg = 0;
    this->predictionTable = new uint32_t[this->memoryLimit];
    for(uint32_t i=0; i<this->memoryLimit; i++) {
        this->predictionTable[i] = 0;
    }
}

GsharePredictor::~GsharePredictor() {
    delete this->predictionTable;
}

bool GsharePredictor::makePrediction(uint32_t pc) {
    uint32_t index = (pc & this->pcMask) ^ (this->globalHistReg & this->pcMask);
    return this->predictionTable[index] >= 2;
}

void GsharePredictor::trainPredictor(uint32_t pc, bool outcome) {
    uint32_t index = (pc & this->pcMask) ^ (this->globalHistReg & this->pcMask);
    if(outcome == true && this->predictionTable[index] < 3) {
        this->predictionTable[index]++;
    }
    if(outcome == false && this->predictionTable[index] > 0) {
        this->predictionTable[index]--;
    }
    this->globalHistReg = (this->globalHistReg << 1) | outcome;
}

TournamentPredictor::TournamentPredictor(uint32_t memoryLimit) : BranchPredictor(memoryLimit) {
    /**
     * tournament predictor:
     *      with one gshare predictor, and one local history predictor
     *      using a choice table to choose between two predictors
     *      choice table indexed by pc & pcMask, each entry is a 2-bit counter, if one predictor makes right
     *      prediction for 2 consecutive times, this predictor is perfered for the specific branch
     *      default predictor is gshare in the code below
     */ 
    this->predictionTableAddrBits = log2(this->memoryLimit);
    this->pcMask = (1 << this->predictionTableAddrBits) - 1;
    this->globalHistReg = 0;
    this->globalHistPredictionTable = new uint32_t[this->memoryLimit];
    this->localHistPredictionTable = new uint32_t[this->memoryLimit];
    this->localHistTable = new uint32_t[this->memoryLimit];
    this->predictorChoiceTable = new uint32_t[this->memoryLimit];
    for(uint32_t i=0; i<this->memoryLimit; i++) {
        this->globalHistPredictionTable[i] = 0;
        this->localHistPredictionTable[i] = 0;
        this->localHistTable[i] = 0;
        this->predictorChoiceTable[i] = 2;
    }
}

TournamentPredictor::~TournamentPredictor() {
    delete this->globalHistPredictionTable;
    delete this->localHistPredictionTable;
    delete this->localHistTable;
    delete this->predictorChoiceTable;
}

bool TournamentPredictor::makePrediction(uint32_t pc) {
    uint32_t localIndex = (this->localHistTable[pc & this->pcMask] ^ pc) & this->pcMask;
    bool localPrediction = (this->localHistPredictionTable[localIndex] >= 2);
    uint32_t globalIndex = (pc & this->pcMask) ^ (this->globalHistReg & this->pcMask);
    bool globalPredicton = (this->globalHistPredictionTable[globalIndex] >= 2);
    if(this->predictorChoiceTable[pc & this->pcMask] >= 2) {
        return globalPredicton;
    }
    else {
        return localPrediction;
    }
}

void TournamentPredictor::trainPredictor(uint32_t pc, bool outcome) {
    uint32_t localIndex = (this->localHistTable[pc & this->pcMask] ^ pc) & this->pcMask;
    bool localPrediction = (this->localHistPredictionTable[localIndex] >= 2);
    uint32_t globalIndex = (pc & this->pcMask) ^ (this->globalHistReg & this->pcMask);
    bool globalPredicton = (this->globalHistPredictionTable[globalIndex] >= 2);
    uint32_t index = pc & this->pcMask;
    if(localPrediction != outcome && globalPredicton == outcome && this->predictorChoiceTable[index] < 3) {
        this->predictorChoiceTable[index]++;
    }
    if(localPrediction == outcome && globalPredicton != outcome && this->predictorChoiceTable[index] > 0) {
        this->predictorChoiceTable[index]--;
    }
    if(outcome == true && this->localHistPredictionTable[localIndex] < 3) {
        this->localHistPredictionTable[localIndex]++;
    }
    if(outcome == false && this->localHistPredictionTable[localIndex] > 0) {
        this->localHistPredictionTable[localIndex]--;
    }
    this->localHistTable[pc & this->pcMask] = (this->localHistTable[pc & this->pcMask] << 1) | outcome;
    if(outcome == true && this->globalHistPredictionTable[globalIndex] < 3) {
        this->globalHistPredictionTable[globalIndex]++;
    }
    if(outcome == false && this->globalHistPredictionTable[globalIndex] > 0) {
        this->globalHistPredictionTable[globalIndex]--;
    }
    this->globalHistReg = (this->globalHistReg << 1) | outcome;
}
