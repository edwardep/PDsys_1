#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>


int* hamming_distance( char ** a1,char ** a2, int m, int n, int l);
double gettime(void);

int NUM_THREADS;

int main(int argc, char **argv)
{

	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	int l = atoi(argv[3]);
	NUM_THREADS = atoi(argv[4]);

	int i,j;

	char ** arr1;
	char ** arr2;

	//initalizing rand()
	srand(time(NULL));

	arr1=(char**)malloc(m*sizeof(char*));
	arr2=(char**)malloc(n*sizeof(char*));
	//fill first array
	for(j = 0; j < m; j++)
	{	

		if((arr1[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");

		//random generated character starting with ASCI 'space' (32-126)
		// for(i = 0; i < l; i++)
		// 	arr1[j][i] = ' ' + rand() % 94;
		for(i = 0; i < l; i++)
			arr1[j][i] = '0' + rand() % 2;
			//arr1[j] = "0000000000";
	}
	//fill second array
	for(j = 0; j < n; j++)
	{
		if((arr2[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");
		// for(i = 0; i < l; i++)
		// 	arr2[j][i] = ' ' + rand() % 94;
		for(i = 0; i < l; i++)
			arr2[j][i] = '0' + rand() % 2;
			//arr2[j] = "1111111111";
	}



	int *distance;
	double t1 = gettime();
	distance=hamming_distance(arr1,arr2,m,n,l);
	double t2 = gettime();
	printf("time:%f\n\n",(t2-t1));


	// for(int h = 0; h < m; h++)
	// 	printf("%s\n",*(arr1+h));
	// for(int h = 0; h < n; h++)		
	// 	printf("%s\n",*(arr2+h));

	// for(int h = 0; h<m*n;h++)
	//  	printf("%d_", distance[h]);
	long long dist = 0;
	for(int h = 0; h<m*n;h++)
		dist += distance[h];
	 
	printf("sum:: %lld\n", dist);



	return 0;
}
/**	
args: 2 arrays of char pointers, sizes of arrays
ret.val : calculated hamming distance

**/	

int* hamming_distance(char ** a1,char ** a2, int m, int n, int l)
{
	int chunk=10;
	int i,j,k = 0;
	int offset = -1;
 	int min,max = 0;
 	int count=0;
 	int small_index = 0;

 	int *distance=(int*)malloc(m*n*sizeof(int));

 	omp_set_num_threads(NUM_THREADS);
 	if(m < n)
 	{
 		small_index = 1;
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
			count = 0;
			#pragma omp parallel 
			{
				int k;

				#pragma omp for reduction(+:count)
				for(k = 0; k < l; k++)
				{	
					if(small_index)
					{
						if(a1[i][k] != a2[j][k])
							count++;
					}
					else
					{
						if(a2[i][k] != a1[j][k])
							count++;
					}
				}
				//barrier is implied at this point (end of #pragma for) 

				//printf("%d_",count);
				distance[offset] = count;
			}
			//printf("\n");
			//printf("out:%d_",count);
		}
	}
	return distance;
}
double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}
