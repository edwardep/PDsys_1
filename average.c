#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX 5
#define NUM_THREADS 2


void* hello_world(void* args_data);
struct args
{
	double * A;
	double ave;
	int tid;
};


pthread_barrier_t for_loop;
pthread_mutex_t reduction_mx;

struct args args_arr[NUM_THREADS];
int main()
{
	pthread_barrier_init(&for_loop,NULL,NUM_THREADS+1);
	pthread_mutex_init(&reduction_mx,NULL);
	double ave=0.0, A[MAX] = {2,2,3,4,4};

	//void*status;

	//#pragma omp parallel// for reduction (+:ave)

	pthread_t threads[NUM_THREADS];
	int t;
	for(t=0;t<NUM_THREADS;t++)
	{
		args_arr[t].A = A;
		args_arr[t].ave = ave;
		args_arr[t].tid = t;
		pthread_create(&threads[t],NULL,hello_world,&args_arr[t]);
	}
	for(t=0;t<NUM_THREADS;t++)
		pthread_join(threads[t],NULL);
	

	
	printf("average: %f\n",args_arr[1].ave);
	printf("average: %f\n",args_arr[0].ave);
	return 0;

}

void* hello_world(void* args_data)
{


	struct args *data;
	data = (struct args *)args_data;
	
	double * A = data->A;
	double ave = data->ave;



	double local_ave = 0.0;
	int i;
	int id = data->tid;
	printf("id:%d\n",id);

	for(i=id;i<MAX;i=i+NUM_THREADS)
	{
		local_ave += A[i];
	}
	//pthread_barrier_wait(&for_loop);
	pthread_mutex_lock(&reduction_mx);
	ave = (ave+local_ave)/MAX;
	//data->ave = ave;
	pthread_mutex_unlock(&reduction_mx);
	printf("_%f-\n",ave);
	data->ave=ave; 

	return 0;

	//return 0;
}