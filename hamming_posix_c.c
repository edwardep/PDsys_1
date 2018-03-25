#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

int* hamming_distance(char ** a1,char ** a2, int m, int n, int l);
double gettime(void);
void* parallel_compare(void *thread_args);

int NUM_THREADS;
 struct args
 {
 	char ** array1;
 	char ** array2;
 	int l;
 	int min;
 	int max;
 	int offset;
 	int tid;
 	int small_index;
 };

int OFFSET = -1;
pthread_mutex_t reduction_mx;

int main(int argc, char **argv)
{
	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	int l = atoi(argv[3]);
	NUM_THREADS = atoi(argv[4]);

	//initalizing rand()
	srand(time(NULL)+atoi(argv[5])+7);

	char ** arr1 = (char**)malloc(m*sizeof(char*));
	char ** arr2 = (char**)malloc(n*sizeof(char*));

	int i,j;

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
	pthread_mutex_init(&reduction_mx,NULL);

	//Printing total time and distance
	printf("\n----- Hamming POSIX coarse");
	int *distance;
	double t1 = gettime();
	distance = hamming_distance(arr1,arr2,m,n,l);
	double t2 = gettime();
	printf("\ntime:    %f\n",(t2-t1));


	long long dist = 0;
	for(int h = 0; h<m*n;h++)
		dist += distance[h];
	 
	printf("hamming: %lld\n", dist);

	pthread_exit(NULL);
	return 0;
}
/**	
args: 2 arrays of char pointers, sizes of arrays
ret.val : calculated hamming distance

**/	
int* hamming_distance(char ** a1,char ** a2, int m, int n, int l)
{
	int offset;
 	int min,max = 0;
 	int small_index = 0;


 	int * distance=(int*)malloc(m*n*sizeof(int));

 	//Setting the correct array limits
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

 	//PARALLEL SECTION begins
 		//Allocate memory for "arguments structure"
		struct args * args_array;
		args_array = (struct args*)malloc(NUM_THREADS*sizeof(struct args));

		pthread_t threads[NUM_THREADS];

		int t,rc;
		//For each thread:
		//	  pass arguments to struct
		//	  call pthread_create(thread_id,null,thread_task,arguments_struct)
		for(t = 0; t < NUM_THREADS; t++)
		{
			args_array[t].array1 = a1;
			args_array[t].array2 = a2;
			args_array[t].min = min;
			args_array[t].max = max;
			args_array[t].l = l;
			args_array[t].offset = -1;
			args_array[t].tid = t;
			args_array[t].small_index = small_index;
			

			rc = pthread_create(&threads[t],NULL,parallel_compare,&args_array[t]);
			if(rc)
			{
				printf("ERROR; return code from pthread_create() is %d\n", rc);
				exit(-1);
			}	
		}	
		int * retval;
		//For each thread:
		//	  call pthread_join(thread_id,return_value)
		//	  add partial count to distance array
		//	  free memory
		for(t = 0; t < NUM_THREADS; t++)
		{
			rc = pthread_join(threads[t],(void**)&retval);
			if(rc){
				printf("ERROR; return code from pthread_join() is %d\n", rc);
				exit(-1);
			}
			for(int w = 0; w < m*n; w++)
				distance[w] += retval[w];

			free(retval);
		}
		free(args_array);
	//PARALLEL SECTION ends
	return distance;
}
/**
	A thread's task.
	Passed argument is a struct of arguments
**/
void* parallel_compare(void *thread_args)
{
	//Declaring local struct and retrieving arguments
	struct args * data;
	data = (struct args*)malloc(sizeof(struct args));


	data = (struct args*)thread_args;
	char ** a1 = data->array1;
	char ** a2 = data->array2;
	int min = data->min;
	int l = data->l;
	int tmp_off = data->offset;
	int max = data->max;
	int tid = data->tid;
	int small_index=data->small_index;

	//declaring & init local variables
	int * distance = (int*)malloc(max*min*sizeof(int));
	int g; for(g = 0; g < min*max; g++) distance[g] = 0;

	int count;
	int i,j,k;

	//setting up workshare loop 
	int start,stop;
	int chunk = min/NUM_THREADS;
	start = tid*chunk;
	if(tid != (NUM_THREADS-1))
		stop =(tid+1)*chunk;
	else
		stop =  min;

	//Iterating through arrays A and B
	for(i = start; i < stop; i++)
	{
		for(j = 0; j < max; j++)
		{
			count=0;
			//Comparing strings
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
			//Mutex lock before changing value of shared memory variable
			pthread_mutex_lock(&reduction_mx);
			OFFSET++;
			distance[OFFSET] = count;
			pthread_mutex_unlock(&reduction_mx);
		}
	}
	//passing result to pthread_exit()
	int *answer = (int*)malloc(min*max*sizeof(int));
	answer = distance;
	pthread_exit(answer);
	return 0;
}




double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}
