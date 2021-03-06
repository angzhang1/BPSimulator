/*
Group 2 - Branch Predictor Simulator
*/

#include "stdafx.h"
#include <stdio.h>
#include <Winsock2.h>
#include <array>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <random>
#include <list>
#include <vector>
#include <exception>
#include <cmath>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

// Prototypes **************************************************************
class BPSimulator {
protected:
    uint32_t numBranches;
    uint32_t numMispredicts;
    uint32_t memLimit;
    uint32_t globalHistRegister;
    uint32_t mask;
    vector<uint8_t> histTable;
    vector<uint8_t> bitTable;

    void predictHandler() {}
    void trainPredictor() {}
    bool makePrediction(uint32_t pc) { return true; }
public:
    BPSimulator(uint32_t memLimit);
    double getMispredictionRate();
    void displayResults();
};

class Gshare : public BPSimulator {
private:
    void predictHandler();
    void trainPredictor(uint32_t pc, bool outcome);
    bool makePrediction(uint32_t pc);
public:
    Gshare(uint32_t memLimit);
};

class Bimodal : public BPSimulator {
private:
    void predictHandler();
    void trainPredictor(uint32_t pc, bool outcome);
    bool makePrediction(uint32_t pc);
public:
    Bimodal();
};

class Tournament : public BPSimulator {
private:
    void predictHandler();
    void trainPredictor(uint32_t pc, bool outcome);
    bool makePrediction(uint32_t pc);
public:
    Tournament();
};

bool readTrace(FILE* stream, uint32_t* pc, bool* outcome);

// Definitions *************************************************************
const char* FILE_NAME = "C:\\Users\\Brently\\OneDrive\\Programming\\Workspace\\BPSimulator\\Traces\\gzip.trace";

// BPSimulator -------------------------------------------------------------
BPSimulator::BPSimulator(uint32_t memLimit) : histTable(memLimit, 0), bitTable(memLimit, 0) {
    this->memLimit = memLimit;
    numBranches = 0;
    numMispredicts = 0;
    mask = (1 << (int)(log(memLimit) / log(2))) - 1;
}

// Return misprediction rate.
double BPSimulator::getMispredictionRate() {
    return ((double)numMispredicts / (double)numBranches) * 100;
}

// Print results of prediction.
void BPSimulator::displayResults() {
    cout << "Memory Limit: " << memLimit << endl;
    cout << "Branches in Trace: " << numBranches << endl;
    cout << "Misprediction Rate: " << setprecision(4) << getMispredictionRate() << endl;
}

// Gshare ------------------------------------------------------------------
Gshare::Gshare(uint32_t memLimit) : BPSimulator(memLimit) {
    predictHandler();
}

// Compare trace file outcomes with predictions, and train predictor as it goes.
void Gshare::predictHandler() {
    uint32_t pc = 0;
    uint32_t numInstructions = 0;
    bool outcome = false;
    FILE* stream;

    // Open trace file.
    try {
        fopen_s(&stream, FILE_NAME, "r");
    }
    catch (exception e) {
        cout << "ERROR: Cannot open " << FILE_NAME << endl;
    }

    // Get number of instructions from trace file.
    if (fread(&numInstructions, sizeof(uint32_t), 1, stream) != 1) {
        printf("Could not read input file\n");
        return;
    }
    numInstructions = ntohl(numInstructions);

    // Read trace file, compare outcomes with predictions, train predictor.
    while (readTrace(stream, &pc, &outcome)) {
        pc = ntohl(pc);
        numBranches++;
        if (makePrediction(pc) != outcome)
            numMispredicts++;
        trainPredictor(pc, outcome);
    }

    cout << "*** Gshare Performance ***" << endl;
    displayResults();
}

// Train behavior of predictor.
void Gshare::trainPredictor(uint32_t pc, bool outcome) {
    int i = (pc & mask) ^ globalHistRegister;
    if (outcome == true && bitTable[i] < 3)
        bitTable[i]++;
    if (outcome == false && bitTable[i] > 0)
        bitTable[i]--;
    globalHistRegister = (((globalHistRegister << 1) | outcome) & mask);
}

// Return a branch prediction.
bool Gshare::makePrediction(uint32_t pc) {
    int i = (pc & mask) ^ (globalHistRegister & mask);
    return bitTable[i] >= 2;
}

// Bimodal -----------------------------------------------------------------
Bimodal::Bimodal() : BPSimulator(memLimit) {
    predictHandler();
}

// Compare trace file outcomes with predictions, and train predictor as it goes.
void Bimodal::predictHandler() {
    uint32_t pc = 0;
    uint32_t numInstructions = 0;
    bool outcome = false;
    FILE* stream;

    // Open trace file.
    try {
        fopen_s(&stream, FILE_NAME, "r");
    }
    catch (exception e) {
        cout << "ERROR: Cannot open " << FILE_NAME << endl;
    }

    // Get number of instructions from trace file.
    if (fread(&numInstructions, sizeof(uint32_t), 1, stream) != 1) {
        printf("Could not read input file\n");
        return;
    }
    numInstructions = ntohl(numInstructions);

    // Read trace file, compare outcomes with predictions, train predictor.
    while (readTrace(stream, &pc, &outcome)) {
        pc = ntohl(pc);
        numBranches++;
        if (makePrediction(pc) != outcome)
            numMispredicts++;
        trainPredictor(pc, outcome);
    }

    cout << "*** Bimodal Performance ***" << endl;
    displayResults();
}

// Train behavior of predictor.
void Bimodal::trainPredictor(uint32_t pc, bool outcome) {
    int i = pc & mask; // bimodel index generate by pc and mask
    // branch taken, prediction not ST, increment
    if(outcome == true && bitTable[i] < 3) {
        bitTable[i]++;
    }
    // branch not taken, prediction not SN, decrement
    if(outcome == false && bitTable[i] > 0) {
        bitTable[i]--;
    }
}

// Return a branch prediction.
bool Bimodal::makePrediction(uint32_t pc) {
    int i = pc & mask;
    return bitTable[i] >= 2;
}

// Tournament --------------------------------------------------------------
Tournament::Tournament() : BPSimulator(memLimit) {
    predictHandler();
}

// Compare trace file outcomes with predictions, and train predictor as it goes.
void Tournament::predictHandler() {
    uint32_t pc = 0;
    uint32_t numInstructions = 0;
    bool outcome = false;
    FILE* stream;

    // Open trace file.
    try {
        fopen_s(&stream, FILE_NAME, "r");
    }
    catch (exception e) {
        cout << "ERROR: Cannot open " << FILE_NAME << endl;
    }

    // Get number of instructions from trace file.
    if (fread(&numInstructions, sizeof(uint32_t), 1, stream) != 1) {
        printf("Could not read input file\n");
        return;
    }
    numInstructions = ntohl(numInstructions);

    // Read trace file, compare outcomes with predictions, train predictor.
    while (readTrace(stream, &pc, &outcome)) {
        pc = ntohl(pc);
        numBranches++;
        if (makePrediction(pc) != outcome)
            numMispredicts++;
        trainPredictor(pc, outcome);
    }

    cout << "*** Tournament Performance ***" << endl;
    displayResults();
}

// Train behavior of predictor.
void Tournament::trainPredictor(uint32_t pc, bool outcome) {

}

// Return a branch prediction.
bool Tournament::makePrediction(uint32_t pc) {
    return true;
}


// General scope definitions ------------------------------------------------
bool readTrace(FILE* stream, uint32_t* pc, bool* outcome)
{
    // Read PC.
    if (fread(pc, sizeof(uint32_t), 1, stream) != 1)
        return false;

    // Read branch instruction.
    uint32_t outcome_int;
    if (fread(&outcome_int, sizeof(uint32_t), 1, stream) != 1)
        return false;
    if (outcome_int == 0)
        *outcome = false;
    else
        *outcome = true;
    return true;
}



// Main ********************************************************************
int main()
{
    uint32_t memLimit = 32;

    Gshare gshare(memLimit);
    return 0;
}
