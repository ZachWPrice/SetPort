all: portsetter

%: %.cpp
	g++-5 -std=c++14 $< -o setport

%: %.cc
	g++-5 -std=c++14 $< -o $@

%: %.c
	gcc $< -o $@

