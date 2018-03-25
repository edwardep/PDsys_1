#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

int* hamming_distance(char ** a1,char ** a2, int m, int n, int l);
double gettime(void);
void* parallel_compare(void *thread_args);

int NUM_THREADS;
int OFFSET = -1;
 struct args
 {
 	char ** array1;
 	char ** array2;
 	int i,l;
 	int min,max;
 	int tid;
 	int small_index;
 };

pthread_mutex_t reduction_mx;

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
	srand(time(NULL)+atoi(argv[5])+6);

	arr1=(char**)malloc(m*sizeof(char*));
	arr2=(char**)malloc(n*sizeof(char*));

	pthread_mutex_init(&reduction_mx,NULL);

	//fill first array
	for(j = 0; j < m; j++)
	{	

		if((arr1[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");

		//random generated character starting with ASCI 'space' (32-126)
		for(i = 0; i < l; i++)
			arr1[j][i] = ' ' + rand() % 94;
		//second implementation of generating character with 0,1
		// for(i = 0; i < l; i++)
		// 	arr1[j][i] = '0' + rand() % 2;
			
	}
	//fill second array
	for(j = 0; j < n; j++)
	{
		if((arr2[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");
		for(i = 0; i < l; i++)
			arr2[j][i] = ' ' + rand() % 94;
		// for(i = 0; i < l; i++)
		// 	arr2[j][i] = '0' + rand() % 2;
			
	}


	//Printing total time and distance
	printf("\n----- Hamming POSIX multi");
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
	int i = 0;
	//int offset;
 	int min,max = 0;
 	int small_index = 0;


 	int * distance=(int*)malloc(m*n*sizeof(int));


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


 	//offset = -1;
	for(i = 0; i < min; i++)
	{	

		struct args * args_array;
		args_array = (struct args*)malloc(NUM_THREADS*sizeof(struct args));
		pthread_t threads[NUM_THREADS];

		int t,rc;
		for(t = 0; t < NUM_THREADS; t++)
		{
			args_array[t].array1 = a1;
			args_array[t].array2 = a2;
			args_array[t].i = i;
			args_array[t].l = l;
			args_array[t].min = min;
			args_array[t].max = max;
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

		for(t = 0; t < NUM_THREADS; t++)
		{
			rc = pthread_join(threads[t],(void**)&retval);
			//printf("waiting here.__________________________\n");
			if(rc){
				printf("ERROR; return code from pthread_join() is %d\n", rc);
				exit(-1);
			}
			
			for(int w = 0; w < max*min; w++){
				//printf("__________i*max+w = %d, tid:%d\n",i*max+w,t);
				distance[w] += retval[w];
			}
			free(retval);
		}

		//for(int g = 0; g < max; g++)printf("retval[%d]: %d\n",g,retval[g]);

		free(args_array);	
	}
	return distance;
}

void* parallel_compare(void *thread_args)
{
	struct args * data;
	data = (struct args*)malloc(sizeof(struct args));


	data = (struct args*)thread_args;
	char ** a1 = data->array1;
	char ** a2 = data->array2;
	int i = data->i;
	int l = data->l;
	int min = data->min;
	int max = data->max;
	int tid = data->tid;
	int small_index=data->small_index;

	int * distance = (int*)malloc(max*min*sizeof(int));
	int g;
	for(g = 0; g < min*max; g++) distance[g] = 0;

	int count;
	int j,k;
	int start,stop;
	int chunk = max/NUM_THREADS;
	start=tid*chunk;


	if(tid != (NUM_THREADS-1))
		stop =(tid+1)*chunk;
	else
		stop =  max;


	for(j = start; j < stop; j++)
	{
		count=0;

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
		//printf("j:%d\n",j);
		pthread_mutex_lock(&reduction_mx);
		OFFSET++;
		distance[OFFSET] = count;
		pthread_mutex_unlock(&reduction_mx);
	}

	
	//printf("Thread %d returns ",tid);
	
	// for(j=0;j<max;j++)
	// 	printf("%d__",distance[j]);
	// printf("\n");


	int *answer = (int*)malloc(max*min*sizeof(int));
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
