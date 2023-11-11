/*
CIS*3090 Assignment 3
Name: Kyle Lukaszek
ID: 1113798
Due: November 10th, 2023
*/

/* Boids using ASCII graphics
   -original NCurses code from "Game Programming in C with the Ncurses Library"
    https://www.viget.com/articles/game-programming-in-c-with-the-ncurses-library/
    and from "NCURSES Programming HOWTO"
    http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
   -Boids algorithms from "Boids Pseudocode:
    http://www.kfish.org/boids/pseudocode.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#ifndef NOGRAPHICS
#include <omp.h>
#include <unistd.h>
#include <ncurses.h>
#endif

#define DELAY 50000

// population size, number of boids
#define POPSIZE 50
// maximum screen size, both height and width
#define SCREENSIZE 100

// boid location (x,y,z) and velocity (vx,vy,vz) in boidArray[][]
#define BX 0
#define BY 1
#define BZ 2
#define VX 3
#define VY 4
#define VZ 5

#define X 0
#define Y 1
#define Z 2

#ifndef NOGRAPHICS
// maximum screen dimensions
int max_y = 0, max_x = 0;
#endif

// location and velocity of boids
float boidArray[POPSIZE][6];

// change in velocity is stored for each boid (x,y,z)
// this is applied after all changes are calculated for each boid
float boidUpdate[POPSIZE][3];

// Function prototypes
void initBoids();
int drawBoids();
void moveBoids();
void ruleOne(float *pc_j, int j);
void ruleTwo(float *c, int j);
void ruleThree(float *pv_j, int j);
void ruleFour(float *place, int sign, int boid);

// Helper function
void setVector3(float *v, float x, float y, float z)
{
   v[0] = x;
   v[1] = y;
   v[2] = z;
}

void initBoids()
{
   int i;
   // calculate initial random locations for each boid, scaled based on the screen size
   for (i = 0; i < POPSIZE; i++)
   {
      boidArray[i][BX] = (float)(random() % SCREENSIZE);
      boidArray[i][BY] = (float)(random() % SCREENSIZE);
      boidArray[i][BZ] = (float)(random() % SCREENSIZE);
      boidArray[i][VX] = 0.0;
      boidArray[i][VY] = 0.0;
      boidArray[i][VZ] = 0.0;
   }
}

#ifndef NOGRAPHICS
int drawBoids()
{
   int c, i;
   float multx, multy;

   // update screen maximum size
   getmaxyx(stdscr, max_y, max_x);

   // used to scale position of boids based on screen size
   multx = (float)max_x / SCREENSIZE;
   multy = (float)max_y / SCREENSIZE;

   clear();

   // display boids
   for (i = 0; i < POPSIZE; i++)
   {
      mvprintw((int)(boidArray[i][BX] * multy), (int)(boidArray[i][BY] * multx), "o");
   }

   refresh();

   usleep(DELAY);

   // read keyboard and exit if 'q' pressed
   c = getch();
   if (c == 'q')
      return (1);
   else
      return (0);
}
#endif

void moveBoids()
{
   int i;

   /*** Your code goes here ***/
   /*** add the code to implement the three boids rules ***/
   /*** do not change any other code except to add timing to the main() routine ***/
   /*** do not replace the data structures defined in this program ***/
   /*** omp code should be used here ***/
   /*** you can call other functions from this location ***/
   
   // counter and sign for rule4 (previously: moveFlock())
   static int counter = 0;
   static int sign = 1;
   
   // pull flock towards two points as the program runs
   // every 200 iterations change point that flock is pulled towards
   if (counter % 200 == 0)
   {
      sign = sign * -1;
   }

   // Pointers to array of floats for each rule vector
   float *all_v1, *all_v2, *all_v3, *all_v4;

   // Initialize arrays of all vectors for each rule per boid (Vector3(0, 0, 0) x POPSIZE -> [0, 0, 0, 0, 0, 0, ...])
   all_v1 = malloc(sizeof(float) * POPSIZE * 3);
   all_v3 = malloc(sizeof(float) * POPSIZE * 3);
   all_v2 = malloc(sizeof(float) * POPSIZE * 3);
   all_v4 = malloc(sizeof(float) * POPSIZE * 3);

   // move boids by calculating updated velocity and new position
   // you can optimize this code if you wish and you can add omp directives here

   /* 
   We pass each iteration of the loop to a thread and set the scheduling to static.
   Dynamic scheduling was also tested but static scheduling was faster.
   */

   // Update the velocity and the position of each boid
   #pragma omp parallel for private(i) shared(sign) schedule(static)
   for (i = 0; i < POPSIZE; i++)
   {
      // Locate the empty rule vectors for each boid/thread
      float *v1 = all_v1 + (i * 3);
      float *v2 = all_v2 + (i * 3);
      float *v3 = all_v3 + (i * 3);
      float *v4 = all_v4 + (i * 3);

      // Apply rules to each boid and store the result in the empty vectors
      ruleOne(v1, i);
      ruleTwo(v2, i);
      ruleThree(v3, i);
      ruleFour(v4, sign, i);

      // Update velocity for each boid using value stored in boidUpdate[][]
      boidArray[i][VX] += boidUpdate[i][BX];
      boidArray[i][VY] += boidUpdate[i][BY];
      boidArray[i][VZ] += boidUpdate[i][BZ];

      // Apply rule vectors to boid velocity
      boidArray[i][VX] += (v1[X] + v2[X] + v3[X] + v4[X]);
      boidArray[i][VY] += (v1[Y] + v2[Y] + v3[Y] + v4[Y]);
      boidArray[i][VZ] += (v1[Z] + v2[Z] + v3[Z] + v4[Z]);

      // Update position for each boid using newly updated velocity
      boidArray[i][BX] += boidArray[i][VX];
      boidArray[i][BY] += boidArray[i][VY];
      boidArray[i][BZ] += boidArray[i][VZ];
   }

   // Free memory allocated for rule vectorsglobal
   free(all_v1);
   free(all_v2);
   free(all_v3);
   free(all_v4);

   // Increment counter for rule4 (moveFlock())
   counter++;
}

// Rule 1: Boids try to fly towards the centre of mass of neighbouring boids.
void ruleOne(float *pc_j, int j)
{
   // Initialize perceived center of gravity vector to 0
   setVector3(pc_j, 0, 0, 0);

   // for each boid, calculate the center of mass of the flock
   for (int k = 0; k < POPSIZE; k++)
   {
      // skip the current boid
      if (k == j)
         continue;
      
      // add the position of each boid to the center of mass
      pc_j[X] += boidArray[k][BX];
      pc_j[Y] += boidArray[k][BY];
      pc_j[Z] += boidArray[k][BZ];
   }

   // divide by N-1 to get the average
   pc_j[X] /= (POPSIZE - 1);
   pc_j[Y] /= (POPSIZE - 1);
   pc_j[Z] /= (POPSIZE - 1);

   // Set the vector to move the boid 1% of the way towards the center of mass
   pc_j[X] = (pc_j[X] - boidArray[j][BX]) / 100;
   pc_j[Y] = (pc_j[Y] - boidArray[j][BY]) / 100;
   pc_j[Z] = (pc_j[Z] - boidArray[j][BZ]) / 100;
} 

// Rule 2: Boids try to keep a small distance away from other objects (including other boids).
void ruleTwo(float *c, int j)
{
   // Initialize vector to 0
   setVector3(c, 0, 0, 0);

   // avoid collision with other boids
   for (int k = 0; k < POPSIZE; k++)
   {
      if (k == j)
         continue;

      // If the boid is too close, move away from it
      if (abs(boidArray[k][BX] - boidArray[j][BX]) < 3 &&
          abs(boidArray[k][BY] - boidArray[j][BY]) < 3 &&
          abs(boidArray[k][BZ] - boidArray[j][BZ]) < 3)
      {
         // Adjust the vector to move away from the boid
         c[X] -= (boidArray[k][BX] - boidArray[j][BX]);
         c[Y] -= (boidArray[k][BY] - boidArray[j][BY]);
         c[Z] -= (boidArray[k][BZ] - boidArray[j][BZ]);
      }
   }
}

// Rule 3: Boids try to match velocity with near boids.
void ruleThree(float *pv_j, int j)
{
   // Initialize perceived velocity vector to 0
   setVector3(pv_j, 0, 0, 0);

   // for each boid, calculate the perceived velocity
   for (int k = 0; k < POPSIZE; k++)
   {
      // skip the current boid
      if (k == j)
         continue;

      // add the velocity of each boid to the perceived velocity
      pv_j[X] += boidArray[k][VX];
      pv_j[Y] += boidArray[k][VY];
      pv_j[Z] += boidArray[k][VZ];
   }

   // divide by N-1 to get the average
   pv_j[X] /= (POPSIZE - 1);
   pv_j[Y] /= (POPSIZE - 1);
   pv_j[Z] /= (POPSIZE - 1);

   // Set the vector to 1/8th the difference between the perceived velocity and the current velocity 
   pv_j[X] = (pv_j[X] - boidArray[j][VX]) / 8;
   pv_j[Y] = (pv_j[Y] - boidArray[j][VY]) / 8;
   pv_j[Z] = (pv_j[Z] - boidArray[j][VZ]) / 8;
}

// Rule 4: Tend towards place
// Originally moveFlock()
void ruleFour(float *place, int sign, int boid)
{
   // move flock towards position (40,40,40)
   if (sign == 1)
      setVector3(place, 20.0, 20.0, 20.0);
   // move flock towards position (60,60,60)
   else
      setVector3(place, 60.0, 60.0, 60.0);

   // add place offset to the boid location (x,y,z) in order to pull it
   // towards the current target point

   // Set the vector to 1/200th the difference between the place and the current position
   place[X] = (place[X] - boidArray[boid][BX]) / 200;
   place[Y] = (place[Y] - boidArray[boid][BY]) / 200;
   place[Z] = (place[Z] - boidArray[boid][BZ]) / 200;
}

int main(int argc, char *argv[])
{
   int i, count;

#ifndef NOGRAPHICS
   // initialize curses
   initscr();
   noecho();
   cbreak();
   timeout(0);
   curs_set(FALSE);
   // Global var `stdscr` is created by the call to `initscr()`
   getmaxyx(stdscr, max_y, max_x);
#endif

   // place boids in initial position
   initBoids();

   // draw and move boids using ncurses
   // do not calculate timing in this loop, ncurses will reduce performance
#ifndef NOGRAPHICS
   while (1)
   {
      if (drawBoids() == 1)
         break;
      moveBoids();
   }
#endif

   // calculate movement of boids but do not use ncurses to draw
   // this is used for timing the parallel implementation of the program
#ifdef NOGRAPHICS
   count = 1000;
   // read number of iterations from the command line
   if (argc == 2)
      sscanf(argv[1], "%d", &count);
   printf("Number of iterations %d\n", count);

   /*** Start timing here ***/
   // we have to use gettimeofday because clock() is not thread safe and will not give correct results
   struct timeval start, end;

   gettimeofday(&start, NULL);

   for (i = 0; i < count; i++)
   {
      moveBoids();
   }
   /*** End timing here ***/
   gettimeofday(&end, NULL);

   // calculate time taken for the loop
   double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

   printf("Time taken: %lf seconds\n", time_taken);
   /*** print timing results ***/
#endif

#ifndef NOGRAPHICS
   // shut down curses
   endwin();
#endif
}
