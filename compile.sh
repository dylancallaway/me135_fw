g++ main.cpp -o test -Iinclude -lRTLib -lwiringPi -lrt -lpthread -Wall `pkg-config --cflags --libs opencv4.pc`
