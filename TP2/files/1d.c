#include <stdio.h>
#include <stdlib.h>
#include <omp.h> 

#define N_VISITANTES 100000
#define N_PROC 2

int ticket[N_PROC] = {0};		
int elegido[N_PROC] = {0};		
int visitantes = 0;

int sacar_ticket (void)
{
	int i, max = ticket[0];
	for(i = 1; i < N_PROC; i++)
		if (max < ticket[i]) max = ticket[i];
	return max + 1;
}

int favorecido (int i, int j)
{
	if ((ticket[i] == 0) || (ticket[j] < ticket[i]))
		return j;
	if (ticket[i] < ticket[j])
		return i;
	return i <= j;
}

void molinete (int cant_molinete)
{
	int i, j;
	for (i = 0; i < N_VISITANTES;i++) {
		elegido[cant_molinete] = 1;
		ticket[cant_molinete] = sacar_ticket();
		elegido[cant_molinete] = 0;
		for(j = 0; j < N_PROC; j++) {
			__asm__("mfence");
			if (j != cant_molinete) {
				while (elegido[j]);
				while (favorecido(j, cant_molinete) == j);
			}
		}
		visitantes++;
		ticket[cant_molinete] = 0;
	}
}

int main ()
{
	int i;
	omp_set_num_threads(N_PROC);	
	#pragma omp parallel for
	for(i = 0; i < N_PROC; i++)
		molinete(i);
	printf("Hoy hubo %d visitantes!\n", visitantes);
	return 0;
}
