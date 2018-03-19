#include <stdio.h>
#include <sys/time.h>
#include <omp.h>



#define THR_NUM 2
static long num_steps = 100000000;
double step;



double gettime(void)
{
	struct timeval ttime;
	gettimeofday(&ttime , NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}


int main()
{
	double pi;
	step = 1.0/(double)num_steps;
	omp_set_num_threads(THR_NUM);

	double t1 = gettime();

	#pragma omp parallel
	{
	int i; double x,sum;


	int nthrds = omp_get_num_threads();
	int id = omp_get_thread_num();
	sum = 0.0;
		for (i = id; i<num_steps; i= i+nthrds)
		{
			x = (i+0.5)*step;
			sum += 4.0/(1.0+x*x);
		}
		#pragma omp critical
		pi += step*sum;
	}
	double t2 = gettime();

	printf("%f\n",(t2-t1));

	return 0;
}