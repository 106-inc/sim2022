![Generated Button](https://tokei.rs/b1/github/106-inc/sim2022?category=code)

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
