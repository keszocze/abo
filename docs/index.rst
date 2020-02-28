.. ABO documentation master file, created by
   sphinx-quickstart on Tue Sep 25 15:28:11 2018.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to ABO's documentation!
===============================

.. toctree::
   :maxdepth: 2
   :caption: Contents:


Installation
============

On Linux and Mac run the following instructions in the main ABO folder::

	mkdir build
	cd build
	cmake ..
	make

The binaries will be created in the ``build`` folder.

The following packages are needed (assuming a Ubuntu based Linux distribution)

* `libtbb2` and `libtbb-dev` 

How to cite
===========

If you are using *abo* or compare your approaches against it, please cite the following paper:

Andreas Wendler and Oliver Keszocze *A fast BDD Minimization Framework for Approximate Computing*.
In: Design, Automation and Test in Europe (DATE'20)

You can use the following bibtex code:

.. code-block:: bibtex

    @inproceedings{Wendler2020,
      title = {{A Fast BDD Minimization Framework for Approximate Computing}},
      booktitle = {Design, Automation and Test in Europe},
      author = {Wendler, Andreas and Keszocze, Oliver},
      location = {Grenoble, FR}
    }



Library
=======
Papers  either referenced in the paper above or interesting to read in the context of this tool are:

* [1] Soeken, M., Große, D., Chandrasekharan, A., Drechsler, R., 2016. BDD minimization for approximate computing, in: Asia and South Pacific Design Automation Conference. Macau, China. https://doi.org/10.1109/ASPDAC.2016.7428057
* [2] Froehlich, S., Große, D., Drechsler, R., 2019. One Method - All Error-Metrics: A Three-Stage Approach for Error-Metric Evaluation in Approximate Computing, in: Design, Automation and Test in Europe. Florence, Italy, pp. 284–287. https://doi.org/10.23919/DATE.2019.8715138
* [3] Keszocze, O., Soeken, M., Drechsler, R., 2018. The complexity of error metrics. Information Processing Letters 139, 1–7. https://doi.org/10.1016/j.ipl.2018.06.010



Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

Functionality
=============

.. toctree::
   :maxdepth: 2
   :caption: Approximation Operators

   operators

.. toctree::
   :maxdepth: 2
   :caption: Error Metrics

   metrics
   
.. toctree::
   :maxdepth: 2
   :caption: Utility Functions

   util

.. toctree::
   :maxdepth: 2
   :caption: Parsing

   parsing

.. toctree::
   :maxdepth: 2
   :caption: Example BDDs

   example_bdds
   
.. toctree::
   :maxdepth: 2
   :caption: Bucket Based BDD Minimization

   minimization
