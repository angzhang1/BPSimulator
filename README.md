# BPSimulator
Branch Prediction Simulator

Build instructions:

mkdir build
cd build
cmake ..
make

Run the simulator:

./simulator [predictor type] [trace file]

For example:
./simulator Gshare ../Traces/gzip.trace

Example output for the gzip.trace using Gshare:

Branches in Trace: 3668480
Misprediction Rate: 405222/3668480 = 11.05

