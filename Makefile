CC = g++
CXXFLAGS = -W -Wall -O3 -I . -std=c++17
TARGET = emulator
OBJS = chip8.o main.o 
LIBS=-lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system 

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o : %.cpp
	$(CC) $(CXXFLAGS) -o $@ -c $<


clean:
	rm *.o emulator