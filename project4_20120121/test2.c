/*****************************************
 * Made by 20120121 Lee HeeRock
 * Test C- program to check
 * (1) Accessing global Array
 * (2) Accessing local Array which is declared in caller proceduere and passed as argument
 * (3) Check while statement and if statment
 * (4) Evaluate the Relative Operation
 
	get Length of the global array
->  fill the array with user input
->  get the start index and end index of the subArray of the globalArray
->  copy 
->  get largest # of the subArray

******************************************/
int gloArr[100];
int gloLen;

int getBiggest(int src[], int indOne, int indTwo){

	int bigOne;
	bigOne = src[indOne];
	indOne = indOne + 1;
	while(indOne <= indTwo){
	
		if(bigOne < src[indOne])
				bigOne = src[indOne];
		indOne = indOne + 1;
	
	}

	return bigOne;
}

void copy(int dest[], int start, int end){


	while(start <= end){
	
		dest[start] = gloArr[start];
		start = start + 1;
	}


}

void main(void){


	int subArrOne[100];
	 	
	int ind;
	int start;
	int end;
	int Large;
	ind = 0;
	/*Determine the Array size to used*/
	gloLen = input();
	/*fill the array  with user inputs*/
	
	while(ind < gloLen){
		gloArr[ind] = input();
		ind = ind + 1;
	}	
	ind = 0;
	/*print the all the elements to the user*/
	while(ind < gloLen){
		output(gloArr[ind]);
		ind = ind +1;
	}
		
	/*get the start index and end index of the subArray of gloArr*/
	start = input();
	end	= input();
	
	if(end < gloLen){
	
		if(start < end){
			
			copy(subArrOne, start, end);
			Large = getBiggest(subArrOne, start, end);
			output(Large);
		}
	}

}
