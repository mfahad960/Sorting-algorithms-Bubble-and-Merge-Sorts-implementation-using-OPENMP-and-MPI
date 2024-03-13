#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <unistd.h>
#include <string.h>

void merge(int *a, int *b, int l, int m, int r) {
	
	int h, i, j, k;
	h = l;
	i = l;
	j = m + 1;
	while((h <= m) && (j <= r)) {
		if(a[h] <= a[j]) {
			b[i] = a[h];
			h++;
		}
		else {
			b[i] = a[j];
			j++;
		}
		i++;
	}
		
	if(m < h) {
		for(k = j; k <= r; k++) {
			b[i] = a[k];
			i++;
		}
	}
	else {
		for(k = h; k <= m; k++) {
			b[i] = a[k];
			i++;
		}
	}
	for(k = l; k <= r; k++) {
		a[k] = b[k];
		}
}

void mergeSort(int *a, int *b, int l, int r) {
	
	int m;
	
	if(l < r) {
		
		m = (l + r)/2;
		
		mergeSort(a, b, l, m);
		mergeSort(a, b, (m + 1), r);
		merge(a, b, l, m, r);
		}
		
}

int main(int argc, char** argv) {
	
	//define the array
	int n = 10000000; //10M
	int *original_array = malloc(n * sizeof(int));
	double elapsed_time;
	
	int c;
	srand(time(NULL));
	for(c = 0; c < n; c++) {
		original_array[c] = rand() % n;
	}

	int world_rank;
	int world_size;
	
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	elapsed_time = - MPI_Wtime();
	
	//define chunks
	int size = n/world_size;
	
	//send each chunk to processes
	int *sub_array = malloc(size * sizeof(int));
	MPI_Scatter(original_array, size, MPI_INT, sub_array, size, MPI_INT, 0, MPI_COMM_WORLD);
	
	//perform the mergesort on each chunk
	int *tmp_array = malloc(size * sizeof(int));
	mergeSort(sub_array, tmp_array, 0, (size - 1));
	
	//gather the sorted subarrays
	int *sorted = NULL;
	if(world_rank == 0) {
		sorted = malloc(n * sizeof(int));
	}
	
	MPI_Gather(sub_array, size, MPI_INT, sorted, size, MPI_INT, 0, MPI_COMM_WORLD);
	
	//final mergeSort call
	if(world_rank == 0) {
		int *other_array = malloc(n * sizeof(int));
		mergeSort(sorted, other_array, 0, (n - 1));
		free(other_array);
	}
	
	free(original_array);
	free(sub_array);
	free(tmp_array);
	
	elapsed_time += MPI_Wtime();
	if (world_rank == 0) {
		FILE *fptr;
		char s[100];
		//writing log file
		fptr = fopen("log.txt", "a+");
		sprintf(s, "%s%d%s%d%s%f%s","Mergesort using MPI with ", n, " data size on ", world_size, " processes: ", elapsed_time, " seconds\n");
		fprintf(fptr, "%s", s);
		printf("%s", s);
		//writing output file
		fptr = fopen("output.txt", "w");
		for(int i =0; i < n; i++){
			fprintf(fptr, "%d ", sorted[i]);
		}
		free(sorted);
  	}	
	
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	
}
