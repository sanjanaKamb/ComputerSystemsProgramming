/*****************************************************************************
 * life.c
 * Parallelized and optimized implementation of the game of life resides here
 * Sanjana Kambalapally - 999147873
 * MD Rayun Mehrab - 999036582
 *
 * The following is an implementation of game of life including some optimizatons.
 * For every generation, 8 pthreads allow paralleization of the code using pthread barriers.
 * A few other optimizatoins such as loop switching, code motion, tiling etc. are 
 * indicated in the code.
 *
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

/**
* Parallelized implementation of the game of life
*/
void* loop_parellize(void* arg){

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

	const int LDA = nrows;

	int vari = nrows/NUM_THREADS;
	int varj = ncols/2;

	int j2,i2,curgen;
	int jself, jnw, jn, jne, jw, je, jsw, js, jse;
	int iself, inw, in, ine, iw, ie, isw, is, ise;
	for (curgen = 0; curgen < gens_max; curgen++)
	{
		// Optimization: loop switching j and i loops
		for (j = 0; j < ncols; j+=varj)
		{
			for (i = initial_i; i < maximum_i; i+=vari)
			{
				// Optimization: Code Motion, Improved formula for inorth and isouth
				const int inorth = (i==0) ? nrows-1 : i-1;
				const int isouth = (i==nrows-1) ? 0 : i+1;

				// Optimization: Tiling
				for(j2=j;j2<j+varj;j2++){

					// Optimization: Improved formula for jwest and jeast
					const int jwest = (j2 == 0)? ncols-1: j2-1;
					const int jeast = (j2 == ncols-1)? 0 : j2+1;

					if(j2 == j){
						// Optimization: Loop iteration memory sharing
						inw = jnw = BOARD (inboard, inorth, jwest);
						in = jn = BOARD (inboard, inorth, j2);
						ine = jne = BOARD (inboard, inorth, jeast);
						iw = jw = BOARD (inboard, i, jwest);
						iself = jself = BOARD (inboard, i, j2);
						ie = je = BOARD (inboard, i, jeast);
						isw = jsw = BOARD (inboard, isouth, jwest);
						is = js = BOARD (inboard, isouth, j2);
						ise = jse = BOARD (inboard, isouth, jeast);
					}
					else{
						//Optimization: Loop iteration memory sharing
						inw = jnw = jn;
						in = jn = jne;
						ine = jne = BOARD (inboard, inorth, jeast);
						iw = jw = jself;
						iself = jself = je;
						ie = je = BOARD (inboard, i, jeast);
						isw = jsw = js;
						is = js = jse;
						ise = jse = BOARD (inboard, isouth, jeast);
					}
					for(i2=i; i2<i+vari;i2++){
						// printf("jwest-%d, jeast-%d, inorth-%d, isouth-%d, ThreadNum-%d\n", jwest,jeast,inorth,isouth,threadNum);
						if(i2>i){
							//Optimization: Loop iteration memory sharing
							const int isouth2 = (i2==nrows-1) ? 0 : i2+1;
							inw = iw;
							in = iself;
							ine = ie;
							iw = isw;
							iself = is;
							ie = ise;
							isw = BOARD (inboard, isouth2, jwest);
							is = BOARD (inboard, isouth2, j2);
							ise = BOARD (inboard, isouth2, jeast);
						}

						const char neighbor_count = inw + in + ine + iw + ie + isw + is + ise;

						//  printf("%d\n", neighbor_count);
						BOARD(outboard, i2, j2) = alivep (neighbor_count, iself);
					}
				}
			}
		}
		// Optimizaton: pthread barrier
		pthread_barrier_wait(a->barrp);
		SWAP_BOARDS( outboard, inboard );
	}
	pthread_exit(0);
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
		return sequential_game_of_life(outboard, inboard, nrows, ncols, gens_max);
	}

	/* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
	const int LDA = nrows;
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
		pthread_create(&tid[m],NULL,loop_parellize,(void *)arg);

	}

	for(n=0; n<NUM_THREADS; n++){
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
