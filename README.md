# RobinX: an XML-driven classification for round-robin sports scheduling
This readme provides additional info on how to compile the RobinX project.
When using this software, please always refer to the following article: Van Bulck, D., Goossens, D., and Schönberger, J., & Guajardo, M. (2020). RobinX: A three-field classification and unified data format for round-robin sports timetabling. European Journal of Operational Research, 280, 568-580.
In addition, the directory Repository contains a backup for the problem instances and solutions found at <https://www.sportscheduling.ugent.be/RobinX>.

# Requirements
- [GCC](https://gcc.gnu.org/)
- [GNU Make](https://www.gnu.org/software/make/)

# Portability
The project is developed under the GNU/Linux OS and the gcc compiler and is not tested under other OSs and compilers.
Since no OS or compiler specific features are used, it is expected that the code also compiles under other platforms or compilers.

# Installation on Linux
Download the directory from github, and execute the following command in a terminal.

```bash
$ cd ./RobinX
$ make
```
# Minimal working example
Once the software is installed, you can validate solutions by providing the appropriate instance and soluton XML files.

```bash
$ ./RobinX -i ./Repository/TravelOptimization/Instances/CON4.xml -s ./Repository/TravelOptimization/Solutions/CON4_Sol_Brandao.xml
```

# Warning
RobinXml is free software and comes with ABSOLUTELY NO WARRENTY! You are welcome
to use, modify, and redistribute it under the GNU General Public Lincese.
For the latest version of this software, please always refer to the online validator available at <https://www.sportscheduling.ugent.be/RobinX>.

# Repository
This directory is a back-up for the problem instancens, bounds, and solutions at <https://www.sportscheduling.ugent.be/RobinX>. An overview of all problems instances can be found in repository.xml.
Please note that this directory is merely a back-up for the website, and that for the latest overview of best found solutions you should always refer to the website itself!
