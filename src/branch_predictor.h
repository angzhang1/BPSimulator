// Branch Predictor Base Class

#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

class BranchPredictor {
 public:
  BranchPredictor() = default;
  virtual ~BranchPredictor(){};

  // This is the real main routine that runs the simulation with respect to a
  // memory limit and a trace file
  void run(uint32_t mem_limit, std::istream& trace);

 protected:
  virtual bool init(uint32_t mem_limit) = 0;

  virtual bool makePrediction(uint32_t pc) = 0;

  virtual void trainPredictor(uint32_t pc, bool outcome) = 0;

  virtual void displayResults(uint32_t num_branch, uint32_t num_miss_predicts);
};

class Naive : public BranchPredictor {
 public:
  Naive() = default;

 protected:
  virtual bool init(uint32_t mem_limit) { return true; }
  virtual bool makePrediction(uint32_t pc) { return true; }
  virtual void trainPredictor(uint32_t pc, bool outcome) {}
};

class Gshare : public BranchPredictor {
 public:
  Gshare() = default;

 protected:
  virtual bool init(uint32_t mem_limit);
  virtual bool makePrediction(uint32_t pc);
  virtual void trainPredictor(uint32_t pc, bool outcome);
  // virtual void displayResults();

  std::vector<uint8_t> table_;
  uint32_t global_history_reg_;
  uint32_t addr_mask_;
  uint32_t ghr_mask_;
};

class BiModel : public BranchPredictor {
 public:
  BiModel() = default;

 protected:
  virtual bool init(uint32_t mem_limit);
  virtual bool makePrediction(uint32_t pc);
  virtual void trainPredictor(uint32_t pc, bool outcome);

  // Define the variables needed
  std::vector<uint8_t> table_;
  uint32_t global_history_reg_;
  uint32_t addr_mask_;
  uint32_t ghr_mask_;
};

class Tournament : public BranchPredictor {
 public:
  Tournament() = default;

 protected:
  virtual bool init(uint32_t mem_limit);
  virtual bool makePrediction(uint32_t pc);
  virtual void trainPredictor(uint32_t pc, bool outcome);

  // Define the variables needed
  std::vector<uint8_t> table_;
  uint32_t global_history_reg_;
  uint32_t addr_mask_;
  uint32_t ghr_mask_;
};
