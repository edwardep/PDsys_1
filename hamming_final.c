#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

int* hamming_distance( char ** a1,char ** a2, int m, int n, int l);
double gettime(void);

int main(int argc, char **argv)
{

	int m = atoi(argv[1]);
	int n = atoi(argv[2]);
	int l = atoi(argv[3]);
	int NUM_THREADS = atoi(argv[4]);
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
			//arr1[j] = "00";
			arr1[j][i] = '0' + rand() % 2;
			
	}
	//fill second array
	for(j = 0; j < n; j++)
	{
		if((arr2[j] = (char *)malloc(l * sizeof (char))) == NULL)
			printf("malloc error\n");
		// for(i = 0; i < l; i++)
		// 	arr2[j][i] = ' ' + rand() % 94;
		for(i = 0; i < l; i++)
			//arr2[j] = "11";
			arr2[j][i] = '0' + rand() % 2;
			
	}

	int *distance;
	double t1 = gettime();
	distance=hamming_distance(arr1,arr2,m,n,l);
	double t2 = gettime();
	printf("%f\n\n",(t2-t1));


	// for(int h = 0; h < m; h++)
	// 	printf("%s\n",*(arr1+h));
	// for(int h = 0; h < n; h++)		
	// 	printf("%s\n",*(arr2+h));

	// for(int h = 0; h<m*n;h++)
	//  	printf("dist: %d\n", distance[h]);

	long long dist = 0;
	for(int h = 0; h<m*n;h++)
		dist += distance[h];
	 
	printf("sum:: %lld\n", dist);


	return 0;
}
/**	HAMMING DISTANCE SEQUENTIAL
args: 2 arrays of char pointers, sizes of arrays
ret.val : calculated hamming distance

**/	
int* hamming_distance_seq(char ** a1,char ** a2, int m, int n, int l)
{
	int i,j,k = 0;
	int count = 0;
	int offset = -1;
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

	for(i = 0; i < min; i++)
	{
		for(j = 0; j < max; j++)
		{	
			//printf("i,j:%d_%d\n",i,j);
			offset++;
			count = 0;
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
			distance[offset] = count;
		}
	}
	return distance;
}

/* HAMMING DISTANCE using OMP (fine grained)*/
int* hamming_distance_omp_a(char ** a1,char ** a2, int m, int n, int l)
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
/* HAMMING DISTANCE using OMP (medium grained)*/
int * hamming_distance_omp_b(char ** a1,char ** a2, int m, int n, int l)
{
	int i,j = 0;
 	int min,max = 0;
 	int small_index = 0;
 	int offset;
 	
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
 		

 		offset = -1;
		for(i = 0; i < min; i++)
		{
		#pragma omp parallel
 		{
 			int k,count;

			#pragma omp for
			for(j = 0; j < max; j++)
			{	
				count = 0;
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

				//printf("_____________count: %d\n",count);
				}

				//printf("Synolo:%d,String%d,dist:%d,offset:%d\n",i,j,count,offset);

				#pragma omp critical
				distance[++offset] = count;
		
			//printf("\n");
			//printf("out:%d_",count);
			}
		
		}
	
	}

	return distance;
}
/* HAMMING DISTANCE using OMP (coarse grained)*/
int* hamming_distance_omp_c(char ** a1,char ** a2, int m, int n, int l)
{
	//int i,j,k = 0;
	int k;
 	int min,max = 0;
 	//int count=0;
 	int small_index = 0;
 	int offset;
 	
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

 	offset = -1;
 	#pragma omp parallel
 	{
 		int i,j,k,count;
 		

 		#pragma omp for
		for(i = 0; i < min; i++)
		{
			for(j = 0; j < max; j++)
			{	
				
				count = 0;
			
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

				//printf("Synolo:%d,String%d,dist:%d,offset:%d\n",i,j,count,offset);
				#pragma omp critical
				distance[++offset] = count;	
		
			//printf("\n");
			//printf("out:%d_",count);
			}
		
		}
	
	}

	return distance;
}
/* HAMMING DISTANCE using pthreads (fine grained)*/
int* hamming_distance_posix_a(char ** a1,char ** a2, int m, int n, int l)
{
	int i,j,k = 0;
	int offset = -1;
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

	for(i = 0; i < min; i++)
	{
		for(j = 0; j < max; j++)
		{	
			offset++;
			struct args * args_array;
			args_array = (struct args*)malloc(NUM_THREADS*sizeof(struct args));

			pthread_t threads[NUM_THREADS];

			int t,rc;
			for(t = 0; t < NUM_THREADS; t++){

				args_array[t].array1 = a1;
			 	args_array[t].array2 = a2;
			 	args_array[t].i = i;
			 	args_array[t].j = j;
			 	args_array[t].l = l;
			 	args_array[t].count = 0;
			 	args_array[t].tid = t;
			 	args_array[t].small_index = small_index;

				rc = pthread_create(&threads[t],NULL,parallel_compare_posix_a,&args_array[t]);
				if(rc){
					printf("ERROR; return code from pthread_create() is %d\n", rc);
					exit(-1);
				}
			}

			int * retval;
			for(t = 0; t < NUM_THREADS; t++){
				pthread_join(threads[t],(void**)&retval);
				distance[offset] += *retval;
				free(retval);
			}
			free(args_array);
		}
	}
	return distance;
}

void* parallel_compare_posix_a(void *thread_args)
{
	struct args * data;
	data = (struct args*)malloc(sizeof(struct args));


	data = (struct args*)thread_args;
	char ** a1 = data->array1;
	char ** a2 = data->array2;
	int i = data->i;
	int j = data->j;
	int l = data->l;
	int count = data->count;
	int tid = data->tid;
	int small_index=data->small_index;


	int distance = 0;
	int k;
	int start,stop;
	int chunk = l/NUM_THREADS;
	start = tid*chunk;
	
	if(tid != (NUM_THREADS-1))
		stop =(tid+1)*chunk;
	else
		stop = l;


	for(k = start; k < stop; k++)
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


	pthread_mutex_lock(&reduction_mx);
	distance += count;
	pthread_mutex_unlock(&reduction_mx);

	int *answer = (int*)malloc(sizeof(*answer));
	*answer = distance;
	pthread_exit(answer);
	return 0;
}
/* HAMMING DISTANCE using pthreads (medium grained)*/
int* hamming_distance_posix_b(char ** a1,char ** a2, int m, int n, int l)
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
			args_array[t].max = max;
			args_array[t].tid = t;
			args_array[t].small_index = small_index;
			

			rc = pthread_create(&threads[t],NULL,parallel_compare_posix_b,&args_array[t]);
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
			
			for(int w = 0; w < max; w++){
				//printf("__________i*max+w = %d, tid:%d\n",i*max+w,t);
				distance[i*max + w] += retval[w];
			}
			free(retval);
		}

		//for(int g = 0; g < max; g++)printf("retval[%d]: %d\n",g,retval[g]);

		free(args_array);	
	}
	return distance;
}

void* parallel_compare_posix_b(void *thread_args)
{
	struct args * data;
	data = (struct args*)malloc(sizeof(struct args));


	data = (struct args*)thread_args;
	char ** a1 = data->array1;
	char ** a2 = data->array2;
	int i = data->i;
	int l = data->l;
	int max = data->max;
	int tid = data->tid;
	int small_index=data->small_index;

	int * distance = (int*)malloc(max*sizeof(int));

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
		distance[j] = count;
		pthread_mutex_unlock(&reduction_mx);
	}

	
	//printf("Thread %d returns ",tid);
	
	// for(j=0;j<max;j++)
	// 	printf("%d__",distance[j]);
	// printf("\n");


	int *answer = (int*)malloc(max*sizeof(int));
	answer = distance;
	pthread_exit(answer);
	return 0;
}
/* HAMMING DISTANCE using pthreads (coarse grained)*/
int* hamming_distance(char ** a1,char ** a2, int m, int n, int l)
{
	int offset;
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


 		offset = -1;
		struct args * args_array;
		args_array = (struct args*)malloc(NUM_THREADS*sizeof(struct args));
		pthread_t threads[NUM_THREADS];

		int t,rc;
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

		for(t = 0; t < NUM_THREADS; t++)
		{
			rc = pthread_join(threads[t],(void**)&retval);
			//printf("waiting here.__________________________\n");
			if(rc){
				printf("ERROR; return code from pthread_join() is %d\n", rc);
				exit(-1);
			}
			
			for(int w = 0; w < m*n; w++)
			{
				//printf("retval[%d]:%d\n",w,retval[w]);
				distance[w] += retval[w];
			}

			free(retval);
		}

		//for(int g = 0; g < max; g++)printf("retval[%d]: %d\n",g,retval[g]);

		free(args_array);
	return distance;
}

void* parallel_compare(void *thread_args)
{
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

	int * distance = (int*)malloc(max*sizeof(int));

	//int offset;
	int count;
	int i,j,k;
	int start,stop;
	int chunk = min/NUM_THREADS;
	//printf("chunk:%d",chunk);
	start = tid*chunk;
	if(tid != (NUM_THREADS-1))
		stop =(tid+1)*chunk;
	else
		stop =  min;

	for(i = start; i < stop; i++)
	{
		for(j = 0; j < max; j++)
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
			//printf("cunt: %d\n",count);
			pthread_mutex_lock(&reduction_mx);
			OFFSET++;
			distance[OFFSET] = count;
			pthread_mutex_unlock(&reduction_mx);
		}
	}

	
	//printf("Thread %d returns ",tid);
	
	// for(j=0;j<M*N;j++)
	// 	printf("%d_",distance[j]);
	// printf("\n");


	int *answer = (int*)malloc(M*N*sizeof(int));
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
