CC = g++
CFLAGS = -Wall -I./include

mcotf: mcotf.cpp
	$(CC) $(CFLAGS) -o mcotf mcotf.cpp

clear:
	rm mcotf
