g++ main.cpp -o test -Iinclude -lwiringPi -lrt -lpthread -Wall `pkg-config --cflags --libs opencv4.pc`
