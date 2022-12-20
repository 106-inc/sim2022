![GitHub repo size](https://img.shields.io/github/repo-size/106-inc/sim2022?style=flat-square)
![GitHub last commit](https://img.shields.io/github/last-commit/106-inc/sim2022?color=red&style=flat-square)
![Generated Button](https://raw.githubusercontent.com/106-inc/sim2022/image-data/badge.svg)

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
