g++ main.cpp /include/RTLib.cpp -o test -Iinclude -lwiringPi -lrt -lpthread -Wall `pkg-config --cflags --libs opencv4.pc`
