compile: 
	clang++ -g -std=c++17 ./Code/*.cpp -o output -pthread
run:
	./output
