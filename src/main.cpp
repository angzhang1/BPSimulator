#include "branch_predictor.h"

using namespace std;

static uint32_t kMemLimit = 32 * 1024 / 2;

int main(int argc, char** argv) {
  string file_name;
  string predictor;

  if (argc == 3) {
    file_name = argv[2];
    predictor = argv[1];
  } else if (argc == 2) {
    predictor = argv[1];
  } else {
    cout << "Usage:" << endl;
    cout << argv[0] << " [predictor type] [trace file name]" << endl;
    return 1;
  }

  istream* trace_input = nullptr;
  ifstream trace_file;
  if (!file_name.empty()) {
    // Open trace file as binary file
    trace_file.open(file_name, ios::in | ios::binary);

    uint32_t num_instructions = 0;
    if (trace_file.good()) {  // consume the number of instruction field
      trace_file.read((char*)&num_instructions, sizeof(num_instructions));
      num_instructions = ntohl(num_instructions);
    } else {
      cerr << "Unable to open file" << endl;
      return 1;
    }

    trace_input = &trace_file;
  } else {
    // read from cin
    trace_input = &cin;
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

  // Used cout to print to console, we can use ofstream to write the result to a
  // file, if desired.
  bp->run(kMemLimit, *trace_input, cout);

  delete bp;

  return 0;
}