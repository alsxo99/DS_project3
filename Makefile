CC = g++
CXXFLAGS = -Wall -std=c++17
OBJS = main.o
TARGET = test

%.o: %.cpp %.hpp
	$(CC) $(CXXFLAGS) -c $<

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $@

.PHONY : clean
clean :
	rm -f $(OBJS)
	rm -f $(TARGET)