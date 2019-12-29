OBJ = HttpServer.o HttpConnection.o HttpResponse.o

.PHONY : all
all : build-in.o

clean:
	rm -f $(shell find -name "*.o")

build-in.o : $(OBJ)
	$(LD) $(LDFLAGS) -r -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
