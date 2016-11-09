all:pro
pro: demo.cpp light_xgboost.hpp
	g++ -o pro demo.cpp -I ./ -Wall
clean:
	rm -rf pro
