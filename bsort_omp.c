#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<omp.h>

void swap(int* xp, int* yp){
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

void printArray(int arr[], int size){
	int i;
	for (i = 0; i < size; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}

int main(){
	int threads;
	int n = 250000;
	
	int arr[n];
	int i = 0, j = 0;
	
	printf("Enter number of threads: ");
	scanf("%d", &threads);
	omp_set_num_threads(threads);
	
	//printf("Enter size of array: ");
	//scanf("%d", &n);
	
	for(i = 0; i < n; i++){
		arr[i] = rand() % n;
	}
	
	//printf("Unsorted array: \n");
	//printArray(arr, n);
	
	double start;
	double end;
	int k, f;
	
	start = omp_get_wtime();
	
	for (i = 0; i < n - 1; i++){
		f = i % 2;
		#pragma omp parallel for default(none),shared(arr,f,n,i)
		for (j = f; j < n - 1; j++){
				if (arr[j] > arr[j + 1]){
					swap(&arr[j], &arr[j + 1]);
				}
				
		}
	}
	
	end = omp_get_wtime();
	printf("Execution Time = %f seconds\n",(end-start));
	
	//printf("\nSorted array: \n");
	//printArray(arr, n);
	return 0;
}
