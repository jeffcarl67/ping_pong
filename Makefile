SYSTEMC_HOME=../../onnc-umbrella-systemc/onncroot

.PHONY:all

all: ping_pong

ping_pong: ping_pong.cpp
	g++ -I$(SYSTEMC_HOME)/include -L$(SYSTEMC_HOME)/lib-linux64 -o $@ $< -lsystemc -lm
