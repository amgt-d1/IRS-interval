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
	* In `input_data()` of `utils.hpp`, you can freely write codes for reading your dataset.
	* The default file format is `left,right`.
   	* For example, if you want to test on Book and BTC, `dataset_id = 0` and `dataset_id = 1`, respectively in `parameter` directory.
* Execution
	* See the directory of each algorithm.

## Citation
If you use our implementation, please cite the following paper.
``` 
@inproceedings{amagata2024independent,  
    title={Independent Range Sampling on Interval Data},  
    author={Amagata, Daichi},  
    booktitle={ICDE},  
    pages={449-461},  
    year={2024}  
}
``` 

## License
Copyright (c) 2023 Daichi Amagata  
This software is released under the [MIT license](https://github.com/amgt-d1/IRS-interval/blob/main/license.txt).
