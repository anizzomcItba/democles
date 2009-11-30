/* shellsort.c */

/* Algoritmo de ordenamiento muy groso: http://en.wikipedia.org/wiki/Shell_sort
 * 	input: an array a of length n with array elements numbered 0 to n − 1

inc ← round(n/2)
while inc > 0 do:
    for i = inc .. n − 1 do:
        temp ← a[i]
        j ← i
        while j ≥ inc and a[j − inc] > temp do:
            a[j] ← a[j − inc]
            j ← j − inc
        a[j] ← temp
    inc ← round(inc / 2.2)
 *
 *
 * n/2 es un caso recomendado como paso inicial.
 * 2.2 es un incremento recomendado para la mayor candidad de
 * casos, el mejor caso es la sucesión de fibonacci con la relación
 * "dorada"
 *
 *
 * Ordena muy rápidamente, en O(n) si el arreglo está casi ordenado, es nuestro
 * caso
 *
 * Básicamente, es similar al insert sort, pero en lugar de querer ordenar
 * un array de 100 elementos por ejemplo de una, lo que hace, partir este
 * array en partes, y luego ordena a N pasos, de tal manera que se genera una
 * "clase" de elementos ordenados... así sucesivamente hasta que N = 1, por
 * lo tanto el array está ordenado.
 */

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <limits.h>


#define SIZE 100




void showArray(int *v, int size);
void shellSort(int *v, int size);
int ordenado(int *v, int size);

int
main(void){
	int i, j, aux;
	int array[SIZE];


	srand(time(NULL));

	/* Creamos el array */
	for (i = 0 ; i < SIZE ; i++)
		array[i] = rand();

	/* Desordenamos el array */

	for (i = 0 ; i < SIZE ; i++){
		j = rand() % SIZE;
		//SWAP
		aux = array[j];
		array[j] = array[i];
		array[i] = aux;
	}

	/* Array desordenado */
	if(!ordenado(array, SIZE))
		printf("Array desordenado\n");
	showArray(array, SIZE);

	shellSort(array, SIZE);
	if(ordenado(array, SIZE))
		printf("Array ordenado\n");
	showArray(array, SIZE);

}



void showArray(int *v, int size){
	int i;

	for (i = 0 ; i < size ; i++)
		printf("%d;", v[i]);
	putchar('\n');
	return;
}



void shellSort(int *v, int size){
	int inc =  size/2;
	int i, j, tmp;

	while(inc > 0){
		for(i = inc ; i < size ; i++){
			tmp = v[i];
			j = i;
			while(j >= inc && v[j - inc] > tmp){
				v[j] = v[j - inc];
				j = j - inc;
			}
			v[j] = tmp;
		}
		inc = inc/2;
	}
}

int ordenado(int *v, int size){
	int i;

	for(i = 1 ; i < size ; i++)
	{
		if(v[i-1] > v[i])
			return 0;
	}
	return 1;
}
