<h1 align="center">
  <br>
  <a href="https://github.com/mcacace/dolly">
  <br>
</h1>

<h4 align="center">A Moose-based application <a href="http://mooseframework.org/" target="blank">MOOSE</a> to interface the Moose framework to the Mfront library.</h4>

<p align="center">
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/license-GPLv3-blue.svg"
         alt="GitHub License">
  </a>
</p>

## About
dolly (like the cloned sheep) is a MOOSE-based application that enable the integartion of the mfront behaviour. Visit the [MOOSE framework](http://mooseframework.org) page for more information.

## Licence
dolly is distributed under the [GNU GENERAL PUBLIC LICENSE v3](https://github.com/ajacquey/Golem/blob/master/LICENSE).


## Getting Started

#### Minimum System Requirements
The following system requirements are from the MOOSE framework (see [Getting Started](http://mooseframework.org/getting-started/) for more information):
* Compiler: C++11 Compliant GCC 4.8.4, Clang 3.4.0, Intel20130607
* Python 2.7+
* Memory: 16 GBs (debug builds)
* Processor: 64-bit x86
* Disk: 30 GBs
* OS: UNIX compatible (OS X, most flavors of Linux)

#### 1. Setting Up a MOOSE Installation
To install dolly, you need first to have a working and up-to-date installation of the MOOSE framework.  
To do so, please visit the [Getting Started](http://mooseframework.org/getting-started/) page of the MOOSE framework and follow the instructions. If you encounter difficulties at this step, you can ask for help on the [MOOSE-users Google group](https://groups.google.com/forum/#!forum/moose-users).

#### 2. Clone dolly
dolly can be cloned directly from [GitHub](https://github.com/mcacace/dolly) using [Git](https://git-scm.com/). In the following, we refer to the directory `projects` which you created during the MOOSE installation (by default `~/projects`):  

    cd ~/projects
    git clone https://github.com/mcacace/dolly.git
    cd ~/projects/golem
    git checkout main

*Note: the "main" branch of dolly is the "stable" branch which is updated only if all tests are passing.*

#### 3. Compile dolly
You can compile dolly by following these instructions:

    cd ~/projects/dolly
    make -j4

#### 4. Test dolly
To make sure that everything was installed properly, you can run the tests suite of dolly:

    cd ~/projects/dolly
    ./run_tests -j2

If all the tests passed, then your installation is working properly. You can now use dolly!

## Usage
To run dolly from the command line with multiple processors, use the following command:

    mpiexec -n <nprocs> ~/projects/dolly/dolly-opt -i <input-file>

Where `<nprocs>` is the number of processors you want to use and `<input-file>` is the path to your input file (extension `.i`).  

Information about the structure of the dolly input files can be found in the documentation (link to follow).
