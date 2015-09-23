#include <stdio.h>
#include <omp.h>

#define N_VISITANTES 100000

int visitantes = 0;

void molinete()
{
	int i;
	for (i=0;i<N_VISITANTES;i++)
	visitantes++;
}

int main ()
{
	omp_set_num_threads(2);
	#pragma omp parallel
  	molinete();  
  	printf("Hoy hubo %d visitantes!\n", visitantes);
	return 0;
}
