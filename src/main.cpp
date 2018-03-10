#include "branch_predictor.h"

using namespace std;

static uint32_t kMemLimit = 32 * 1024 / 2;

int main(int argc, char** argv) {
  string file_name;
  string predictor;

  if (argc == 3) {
    file_name = argv[2];
    predictor = argv[1];
  } else {
    cout << "Usage:" << endl;
    cout << argv[0] << " [predictor type] [trace file name]" << endl;
    return 1;
  }

  BranchPredictor* bp = nullptr;

  if (predictor == "Gshare") {
    bp = new Gshare();
  } else if (predictor == "BiModel") {
    bp = new BiModel();
  } else if (predictor == "Tournament") {
    bp = new Tournament();
  } else {
    // Run the naive predictor which always predicts true
    bp = new Naive();
    cout << "Running naive simulator! " << endl;
  }

  bp->run(kMemLimit, file_name);

  delete bp;

  return 0;
}