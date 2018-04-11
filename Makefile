all:
	g++ -g -o main *.cc -std=c++11 -lpthread -fpermissive

clean:
	rm -f main
