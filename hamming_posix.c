#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 4
#define M 100
#define N 100
#define L 5000



int hamming_distance( char ** a1,char ** a2, int m, int n, int l);
double gettime(void);
void* parallel_compare(void *thread_args);


 struct args
 {
 	char ** array1;
 	char ** array2;
 	int i,j,l;
 	int count;
 	int distance;
 	int tid;
 	int small_index;
 };



pthread_mutex_t reduction_mx;

int main(int argc, char **argv)
{



	int m = M;
	int n = N;
	int l = L;
	int i,j;
	char * arr1[m];
	char * arr2[n];

	//initalizing rand()
	srand(time(NULL));

	pthread_mutex_init(&reduction_mx,NULL);

	//fill first array
	for(j = 0; j < m; j++)
	{	

		if((arr1[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");

		//random generated character starting with ASCI 'space' (32-126)
		for(i = 0; i < l; i++)
			arr1[j][i] = '0' + rand() % 2;
	
	}
	//fill second array
	for(j = 0; j < n; j++)
	{
		if((arr2[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");
		for(i = 0; i < l; i++)
			arr2[j][i] = '0' + rand() % 2;
	}



	//int str_distance[m*n];


	double t1 = gettime();
	printf("temp: %d\n",hamming_distance(arr1,arr2,m,n,l));
	double t2 = gettime();
	printf("%f\n\n",(t2-t1));


	// for(int h = 0; h < m; h++)
	// 	printf("%s\n",*(arr1+h));
	// for(int h = 0; h < n; h++)		
	// 	printf("%s\n",*(arr2+h));

	// for(int h = 0; h<m*n;h++)
	//  	printf("%d_", distance[h]);
	

	// int dist = 0;
	// for(int h = 0; h<m*n;h++)
	// 	dist += str_distance[h];
	 
	//printf("_%d_", dist);



	return 0;
}
/**	
args: 2 arrays of char pointers, sizes of arrays
ret.val : calculated hamming distance

**/	

int hamming_distance(char ** a1,char ** a2, int m, int n, int l)
{
	int chunk=10;
	int i,j,k = 0;
	int offset = -1;
 	int min,max = 0;
 	int count=0;
 	int small_index = 0;
 	int temp =0;
 	// for(int init = 0; init < l;init++)
 	// 	str_distance[init]=0;




 	//omp_set_num_threads(NUM_THREADS);
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
			//count = 0;
			struct args * args_array;
			args_array = (struct args*)malloc(NUM_THREADS*sizeof(struct args));

			//printf("dist1: %d\n",str_distance[offset]);


			pthread_t threads[NUM_THREADS];

			int t,rc;
			for(t = 0; t < NUM_THREADS; t++){



				args_array[t].array1 = a1;
			 	args_array[t].array2 = a2;
			 	args_array[t].i = i;
			 	args_array[t].j = j;
			 	args_array[t].l = l;
			 	args_array[t].distance = 0;
			 	args_array[t].count = 0;
			 	args_array[t].tid=t;
			 	args_array[t].small_index=small_index;

				rc = pthread_create(&threads[t],NULL,parallel_compare,&args_array[t]);
				if(rc){
					printf("ERROR; return code from pthread_create() is %d\n", rc);
					exit(-1);
				}
			}

			for(t = 0; t < NUM_THREADS; t++)
				pthread_join(threads[t],NULL);


			//printf("offset: %d\n",offset);
			//printf("dist: %d\n",str_distance[offset]);

			for(k=0;k<NUM_THREADS;k++){
				//printf("%d_",args_array[k].distance);
				temp += args_array[k].distance;
			}

			//printf("dist: %d\n",str_distance[offset]);



			//printf("\n");
			//printf("out:%d_",count);
		}
	}
	return temp;
}

void* parallel_compare(void *thread_args)
{
	struct args * data;
			data = (struct args*)malloc(sizeof(struct args));


	data = (struct args*)thread_args;
	char ** a1 = data->array1;
	char ** a2 = data->array2;
	int i = data->i;
	int j = data->j;
	int l = data->l;
	int distance = data->distance;
	int count=data->count;
	int tid=data->tid;
	int small_index=data->small_index;

	int k;

	for(k=tid;k<l;k=k+NUM_THREADS)
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
	//pthread_barrier_wait(&for_loop);
	pthread_mutex_lock(&reduction_mx);
	distance+=count;
	pthread_mutex_unlock(&reduction_mx);
	//printf("%d_",distance);
	data->distance=distance;
	return 0;

	//printf("%d\n", );

	// #pragma omp for schedule(static,chunk) reduction(+:count) 
	// 			for(k = 0; k < l; k++)
	// 			{	
	// 				if(small_index)
	// 				{
	// 					if(a1[i][k] != a2[j][k])
	// 						count++;
	// 				}
	// 				else
	// 				{
	// 					if(a2[i][k] != a1[j][k])
	// 						count++;
	// 				}
	// 			}

	// 			//printf("%d_",count);
	// 			distance[offset] = count;
}
double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}
