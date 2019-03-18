g++ main.cpp -o test -Iinclude -lRTLib.cpp -lwiringPi -lrt -lpthread -Wall `pkg-config --cflags --libs opencv4.pc`
