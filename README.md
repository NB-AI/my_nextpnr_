# my_nextpnr

The modified routing version<br/>


## Pre-steps:

* The SAT solver Cadical path is embedded in <br/>

nextpnr-master/CMakeLists.txt<br/>

Modify the following lines in this file to your own Cadical path:<br/>

include_directories(/home/c/cadical/src)<br/>
target_link_libraries(nextpnr-ice40 PRIVATE /home/c/cadical/build/libcadical.a)  # for shared library<br/>

Through that, the software can be used by router1.cc.<br/>


* Used C++ version: g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0


* The used versions of Yosys, Cadical, Icestorm are the embedded folders identifiable by name


## Modified files: 

* nextpnr-master/CMakeLists.txt
* nextpnr-master/common/route/router1.cc<br/>


## Code to run:

* Open terminal in nextpnr-master:

For the case that CMakeLists was modified before:<br/>
cmake -DARCH=ice40 -DCMAKE_INSTALL_PREFIX=/usr/local . -DBUILD_GUI=ON<br/>


For the case that router1.cc and/or CMakeLists were modified before:<br/>
make -j$(nproc)<br/>
sudo make install<br/>

* Open terminal in nextpnr-master/ice40/examples/blinky :

nextpnr-ice40 --json blinky.json --pcf blinky.pcf --asc new_blinky.asc > new_log_file.log 2>&1 --package hx1k<br/>
icepack new_blinky.asc new_blinky.bin<br/>
iceprog new_blinky.bin<br/>


