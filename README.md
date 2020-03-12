[![Build Status](https://img.shields.io/github/workflow/status/keszocze/abo/C and C++ CI)](https://github.com/keszocze/abo/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

# abo Another BDD Optimization

## Installation
The following packages are needed (assuming a Ubuntu based Linux distribution)

* `cmake`
* `libboost-serialization-dev`
* `libtbb-dev`

If the documentation is being built, the following additional packages are also necessary

* `doxygen`
* `graphviz`
* `python3`
* `python3-pip`
* `sphinx` (installed with pip3)
* `sphinx_rtd_theme` (installed with pip3)

Building the documentation can be turned off by setting the cmake variable BUILD_DOCUMENTATION to OFF.
If it is being built, the resulting files are located in the build folder in docs/sphinx.

## How to reproduce the results
Build the code using cmake
* ```mkdir build && cd build```
* ```cmake ..```
* ```make```

To reproduce the results, run the script 'run_experiments.sh' in the build folder.
For each experiment, one text file in the folder 'result' will be created.

## How to cite

If you are using *abo*, please cite the following paper:

Andreas Wendler and Oliver Keszocze *A fast BDD Minimization Framework for Approximate Computing*. 
In: Design, Automation and Test in Europe (DATE'20) 

You can use the following bibtex code:
```
@inproceedings{Wendler2020,
  title = {{A Fast BDD Minimization Framework for Approximate Computing}},
  booktitle = {Design, Automation and Test in Europe},
  author = {Wendler, Andreas and Keszocze, Oliver},
  location = {Grenoble, FR}
}
```

## References

**[Soeken2016]** 
Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate computing, in: Asia and South Pacific Design Automation Conference. Macau, China. https://doi.org/10.1109/ASPDAC.2016.7428057

**[Froehlich2019]** 
Froehlich, S., Große, D., Drechsler, R., 2019. One Method - All Error-Metrics: A Three-Stage Approach for Error-Metric Evaluation in Approximate Computing, in: Design, Automation and Test in Europe. Florence, Italy, pp. 284–287. https://doi.org/10.23919/DATE.2019.8715138

**[Keszocze2018]**
Keszocze, O., Soeken, M., Drechsler, R., 2018. The complexity of error metrics. Information Processing Letters 139, 1–7. https://doi.org/10.1016/j.ipl.2018.06.010
