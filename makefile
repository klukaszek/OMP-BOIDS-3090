
all: boids boidsomp

boids: boids.c
	gcc boids.c -o boids -lncurses -lm 

boidsomp: boids.c
	gcc -fopenmp -g boids.c -o boidsomp -lm -DNOGRAPHICS

