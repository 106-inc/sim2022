# sim2022
RISC-V simulator

## How to build

At first you have to install `spdlog` library:
```
sudo apt install libspdlog-dev
```
Then you need to instal `GTest` for testin:
```
sudo apt-get install libgtest-dev
```
Now you are ready to build:
```
cd /path/to/sim2022
mkdir build
cd build
cmake ..
cmake --build . -j
```