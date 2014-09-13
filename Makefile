CXXFLAGS = -std=c++1y

.DEFAULT: all

all: learn parse

learn: learn.cc
	g++ $(CXXFLAGS) learn.cc -o learn

parse: parse.cc
	g++ $(CXXFLAGS) parse.cc -o parse


.PHONY: clean

clean:
	rm learn
