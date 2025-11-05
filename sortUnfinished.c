/*AUTHORS: Destry Mills
  VERSION: 1.0 29 October 2025
				Basic implementation of hash map for looking up characters 
  RESOURCES:
*/

#include <stdio.h>
#include <string.h>
#include "sort.h"

#define EXSTRING "abcdefghijklmnopqrstuvqxyz"

int main() {

//length of string [temp]
int length = strlen(EXSTRING);
//frequency of letters (by ASCII value)
int freq[256] = {0};
//prints number of character
printf("The text file is %d characters\n %d bits\n", length, length*8);

CharInfo head;

//increments frequency of ascii characters in freq array
for(int i = 0; i < length; i++){
	freq[(unsigned char)EXSTRING[i]]++;
}

//make and sort array of all letters with content int linked list CharInfo
int i = 0;
while(i < 256){
	if(freq[i] > 0){
		if(head.letter == NULL){
			head.letter = i;
			head.frequency = freq[i];
		} else {
			CharInfo currentChar = head;
			while(currentChar != NULL){
			    if(currentChar.next->frequency > freq[i]){
			        CharInfo temp;
			        temp.frequency = freq[i];
			        temp.letter = i;
			        temp.next = currentChar.next;
			        currentChar.next = *temp;
			    }
			}
		}
	}
}


/* SORT BY FREQUENCY -----------------------------------------------------------
*  ASSEMBLE HUFFMAN TREE
*  ENCODE
*/
}