all: test_setport

%: %.cpp
	g++-5 -std=c++14 $< -o test_setport

%: %.cc
	g++-5 -std=c++14 $< -o $@

%: %.c
	gcc $< -o $@

