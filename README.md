# BPSimulator
Branch Prediction Simulator

## Build instructions:

> mkdir build

> cd build

> cmake ..

> make

## Run the simulator

> ./simulator [predictor type] [trace file]

For example:
> ./simulator Gshare ../Traces/gzip.trace

Example output for the gzip.trace using Gshare:
```
Branches in Trace: 3668480
Misprediction Rate: 405222/3668480 = 11.05
```

### Process all traces using pipes

For example
> ls *.trace | xargs -L 1 ../BPSimulator/simulator Tournament > ../tournament_256.csv

## Result Visualization

![32KB Result](https://github.com/angzhang1/BPSimulator/blob/master/miss-prediction-rate_32.png)
