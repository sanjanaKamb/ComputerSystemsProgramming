/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 ****************************************************************************/
#include <pthread.h>
#include "life.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
		char* temp = b1; \
		b1 = b2; \
		b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])
#define MIN(a,b) (((a)<(b))?(a):(b))
#define NUM_THREADS 8

struct arguments{
	int nrows;
	int ncols;
	int threadNum;
	char* outboard;
	char* inboard;
	pthread_barrier_t* barrp;
	int gens_max;
};

typedef struct arguments structArgs;

void* loop_parellize(void* arg){
	// printf("%d\n", 6);
	structArgs *a;
	a = (structArgs*) arg;
	int nrows = a->nrows;
	char* outboard = a->outboard;
	char* inboard = a->inboard;
	int threadNum = a->threadNum;
	int ncols = a->ncols;
	int gens_max = a->gens_max;

	int initial_i = threadNum*(nrows/NUM_THREADS);
	int maximum_i = initial_i + (nrows/NUM_THREADS);
	int i,j;
	/*printf("START\n");
  printf("nrows- %d, ncols- %d, threadNum- %d, initial_iTOmaximum_i- %d-%d \n", nrows,ncols,threadNum,initial_i, maximum_i);
  printf("END\n");*/
	const int LDA = nrows;
	/* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
	// 1) loop switching from cache related

	int vari = 128;
	int varj = 32;

	//if(ncols<512){
	//int var=ncols/2;
	//}

	int j2,i2,curgen;
	int iself, inw, in, ine, iw, ie, isw, is, ise, in2, isw2, is2, ise2;
	for (curgen = 0; curgen < gens_max; curgen++)
	{
		for (j = 0; j < ncols; j+=varj)
		{//printf("%d\n", 3);
			//3)
			for(j2=j;j2<j+varj;j2++){
				const int jwest = (j2 == 0)? ncols-1: j2-1;
				const int jeast = (j2 == ncols-1)? 0 : j2+1;
				for (i = initial_i; i < maximum_i; i+=vari)
				{
					//printf("%d, %d, %d, %d, %d\n", 7,initial_i,maximum_i, i, j);
					for(i2=i; i2<i+vari;i2++){
						const int inorth = (i2==0) ? nrows-1 : i2-1;
						const int isouth = (i2==nrows-1) ? 0 : i2+1;
						// printf("jwest-%d, jeast-%d, inorth-%d, isouth-%d, ThreadNum-%d\n", jwest,jeast,inorth,isouth,threadNum);
						//printf("%d\n", 8);
						if(i2 == i){
							inw = BOARD (inboard, inorth, jwest);
							in = BOARD (inboard, inorth, j2);
							ine = BOARD (inboard, inorth, jeast);
							iw = BOARD (inboard, i2, jwest);
							iself = BOARD (inboard, i2, j2);
							ie = BOARD (inboard, i2, jeast);
							isw = BOARD (inboard, isouth, jwest);
							is = BOARD (inboard, isouth, j2);
							ise = BOARD (inboard, isouth, jeast);
						}
						else{
							inw = iw;
							in = iself;
							ine = ie;
							iw = isw;
							iself = is;
							ie = ise;
							isw = BOARD (inboard, isouth, jwest);
							is = BOARD (inboard, isouth, j2);
							ise = BOARD (inboard, isouth, jeast);
						}

						const char neighbor_count = inw + in + ine + iw + ie + isw + is + ise;

						//  printf("%d\n", neighbor_count);
						//printf("%d\n", 9);
						BOARD(outboard, i2, j2) = alivep (neighbor_count, iself);
						//printf("%d\n", 10);
					}
				}
			}
		}
		pthread_barrier_wait(a->barrp);
		SWAP_BOARDS( outboard, inboard );
	}
	// printf("%d\n", 11);
	pthread_exit(0);
}

char*
threadless_game_of_life (char* outboard, 
		char* inboard,
		const int nrows,
		const int ncols,
		const int gens_max)
{
	/* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
	const int LDA = nrows;
	int curgen, i, j;

	for (curgen = 0; curgen < gens_max; curgen++)
	{
		/* HINT: you'll be parallelizing these loop(s) by doing a
           geometric decomposition of the output */
		for (j = 0; j < ncols; j++)
		{
			for (i = 0; i < nrows; i++)
			{
				const int inorth = mod (i-1, nrows);
				const int isouth = mod (i+1, nrows);
				const int jwest = mod (j-1, ncols);
				const int jeast = mod (j+1, ncols);

				printf("jwest-%d, jeast-%d, inorth-%d, isouth-%d\n", jwest,jeast,inorth,isouth);
				const char neighbor_count =
						BOARD (inboard, inorth, jwest) +
						BOARD (inboard, inorth, j) +
						BOARD (inboard, inorth, jeast) +
						BOARD (inboard, i, jwest) +
						BOARD (inboard, i, jeast) +
						BOARD (inboard, isouth, jwest) +
						BOARD (inboard, isouth, j) +
						BOARD (inboard, isouth, jeast);
				printf("%d\n",neighbor_count);
				BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

			}
		}
		SWAP_BOARDS( outboard, inboard );

	}
	/*
	 * We return the output board, so that we know which one contains
	 * the final result (because we've been swapping boards around).
	 * Just be careful when you free() the two boards, so that you don't
	 * free the same one twice!!!
	 */
	return inboard;
}

/*****************************************************************************
 * Game of life implementation
 ****************************************************************************/
char*
game_of_life (char* outboard, 
		char* inboard,
		const int nrows,
		const int ncols,
		const int gens_max)
{
	if(nrows<32 || ncols<32){
		return threadless_game_of_life(outboard, inboard, nrows, ncols, gens_max);
	}

	// printf("%d\n", 1);
	/* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
	const int LDA = nrows;

	// printf("%d\n", 2);




	// printf("%d\n", 4);

	pthread_barrier_t barro;
	pthread_t tid[NUM_THREADS];
	int m,n;
	pthread_barrier_init(&barro, NULL, NUM_THREADS);

	for(m=0; m<NUM_THREADS; m++){
		structArgs *arg;
		arg = (structArgs *)malloc(sizeof(structArgs));
		arg->nrows = nrows;
		arg->outboard = outboard;
		arg->inboard = inboard;
		arg->ncols = ncols;
		arg->threadNum=m;
		arg->barrp = &barro;
		arg->gens_max=gens_max;
		//   printf("M-%d\n", m);
		pthread_create(&tid[m],NULL,loop_parellize,(void *)arg);

	}

	for(n=0; n<NUM_THREADS; n++){
		// printf("N-%d\n", n);
		pthread_join(tid[n],NULL);
	}

	/*
	 * We return the output board, so that we know which one contains
	 * the final result (because we've been swapping boards around).
	 * Just be careful when you free() the two boards, so that you don't
	 * free the same one twice!!!
	 */
	return inboard;
}
