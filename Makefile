CXX      = g++
CXXFLAGS =
LINK     = g++
LIBS     = -lpthread
TARGET   = led-server
OBJECTS  = led.o main.o cmd.o
all: Makefile $(TARGET)

clean:
	rm -f *.o

distclean:
	rm -f *.o $(TARGET)

$(TARGET):  $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS) $(LIBS)

led.o: led.cpp led.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

main.o: main.cpp led.h \
		cmd.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

cmd.o: cmd.cpp cmd.h \
		led.h
	$(CXX) -c  $(CXXFLAGS) -o $@ $<
