## Introduction
* This repository provides implementations of our algorithms for the IRS problem on interval data.

## Requirement
* Linux OS (Ubuntu).
   * The others have not been tested.
* `g++ 11.3.0` (or higher version).

## How to use
* Parameter configuration can be done via txt files in `parameter` directory.
* Dataset should be stored in `dataset` directory.
	* We assign a unique dataset ID for each dataset. You can freely assign it.
	* In `input_data()` of `util.hpp`, you can freely write codes for reading your dataset.
	* The default file format is `left,right`.
* Computation time will be stored in `result` directory.
* Compile: `g++ -O3 -o xxx.out main.cpp`
* Run: `./xxx.out`


## Citation
If you use our implementation, please cite the following paper.
``` 

``` 

## License
Copyright (c) 2023 Daichi Amagata  
This software is released under the [MIT license](https://github.com/amgt-d1/IRS-interval/blob/main/license.txt).
