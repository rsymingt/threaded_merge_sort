all: clear compile run

clear:
	clear

compile: src/*.c
	gcc src/*.c -Wall -g -std=c99 -o mergesort -pthread

run:
	./mergesort
