cd /share/home/hara/workspace/fove/src/build
make
./main 0
cd /share/home/hara/workspace/fove/data/src
python3 calibration.py
cd /share/home/hara/workspace/fove/src/build
./main 1
