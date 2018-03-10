#include "branch_predictor.h"

#include <iomanip>
#include <iostream>

/**
 * Strong not taken: 0
 * Weakly not taken: 1
 * Weakly taken:     2
 * Strong taken:     3
 */

using namespace std;

// Helper functions for reading file
bool ReadBranches(ifstream& trace_stream, uint32_t* pc, bool* outcome) {
  if (trace_stream.good()) {
    trace_stream.read((char*)pc, sizeof(uint32_t));

    char outcome_int = 0;
    trace_stream.read(&outcome_int, sizeof(outcome_int));
    if (outcome_int == 0) {
      *outcome = false;
    } else {
      *outcome = true;
    }
    return true;
  }

  return false;
}

void BranchPredictor::displayResults(uint32_t num_branch,
                                     uint32_t num_miss_predicts) {
  cout << "Branches in Trace: " << num_branch << endl;
  cout << "Misprediction Rate: " << num_miss_predicts << "/" << num_branch
       << setprecision(4) << " = "
       << ((double)num_miss_predicts / (double)num_branch) * 100 << endl;
}

// Run branch predictor simulator main routine
void BranchPredictor::run(uint32_t mem_limit,
                          const std::string& trace_file_name) {
  init(mem_limit);

  // Open trace file as binary file
  ifstream trace(trace_file_name, ios::in | ios::binary);

  uint32_t num_instructions = 0;
  if (trace.is_open()) {
    trace.read((char*)&num_instructions, sizeof(num_instructions));
    num_instructions = ntohl(num_instructions);

    // cout << "Number of instructions: " << num_instructions << endl;

  } else {
    cerr << "Unable to open file" << endl;
    return;
  }

  // Read each branch from the trace
  uint32_t pc = 0;

  bool outcome = false;
  uint32_t num_branches = 0;
  uint32_t mis_preds = 0;
  while (ReadBranches(trace, &pc, &outcome)) {
    pc = ntohl(pc);

    num_branches++;

    // Make a prediction and compare with actual outcome
    if (makePrediction(pc) != outcome) mis_preds++;

    // Train the predictor
    trainPredictor(pc, outcome);
  }

  displayResults(num_branches, mis_preds);
}

bool Gshare::init(uint32_t mem_limit) {
  // size of the table
  uint32_t table_bits = std::log2(mem_limit);
  // length of the history register, which matches the table
  uint32_t history_reg_bits = table_bits;
  // address mask for the pc
  addr_mask_ = (1 << history_reg_bits) - 1;
  // global history register master for table look up index
  ghr_mask_ = (1 << table_bits) - 1;
  global_history_reg_ = 0;
  table_.resize(mem_limit, 0);
  return true;
}

/**
 * Strong not taken: 0
 * Weakly not taken: 1
 * Weakly taken:     2
 * Strong taken:     3
 */

bool Gshare::makePrediction(uint32_t pc) {
  uint32_t index = (pc & addr_mask_) ^ global_history_reg_;
  return table_[index] >= 2;
}

void Gshare::trainPredictor(uint32_t pc, bool outcome) {
  uint32_t index = (pc & addr_mask_) ^ global_history_reg_;
  // cout << pc << ", " << boolalpha << outcome << "," << index << endl;
  if (outcome == true && table_[index] < 3) table_[index]++;
  if (outcome == false && table_[index] > 0) table_[index]--;

  global_history_reg_ = (((global_history_reg_ << 1) | outcome) & ghr_mask_);
}

bool BiModel::init(uint32_t mem_limit) { return true; }

bool BiModel::makePrediction(uint32_t pc) { return true; }
void BiModel::trainPredictor(uint32_t pc, bool outcome) { return; }

bool Tournament::init(uint32_t mem_limit) { return true; }

bool Tournament::makePrediction(uint32_t pc) { return true; }
void Tournament::trainPredictor(uint32_t pc, bool outcome) { return; }