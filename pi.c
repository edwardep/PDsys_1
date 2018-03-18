#include <stdio.h>
#include <sys/time.h>
#include <omp.h>

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

	int i; double x,pi,sum = 0.0;

	step = 1.0/(double)num_steps;
	

	double t1 = gettime();	
	
		for (i = 0; i<num_steps; i++)
		{

			#pragma omp parallel
			{
				x = (i+0.5)*step;
				sum = sum + 4.0/(1.0+x*x);
			}
		pi = step*sum;
		}
	double t2 = gettime();

	printf("%f\n",(t2-t1));

	return 0;
}