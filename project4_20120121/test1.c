/***********************************************
 *made by : 20120121 LEE HEE ROCK
 *test case to check:
 * (1) Accessing Local array which is declared in other procedure and passed as argument
 * (2) User input and Output 
 *
 * Flow
 * (1) main() : get Array length from user(Cannot exceed 1000)  [ and call giveNUM() with passing array and length as argument ]
 * (2) giveNum(int arr[], int len) : get User inputs by "length" times, and each value is stored in the array. [and call print]
 * (3) print(int arr[], len) : print the array element
 * (4) BubbleSort(int arr[], len) : Sort the array by descending order 
 * (5) sumOfArray(int arr[], len) : get total sum of the array element
 * (6) each step is processed different procedures.
 * ********************************************/
void swap(int arr[], int indOne, int indTwo){


	int tmp;
	tmp = arr[indOne];
	arr[indOne] = arr[indTwo];
	arr[indTwo] = tmp;

}

int BubbleSort(int arr[], int len){

	int i;
	int j;
	int temp;

	i = 0;

	while(i < len){
		j = i + 1;
		while(j < len){
			if(arr[i] < arr[j]){
					swap(arr, i, j);
			}
			j = j + 1;
		
		}
		i = i + 1;
	}
}


int sumOfArray(int b[], int len){
	
	int sum;
	int i;
	i=0;
	sum = 0;
	while(i < len){
	
		sum = sum + b[i];
		
		i = i + 1;    	
	
	}
	
	return sum;
}


void print(int k[], int a){
	
	int ret;
	int ind;
	ind = 0;
	
		while(ind < a){
		
			output(k[ind]);
			ind = ind + 1;
		
		}
		
}

void giveNum(int arr[], int len){

	int ind;
	ind = 0;
		
		while(ind < len){
			arr[ind] = input();
			ind = ind + 1;
		}
	/*print the array elements in which user fills*/
	print(arr, len);
	/*sort the array by descending order*/
	BubbleSort(arr, len);
	/*print the sorted result*/
	print(arr, len);	
}


void main(void){

	int arr[1000];
	int len;
	/*get size of the array*/
	len = input();
	giveNum(arr, len);
	output(sumOfArray(arr, len));
}
