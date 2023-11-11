CIS*3090 Assignment 3
Name: Kyle Lukaszek
ID: 1113798
Due: November 10th, 2023

COMPILATION INSTRUCTIONS:

To compile the two programs, run the following command:
  make

You should now have two executables, boids and boidsomp

You can also run the following command to clean up the directory:
  make clean


RUNNING INSTRUCTIONS:

usage: boids
  -will run the graphical version of the boids program
  -press q to quit

usage: boidsomp <number>
  -run the parallel version of the boids program
  -<number> is the number of iterations it will perform before exiting, the default is 1000


TIMINGS (6 core machine @ 4.9GHz):

I am recording timings in sets of three to take a look at variance.

The code has also been tested on the SOCS server. The timings are a bit slower (especially as i increases), but the parallel version is still faster than the serial version.

1000 Iterations: 

- 0.022411 seconds
- 0.023389 seconds
- 0.024498 seconds

2000 Iterations:

- 0.061715 seconds
- 0.047489 seconds
- 0.047077 seconds

4000 Iterations:

- 0.085252 seconds
- 0.082893 seconds
- 0.111048 seconds

8000 Iterations:

- 0.149974 seconds
- 0.170562 seconds
- 0.150100 seconds

16000 Iterations:

- 0.281542 seconds
- 0.280866 seconds
- 0.278825 seconds

32000 Iterations:

- 0.544395 seconds
- 0.534494 seconds
- 0.544541 seconds

64000 Iterations:

- 1.134410 seconds
- 1.072150 seconds
- 1.179133 seconds

128000 Iterations:

- 2.226186 seconds
- 2.111068 seconds
- 2.360520 seconds

256000 Iterations:

- 4.898292 seconds
- 4.823466 seconds
- 4.590906 seconds

512000 Iterations:

- 10.419532 seconds
- 10.556908 seconds
- 9.649738 seconds

1024000 Iterations:

- 16.596190 seconds
- 16.512312 seconds
- 20.652774 seconds
