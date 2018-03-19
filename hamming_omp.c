#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>


#define NUM_THREADS 2

int hamming_distance(int * distance, char ** a1,char ** a2, int m, int n, int l);

double gettime(void);

int main(char argc, void* argv[])
{



	int m = 2;
	int n = 3;
	int l = 20;
	int i,j;
	char * arr1[m];
	char * arr2[n];

	//initalizing rand()
	srand(time(NULL));

	//fill first array
	for(j = 0; j < m; j++)
	{	

		if((arr1[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");

		//random generated character starting with ASCI 'space' (32-126)
		for(i = 0; i < l; i++)
			arr1[j][i] = ' ' + rand() % 94;
	
	}
	//fill second array
	for(j = 0; j < n; j++)
	{
		if((arr2[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");
		for(i = 0; i < l; i++)
			arr2[j][i] = ' ' + rand() % 94;
	}



	// int distance[m*n];
	// double t1 = gettime();
	// //printf("_%d_",hamming_distance(distance,arr1,arr2,m,n,l));
	// double t2 = gettime();
	// printf("\n%f\n\n",(t2-t1));


	for(int h = 0; h < m; h++)
		printf("%s\n",*(arr1+h));
	for(int h = 0; h < n; h++)		
		printf("%s\n",*(arr2+h));

	// for(int h = 0; h<m*n;h++)
	//  	printf("%d_", distance[h]);



	return 0;
}
/**	
args: 2 arrays of char pointers, sizes of arrays
ret.val : calculated hamming distance

**/	

int hamming_distance(int * distance,char ** a1,char ** a2, int m, int n, int l)
{
	int i,j = 0;
	
	int offset = -1;
 	int min,max = 0;

 	int sum;
 	omp_set_num_threads(NUM_THREADS);

 	if(m < n)
 	{
 		min = m;
 		max = n;
 	}
 	else
 	{
 		min = n;
 		max = m;
 	}

	for(i = 0; i < min; i++)
	{
		for(j = 0; j < max; j++)
		{	


			offset++;
			printf("\n_offset: %d\n",offset);

			#pragma omp parallel
			{
				

				int local_nthreads = omp_get_num_threads();
				int id = omp_get_thread_num();
				int k;

				int count = 0;
			
				for(k = id; k < l; k = k + local_nthreads)
				{
					if(a1[i][k] != a2[j][k])
						count++;
				}
				#pragma omp critical
				sum += count;
				//printf("id: %d, sum: %d\n",id,sum);
				//distance[offset] = count;
			}
		}
	}
	return sum;
}

double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}
