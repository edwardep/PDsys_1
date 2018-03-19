#include <stdio.h>
#include <sys/time.h>
#include <omp.h>



#define THR_NUM 4
static long num_steps = 100000;
double step;



double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}


int main()
{
	int nthreads;
	int i;
	
	double sum[THR_NUM];
	double pi;
	step = 1.0/(double)num_steps;
	omp_set_num_threads(THR_NUM);

	double t1 = gettime();

	#pragma omp parallel
	{
	int i; double x;


	int nthrds = omp_get_num_threads();
	int id = omp_get_thread_num();
	if(id == 0) nthreads = nthrds;
	
	sum[id] = 0.0;
		for (i = id; i<num_steps; i= i+nthrds)
		{
			x = (i+0.5)*step;
			sum[id] += 4.0/(1.0+x*x);
		}
	
	}
	
	for(i = 0;i<nthreads;i++)
		pi += step*sum[i];
	
	double t2 = gettime();

	printf("%f\n",(t2-t1));

	return 0;
}