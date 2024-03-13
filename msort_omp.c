#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include <string.h>

void merge(int *X, int n, int *tmp){
	int i = 0;
	int j = n/2;
	int ti = 0;
	
	//i will iterate till first  half anf J will iterate for 2nd half of array
	while (i<n/2 && j<n) {
		if (X[i] < X[j]) {
			tmp[ti] = X[i];
			ti++; i++;
		}
		else {
			tmp[ti] = X[j];
			ti++; 
			j++;
		}
	}
	
	while (i<n/2) { /* finish up lower half */
		tmp[ti] = X[i];
		ti++;
		i++;
	}
	
	while (j<n) { /* finish up upper half */
		tmp[ti] = X[j];
		ti++; 
		j++;
	}
	//Copy sorted array tmp back to  X (Original array)
	memcpy(X, tmp, n*sizeof(int));
}

void mergesort(int *X, int n, int *tmp){
	if (n < 2) return;

	#pragma omp task firstprivate (X, n, tmp)
	mergesort(X, n/2, tmp);

	#pragma omp task firstprivate (X, n, tmp)
	mergesort(X+(n/2), n-(n/2), tmp);

	#pragma omp taskwait
	merge(X, n, tmp);
}

void printArray(int *arr, int size){
	int i;
	for (i = 0; i < size; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}

int main(){
	int threads;
	int n = 10000000;//10M
	
	int *arr, *tmp;
	arr = (int*)calloc(n,sizeof(int));
	tmp = (int*)calloc(n,sizeof(int));
	int i = 0, j = 0;
	
	printf("Array size: %d", n);
	printf("\nEnter number of threads: ");
	scanf("%d", &threads);
	omp_set_num_threads(threads);
	
	//printf("Enter size of array: ");
	//scanf("%d", &n);
	
	for(i = 0; i < n; i++){
		arr[i] = rand() % n;
		//printf("%d ", arr[i]);
	}
	
	double start; 
	double end; 
	start = omp_get_wtime();
	int k;
	
	#pragma omp parallel
	{
		#pragma omp single
		mergesort(arr, n, tmp);
	}
	
	end = omp_get_wtime();
	printf("Execution Time = %f seconds\n",(end-start));
	
	return 0;
}
