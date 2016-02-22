/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

/* 
 * ECE454 Students: 
 * Please fill in the following team struct 
 */
team_t team = {
    "Null Terminators",              /* Team name */

    "MD Rayun Mehrab",     /* First member full name */
    "rayun.mehrab@mail.utoronto.ca",  /* First member email address */

    "Sanjana Kambalapally",                   /* Second member full name (leave blank if none) */
    "sanjana.kambalapally@mail.utoronto.ca"                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}




// second attempt (commented out for now)
char rotate_two_descr[] = "second attempt";
void attempt_two(int dim, pixel *src, pixel *dst) 
{
	  int i, j, i_dim ;
	  int dim_square_minus = dim*dim-dim;

	    for (i = 0; i < dim; i++){
	    	i_dim = i*dim;
	    	for (j = 0; j < dim; j++){
	    		dst[dim_square_minus-j*dim + i] = src[i_dim + j];
	    	}
	    }


}


// third attempt - 1.3
char rotate_three_descr[] = "third attempt";
void attempt_three(int dim, pixel *src, pixel *dst)
{
	 int i, j;

	    for (j = 0; j < dim; j++)
		for (i = 0; i < dim; i++)
		    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];

}


// fourth attempt - 1.2-1.4
char rotate_four_descr[] = "fourth attempt";
void attempt_four(int dim, pixel *src, pixel *dst)
{

	int i, j;

		    for (j = 0; j < dim; j++){
		    	for (i = 0; i < dim; i+=32){
		    		dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
		    		dst[RIDX(dim-1-j, i+1, dim)] = src[RIDX(i+1, j, dim)];
		    		dst[RIDX(dim-1-j, i+2, dim)] = src[RIDX(i+2, j, dim)];
		    		dst[RIDX(dim-1-j, i+3, dim)] = src[RIDX(i+3, j, dim)];
		    		dst[RIDX(dim-1-j, i+4, dim)] = src[RIDX(i+4, j, dim)];
		    		dst[RIDX(dim-1-j, i+5, dim)] = src[RIDX(i+5, j, dim)];
		    		dst[RIDX(dim-1-j, i+6, dim)] = src[RIDX(i+6, j, dim)];
		    		dst[RIDX(dim-1-j, i+7, dim)] = src[RIDX(i+7, j, dim)];
		    		dst[RIDX(dim-1-j, i+8, dim)] = src[RIDX(i+8, j, dim)];
		    		dst[RIDX(dim-1-j, i+9, dim)] = src[RIDX(i+9, j, dim)];
		    		dst[RIDX(dim-1-j, i+10, dim)] = src[RIDX(i+10, j, dim)];
		    		dst[RIDX(dim-1-j, i+11, dim)] = src[RIDX(i+11, j, dim)];
		    		dst[RIDX(dim-1-j, i+12, dim)] = src[RIDX(i+12, j, dim)];
		    		dst[RIDX(dim-1-j, i+13, dim)] = src[RIDX(i+13, j, dim)];
		    		dst[RIDX(dim-1-j, i+14, dim)] = src[RIDX(i+14, j, dim)];
		    		dst[RIDX(dim-1-j, i+15, dim)] = src[RIDX(i+15, j, dim)];
		    		dst[RIDX(dim-1-j, i+16, dim)] = src[RIDX(i+16, j, dim)];
		    		dst[RIDX(dim-1-j, i+17, dim)] = src[RIDX(i+17, j, dim)];
		    		dst[RIDX(dim-1-j, i+18, dim)] = src[RIDX(i+18, j, dim)];
		    		dst[RIDX(dim-1-j, i+19, dim)] = src[RIDX(i+19, j, dim)];
		    		dst[RIDX(dim-1-j, i+20, dim)] = src[RIDX(i+20, j, dim)];
		    		dst[RIDX(dim-1-j, i+21, dim)] = src[RIDX(i+21, j, dim)];
		    		dst[RIDX(dim-1-j, i+22, dim)] = src[RIDX(i+22, j, dim)];
		    		dst[RIDX(dim-1-j, i+23, dim)] = src[RIDX(i+23, j, dim)];
		    		dst[RIDX(dim-1-j, i+24, dim)] = src[RIDX(i+24, j, dim)];
		    		dst[RIDX(dim-1-j, i+25, dim)] = src[RIDX(i+25, j, dim)];
		    		dst[RIDX(dim-1-j, i+26, dim)] = src[RIDX(i+26, j, dim)];
		    		dst[RIDX(dim-1-j, i+27, dim)] = src[RIDX(i+27, j, dim)];
		    		dst[RIDX(dim-1-j, i+28, dim)] = src[RIDX(i+28, j, dim)];
		    		dst[RIDX(dim-1-j, i+29, dim)] = src[RIDX(i+29, j, dim)];
		    		dst[RIDX(dim-1-j, i+30, dim)] = src[RIDX(i+30, j, dim)];
		    		dst[RIDX(dim-1-j, i+31, dim)] = src[RIDX(i+31, j, dim)];

		    	}
		    }

}

// five attempt -2.3-2.4
char rotate_five_descr[] = "fifth attempt";
void attempt_five(int dim, pixel *src, pixel *dst)
{
	 int i, j, i2, j2;

	    for (j = 0; j < dim; j+=32){
	    	for (i = 0; i < dim; i+=32){
	    		for(j2=j;j2<MIN(j+32,dim);j2++){
	    				for(i2=i; i2<MIN(i+32,dim);i2++){
	    					dst[RIDX(dim-1-j2, i2, dim)] = src[RIDX(i2, j2, dim)];
	    				}
	    		}
	    	}
	    }

}

//2048 -16 , 512 - 32
// six attempt - 2.4-2.5
char rotate_six_descr[] = "sixth attempt";
void attempt_six(int dim, pixel *src, pixel *dst)
{

	 int i, j, i2, j2;

	 int var = 32;
	 int var2 = 32;
	 if(dim==2048 ){
		 var2 = 16;
	 }
	 else if(dim==512 || dim==1024 ||dim==4096){
	 		 var2 = 32;
	 	 }
	 else if(dim>=256){
		 var2 = 32;
	 }
	    for (j = 0; j < dim; j+=var){
	    	for (i = 0; i < dim; i+=var2){
	    		for(j2=j;j2<MIN(j+var,dim);j2++){
	    				for(i2=i; i2<MIN(i+var2,dim);i2++){
	    					dst[RIDX(dim-1-j2, i2, dim)] = src[RIDX(i2, j2, dim)];
	    				}
	    		}
	    	}
	    }

}


// seventh attempt - 2.5
char rotate_seven_descr[] = "seventh attempt";
void attempt_seven(int dim, pixel *src, pixel *dst)
{

	 int i, j, i2, j2;

	 int var = 32;
	 int var2 = 32;
	 if(dim==2048 ){
		 var2 = 16;
	 }
	 else if(dim==512 || dim==1024 ||dim==4096){
	 		 var2 = 32;
	 	 }
	 else if(dim>=256){
		 var2 = 32;
	 }

	 if(dim==2048 || dim==1024){
		 var = 32;
	 }else if(dim >= 128){
		 var = 64;
	 }


	    for (j = 0; j < dim; j+=var){
	    	for (i = 0; i < dim; i+=var2){
	    		for(j2=j;j2<MIN(j+var,dim);j2++){
	    				for(i2=i; i2<MIN(i+var2,dim);i2++){
	    					dst[RIDX(dim-1-j2, i2, dim)] = src[RIDX(i2, j2, dim)];
	    				}
	    		}
	    	}
	    }

}

// eighth attempt - 2.5-2.6
char rotate_eight_descr[] = "eighth attempt";
void attempt_eight(int dim, pixel *src, pixel *dst)
{

	 int i, j, i2, j2;

	 int var = 32;
	 int var2 = 32;


	 if(dim==2048 ||dim==4096){
		 var2 = 16;
	 }
	 else if(dim==512 || dim==1024 ){
	 		 var2 = 32;
	 	 }
	 else if(dim>=256){
		 var2 = 32;
	 }

	 if(dim==2048 || dim==1024){
		 var = 1024;
	 }else if(dim==4096){
		 var = 512;
	 }

	    for (j = 0; j < dim; j+=var){
	    	for (i = 0; i < dim; i+=var2){
	    		for(j2=j;j2<MIN(j+var,dim);j2++){
	    				for(i2=i; i2<MIN(i+var2,dim);i2++){
	    					dst[RIDX(dim-1-j2, i2, dim)] = src[RIDX(i2, j2, dim)];
	    				}
	    		}
	    	}
	    }

}

// ninth attempt - trying code motion, breaking down RIDX funct - 2.5-2.7. Basically about fractions of 0.1 better than attempt_eight, so when attempt_eight is reeeeally close to 2.7 but displays 2.6 instead, this attempt takes it over the line.
char rotate_nine_descr[] = "ninth attempt";
void attempt_nine(int dim, pixel *src, pixel *dst)
{

	 int i, j, i2, j2;

	 int var = 32;
	 int var2 = 32;


	 if(dim==2048 ){
		 var2 = 16;
	 }
	 else if(dim==512 || dim==1024 ||dim==4096){
	 		 var2 = 32;
	 	 }
	 else if(dim>=256){
		 var2 = 32;
	 }

	 if(dim==2048 || dim==1024){
		 var = 1024;
	 }else if(dim==4096){
		 var = 512;
	 }
	
	//Separating at 512 because attempt_eight showed better results for less than 512.
	if(dim<=1024){

	    for (j = 0; j < dim; j+=var){
	    	for (i = 0; i < dim; i+=var2){
	    		for(j2=j;j2<MIN(j+var,dim);j2++){
	    				for(i2=i; i2<MIN(i+var2,dim);i2++){
	    					dst[RIDX(dim-1-j2, i2, dim)] = src[RIDX(i2, j2, dim)];
	    				}
	    		}
	    	}
	    }
	}
	else{

	    for (j = 0; j < dim; j+=var){
	    	for (i = 0; i < dim; i+=var2){
	    		for(j2=j;j2<MIN(j+var,dim);j2++){
					int dst_jn = (dim-1-j2)*dim;
    				for(i2=i; i2<MIN(i+var2,dim);i2++){
    					dst[dst_jn+i2] = src[(i2*dim)+j2];
    				}
	    		}
	    	}
	    }
	}

}


char rotate_ten_descr[] = "tenth attempt";
void attempt_ten(int dim, pixel *src, pixel *dst)
{

	 int i, j, i2, j2, jj2;

	 int var = 32;
	 int var2 = 32;


	 if(dim==2048 ){
		 var2 = 16;
	 }
	 else if(dim==512 || dim==1024 ||dim==4096){
	 		 var2 = 32;
	 	 }
	 else if(dim>=256){
		 var2 = 32;
	 }

	 if(dim==2048 || dim==1024){
		 var = 1024;
	 }else if(dim==4096){
		 var = 512;
	 }
	
	if(dim<=1024){

	    for (j = 0; j < dim; j+=var){
	    	for (i = 0; i < dim; i+=var2){
	    		for(j2=j;j2<MIN(j+var,dim);j2++){
	    				for(i2=i; i2<MIN(i+var2,dim);i2++){
	    					dst[RIDX(dim-1-j2, i2, dim)] = src[RIDX(i2, j2, dim)];
	    				}
	    		}
	    	}
	    }
	}
	else{

	    for (j = 0; j < dim; j+=var){
			int dst_jn[var];
			for(jj2=j;jj2<MIN(j+var,dim);jj2++){
				dst_jn[jj2%var] = (dim-1-jj2)*dim;
			}
			for (i = 0; i < dim; i+=var2){
	    		for(j2=j;j2<MIN(j+var,dim);j2++){
    				for(i2=i; i2<MIN(i+var2,dim);i2++){
    					dst[dst_jn[j2%var]+i2] = src[(i2*dim)+j2];
    				}
	    		}
	    	}
	    }
	}

}

char rotate_eleven_descr[] = "11 attempt";
void attempt_eleven(int dim, pixel *src, pixel *dst)
{

	 int i, j, i2, j2;

		 int var = 32;
		 int var2 = 32;


		 if(dim==2048 ||dim==4096){
			 var2 = 16;
		 }
		 else if(dim==512 || dim==1024 ){
		 		 var2 = 32;
		 	 }
		 else if(dim==256 || dim==128){
			 var2 = 32;
		 }
/*
		 if(dim==2048 || dim==1024){
			 var = 8;
		 }else if(dim==4096){
			 var = 8;
		 }*/

		    for (j = 0; j < dim; j+=var){
		    	for (i = 0; i < dim; i+=var2){
		    		for(j2=j;j2<MIN(j+var,dim);j2++){
		    				for(i2=i; i2<MIN(i+var2,dim);i2++){
		    					dst[RIDX(dim-1-j2, i2, dim)] = src[RIDX(i2, j2, dim)];
		    				}
		    		}
		    	}
		    }


}

/*
 * ECE 454 Students: Write your rotate functions here:
 */ 

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version";
void rotate(int dim, pixel *src, pixel *dst) 
{
    attempt_eight(dim, src, dst);
}



/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
   add_rotate_function(&naive_rotate, naive_rotate_descr);
    add_rotate_function(&rotate, rotate_descr);

    //add_rotate_function(&attempt_two, rotate_two_descr);
   // add_rotate_function(&attempt_three, rotate_three_descr);
    //add_rotate_function(&attempt_four, rotate_four_descr);
    //add_rotate_function(&attempt_five, rotate_five_descr);
   //add_rotate_function(&attempt_six, rotate_six_descr);
    //add_rotate_function(&attempt_seven, rotate_seven_descr);
    //add_rotate_function(&attempt_eight, rotate_eight_descr);
    //add_rotate_function(&attempt_nine, rotate_nine_descr);   
    //add_rotate_function(&attempt_ten, rotate_ten_descr);   
   // add_rotate_function(&attempt_eleven, rotate_eleven_descr);

    /* ... Register additional rotate functions here */
}

