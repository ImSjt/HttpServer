OBJ = Channel.o Poller.o PollPoller.o SocketsOps.o EventLoop.o EventLoopThread.o \
		Timer.o TimerQueue.o Socket.o InetAddress.o Acceptor.o Buffer.o \
		EventLoopThreadPool.o TcpConnection.o TcpServer.o

DIR_HTTP = $(shell pwd)/http
OBJ_HTTP = $(DIR_HTTP)/build-in.o

.PHONY : all http
all : http_build build-in.o

http_build:
	make -C $(DIR_HTTP)

clean:
	make -C $(DIR_HTTP) clean
	rm -f $(shell find -name "*.o")

build-in.o : $(OBJ) $(OBJ_HTTP)
	$(LD) $(LDFLAGS) -r -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
