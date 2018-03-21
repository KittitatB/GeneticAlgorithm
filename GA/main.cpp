/*
 Cifer: Automating classical cipher cracking in C
 Copyright (C) 2008  Daniel Richman & Simrun Basuita
 
 Cifer is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Cifer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Cifer.  If not, see <http://www.gnu.org/licenses/>.
 */

/* ga.c is an example of a very simple genetic algorithm in C */
/* It could be useful for cracking encryption schemes such as monoalphabetic
 * substitution... maybe someday we will try it out... */

/* Usage: edit the below #define's to your liking, set the target solution,
 * compile, and run without any arguments. Sometimes it will get stuck at ~95%.
 * If it does this, just kill it and restart ;) */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GA_POPSIZE 2048 // Population size
#define GA_MAXITER 1648 // Maximum iterations
#define GA_ELITERATE 0.10 // Elitism rate
#define GA_MUTPERC 0.25 // Mutation rate
#define PRINT_INTER 0 // Print status every this iterations/generations

#define GA_ELITSIZE (GA_POPSIZE * GA_ELITERATE) // Number of elites
#define GA_MUTCHANCE 4

#define TARGET_LEN 20 /* The length of int target (below) */
#define CHROMOSOME_MAX 10 /* The maximum size of a chromosome */
int target[TARGET_LEN] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }; // Target solution

/* A member of our population */
typedef struct
{
    int sol[TARGET_LEN]; // This member's solution (chromosome)
    int fitness;         // How good the solution is
} ga_memb;

void init_pop(ga_memb *pop); /* Calls: */
void zero_fitness(ga_memb *pop); /* Set fitnesses to 0 */
void randall_sols(ga_memb *pop); /* Make all the solutions random */

void calc_fitness(ga_memb *pop); /* Calculate a member's fitness based on its
                                  solution */

void sort_by_fitness(ga_memb *pop); /* Sort the whole population by fitness */

/* Print the best member's solution, gen is the number of the current
 * generation which we also print */
void print_best(ga_memb *pop, unsigned const int gen);

void mate_pop(ga_memb *pop, ga_memb *buf); // Mates pop into buf
void cp_mems(ga_memb *src, ga_memb *targ, unsigned int size);
void mutate(ga_memb *pop); // Mutates some of the population

/* Swaps the things that pt1 and pt2 point to */
void swap_pts(ga_memb **pt1, ga_memb **pt2);

/* We use a simple insertion sort */
#define sort_by_fitness(a) (sort_ga_memb(a, GA_POPSIZE))
void sort_ga_memb(ga_memb *a, int asize);

int main(void)
{
    int i, j; /* Counters */
    
    /* Create our two populations */
    /* They are really the same population, but we havae two for speed */
    ga_memb pop_a[GA_POPSIZE], pop_b[GA_POPSIZE];
    ga_memb *pop = pop_a, *buf = pop_b;
    
    srand((unsigned int) time(NULL)); /* Seed rand() with the time */
    
    init_pop(pop_a);
    init_pop(pop_b);
    
    for (i = 0, j = 0; i < GA_MAXITER; i++, j++)
    {
        calc_fitness(pop); // Fill out ga_memb.fitness
        sort_by_fitness(pop);
        
        if (j > PRINT_INTER) /* Print our stats every PRINT_INTER iterations of
                              the loop */
        {
            print_best(pop, i);
            j = 0;
        }
        
        // If the number of correct parts is the same as the TARGET_LEN, it is all
        // correct :)
        if (pop[GA_POPSIZE - 1].fitness == TARGET_LEN) break;
        
        mate_pop(pop, buf); // Mate the population into the buffer
        swap_pts(&pop, &buf); // Make the buffer our population, and vice versa
    }
    
    print_best(pop, i);
    
    return 0;
}

void init_pop(ga_memb *pop)
{
    zero_fitness(pop);
    randall_sols(pop);
}

void zero_fitness(ga_memb *pop)
{
    int i;
    for (i = 0; i < GA_POPSIZE; i++)
    {
        pop[i].fitness = 0;
    }
}

void randall_sols(ga_memb *pop)
{
    int i, j;
    for (i = 0; i < GA_POPSIZE; i++)  for (j = 0; j < TARGET_LEN; j++)
        pop[i].sol[j] = rand() % CHROMOSOME_MAX;
}

/* For every correct chromosome, we add 1 to the score */
/* Thus, a perfect score is equal to TARGET_LEN */
void calc_fitness(ga_memb *pop)
{
    unsigned int fitness;
    
    int i, j;
    for (i = 0; i < GA_POPSIZE; i++)
    {
        fitness = 0;
        for (j = 0; j < TARGET_LEN; j++)
        {
            if (pop[i].sol[j] == target[j])
            {
                fitness += 1;
            }
        }
        pop[i].fitness = fitness;
    }
}

//#define balh(a) (insertion_sort_ga_memb(a, GA_POPSIZE))

void swap(ga_memb *a, ga_memb *b)
{
    ga_memb temp = *a;
    *a = *b;
    *b = temp;
}


void quicksort(ga_memb arr[], int l, int r)
{

    if (l >= r)
    {
        return;
    }
    
    int pivot = arr[r].fitness;
    int cnt = l;
    
    for (int i = l; i <= r; i++)
    {
        
        if (arr[i].fitness <= pivot)
        {

            swap(&arr[cnt], &arr[i]);

            cnt++;
        }
    }
    
    quicksort(arr, l, cnt-2);
    quicksort(arr, cnt, r);
}

void sort_ga_memb(ga_memb *a, int asize)
{
    
    quicksort(a, 0, asize);
//    int i, j, k;
//    ga_memb d;
//    
//    k = asize;
//    for (i = 0; i < k; i++)
//    {
//        d = a[i];
//        j = i - 1;
//        while (j >= 0 && a[j].fitness > d.fitness)
//        {
//            a[j + 1] = a[j];
//            j = j - 1;
//        }
//        a[j + 1] = d;
//    }
}

void print_best(ga_memb *pop, unsigned const int gen)
{
    int i;
    
    printf("At gen %d, best:%d", gen, *(pop[GA_POPSIZE - 1].sol));
    for (i = 1; i < TARGET_LEN; i++)
        printf(",%d", *((pop[GA_POPSIZE - 1].sol) + i));
    printf("  (%d%%)\n", (pop[GA_POPSIZE - 1].fitness * 100 ) / TARGET_LEN);
}

/* Our mating method is a simple cut and swap:
 *
 * Consider two solutions:
 *
 * parent1: |====================|
 * parent2: |XXXXXXXXXXXXXXXXXXXX|
 *
 * Pick an arbitrary point to cut at:
 *
 * parent1: |==============|======|
 * parent2: |XXXXXXXXXXXXXX|XXXXXX|
 *
 * Swap the part of the solution after that point to form the children:
 *
 * child1:  |==============XXXXXXX|
 * child2:  |XXXXXXXXXXXXXX=======|
 *
 * Note that only the children go into the new population; all the parents
 * die.
 */

ga_memb tornament(ga_memb *pop){
    ga_memb competitor[3];
    for(int i = 0; i<3; i++){
        competitor[i] = pop[rand() % GA_POPSIZE];
    }
    ga_memb winner = competitor[0];
    for(int i = 1; i<3; i++){
        if(winner.fitness < competitor[i].fitness) winner = competitor[i];
    }
    return winner;
}


void mate_pop(ga_memb *pop, ga_memb *buf) // Mates pop into buf
{
    unsigned int i, j;
    ga_memb p1, p2;
    int randint;
    
    // Copy over the elites
    cp_mems(pop, buf, GA_ELITSIZE);
    
    //  begin tornament selection and mate the winner
    for (i = GA_ELITSIZE; i < GA_POPSIZE; i += 2)
    {
        p1 = tornament(pop);
        p2 = tornament(pop);
        randint = rand() % TARGET_LEN;
        
        // The first half of the chromosomes don't change
        for (j = 0; j < randint; j++)
        {
            buf[i    ].sol[j] = p1.sol[j];
            buf[i + 1].sol[j] = p2.sol[j];
        }
        // The second half of the chromosomes are swapped
        for (j = randint; j < TARGET_LEN; j++)
        {
            buf[i    ].sol[j] = p2.sol[j];
            buf[i + 1].sol[j] = p1.sol[j];
        }
    }
    
    mutate(buf); /* Add a bit of random mutation in the solutions,
                  'the spice of life' ;D */
}

// Swap pt1 and pt2
void swap_pts(ga_memb **pt1, ga_memb **pt2)
{
    ga_memb *pt_tmp;
    
    pt_tmp = *pt2;
    *pt2 = *pt1;
    *pt1 = pt_tmp;
}

// Mutates some of the population
void mutate(ga_memb *pop)
{
    int i, randint;
    for (i = 0; i < GA_POPSIZE; i++)
    {
        for (int j = 0; j < TARGET_LEN; j ++){
            if (rand() % 10 < GA_MUTCHANCE)
            {
                randint = rand();
                pop[0].sol[randint % TARGET_LEN] = randint;
            }
        }
    }
}

void cp_mems(ga_memb *src, ga_memb *targ, unsigned int size)
{
    unsigned int i, j;
    
    for (i = GA_POPSIZE - 1; i >= size; i--)
    {
        targ[i].fitness = src[i].fitness;
        for (j = 0; j < TARGET_LEN; j++)
        {
            targ[i].sol[j] = src[i].sol[j];
        }
    }
}
