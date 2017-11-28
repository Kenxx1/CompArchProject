#include <iostream>
#include <cmath>
#include <omp.h>
#define NUM_THREADS 2;
using namespace std;

static long num_steps =100000;
double step;
  	
  int main()
  {
  	double omp_get_wtime();
  	int i;
  	double x,pi,sum=0.0;
  	step = 1.0/(double)num_steps;
  	int nthreads;
  	double sum[NUM_THREADS];
	#pragma omp parallel
	{
		int i, id, nthrds;
		double x;
		id = omp_get_thread_num();
		nthreads = omp_get_num_threads();

		if(id==0)
			nthreads=nthrds;

	  	for(i =id, sum[id]=0.0; i<num_steps;i=i+nthrds)
	  	{
	  		x=(i+0.5)*step;
	  		sum[id]+=4.0/(1.0+x*x);
	  	}
		for (i = 0,pi=0.0; i < nthreads; i++)
		{
			pi+=sum[i]*step;
		}
  }
  	cout<<pi<<endl;
	cout<<omp_get_wtime()<<endl;  
  }
