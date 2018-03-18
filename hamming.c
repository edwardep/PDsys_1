#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

void hamming_distance(int * distance, char ** a1,char ** a2, int m, int n, int l);

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



	int distance[m*n];
	hamming_distance(distance,arr1,arr2,m,n,l);


	for(int h = 0; h < m; h++)
		printf("%s\n",*(arr1+h));
	for(int h = 0; h < m; h++)		
		printf("%s\n",*(arr2+h));

	for(int h = 0; h<m*n;h++)
	 	printf("%d_", distance[h]);



	return 0;
}
/**	
args: 2 arrays of char pointers, sizes of arrays
ret.val : calculated hamming distance

**/	

void hamming_distance(int * distance,char ** a1,char ** a2, int m, int n, int l)
{
	int i,j,k = 0;
	int count = 0;
	int offset = -1;
 	int min,max = 0;


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
			count = 0;
			for(k = 0; k < l; k++)
			{
				if(a1[i][k] != a2[j][k])
					count++;
			}
			distance[offset] = count;
		}
	}

}