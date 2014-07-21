all:
	g++ -Wall main.cpp PracticalSocket.cpp serialib.cpp -o bcast
clean:
	rm -rf *.o *~ bcast 
