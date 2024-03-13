#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// merge two sorted arrays v1, v2 of lengths n1, n2, respectively
int *merge(int * v1, int n1, int * v2, int n2){
	int * result = (int *)malloc((n1 + n2) * sizeof(int));
	int i = 0;
	int j = 0;
	int k;
	for (k = 0; k < n1 + n2; k++) {
		if (i >= n1) {
			result[k] = v2[j];
			j++;
		}
		else if (j >= n2) {
			result[k] = v1[i];
			i++;
		}
		else if (v1[i] < v2[j]) {
			result[k] = v1[i];
			i++;
		}
		else {
			result[k] = v2[j];
			j++;
		}
	}
	return result;
}


void swap(int *v, int i, int j){
	int t;
	t = v[i];
	v[i] = v[j];
	v[j] = t;
}
// bubblesort array v of length n
void bubblesort(int *v, int n){
	int i,j;
	for(i=n-2;i>=0;i--){
		for(j=0;j<=i;j++){
			if(v[j]>v[j+1]){
				swap(v,j,j+1);
		        }
		}      
	}
}

int main(int argc, char **argv){
	int * data;
	int * chunk;
	int * temp;
	int m,n;
	int rank,size;
	int s;
	int i;
	int step;
	double elapsed_time;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	//start timer
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = - MPI_Wtime();
	if(rank==0){
		//define array
		int r;
		n = 250000; //250k
		//calculate chunk size to account for left out elements at the end of array
		s = n/size;
		r = n%size;
		data = (int *)malloc((n+size-r)*sizeof(int));
		for(i=0;i<n;i++)
			data[i] = rand() % n;
		if(r!=0){
			for(i=n;i<n+size-r;i++)
  			   data[i]=0;
			s=s+1;
		}
	}
	// all processes create chunks bcasting chunk size to have uniform chunks
	MPI_Bcast(&s,1,MPI_INT,0,MPI_COMM_WORLD);
	chunk = (int *)malloc(s*sizeof(int));
	MPI_Scatter(data,s,MPI_INT,chunk,s,MPI_INT,0,MPI_COMM_WORLD);
	bubblesort(chunk,s);
	
	step = 1;
	while(step<size){
		//even odd sorting (every even process from 2 to n receives chunk from next even process and mergesorts it to create 1 chunk, odd processes to same from 1 to n processes)
		if(rank%(2*step)==0){
			if(rank+step<size){
				MPI_Recv(&m,1,MPI_INT,rank+step,0,MPI_COMM_WORLD,&status);
				temp = (int *)malloc(m*sizeof(int));
				MPI_Recv(temp,m,MPI_INT,rank+step,0,MPI_COMM_WORLD,&status);
				chunk = merge(chunk,s,temp,m);
				s = s+m;
			}
		}
		else{
			int a = rank-step;
			MPI_Send(&s,1,MPI_INT,a,0,MPI_COMM_WORLD);
			MPI_Send(chunk,s,MPI_INT,a,0,MPI_COMM_WORLD);
			break;
		}
		step = step*2;
	}
	
	//stop timer
	elapsed_time += MPI_Wtime();
	
	if(rank==0){
		FILE *fptr;
		char s[100];
		
		//writing log file
		fptr = fopen("log.txt", "a+");
		sprintf(s, "%s%d%s%d%s%f%s","Bubblesort using MPI with ", n, " data size on ", size, " processes: ", elapsed_time, " seconds\n");
		fprintf(fptr, "%s", s);
		printf("%s", s);

		//writing output file
		fptr = fopen("output.txt", "w");
		for(int i =0; i < n; i++){
			fprintf(fptr, "%d ", chunk[i]);
		}
		
		free(chunk);
		fclose(fptr);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
