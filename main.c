#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Gorkem Sahin
No: 15011087
01.01.2018
IDE: Dev-C++
TDM-GCC 4.9.2
Windows 10 64-bit
*/

#define SHINGLE_LIMIT 1000
#define DOC_LIMIT 30
#define SIG_LIMIT 100

void initArrays();
void getFiles ();
void getText (FILE*, int);
void getShingles (int, char*, int);
int placeShingle (int, char*);
double calculateJaccardSimilarity (int, int);
void printJaccardSimilarity ();
void initHashCoefficients (int*);
int findPrimeNumber ();
void initSigMatrix (int*, int);
double calculateSigSimilarity (int, int);
void printSigSimilarity();
void printSimilarDocs(double);

/*
Unused functions
void printShingles ();
void printDocs ();
*/

char* shingles[SHINGLE_LIMIT];
int docs[SHINGLE_LIMIT][DOC_LIMIT]; 
int sigMatrix[SIG_LIMIT][DOC_LIMIT];
int k;
int noOfShingles = 0;
int noOfFiles = 0;

int main(int argc, char *argv[]) {
	int hashCoefficients[SIG_LIMIT];
	int primeNo;
	double threshold;
	printf("Please enter K.\n");
	scanf("%d", &k);
	initArrays();
	getFiles();
	printJaccardSimilarity();
	initHashCoefficients(hashCoefficients);
	primeNo = findPrimeNo();
	initSigMatrix(hashCoefficients, primeNo);
	printSigSimilarity();
	printf("\nPlease enter the desired threshold for similarity detection.\n");
	scanf("%lf", &threshold);
	printSimilarDocs(threshold);
	return 0;
}

void initArrays (){
	int i, j;
	for (i=0;i<SHINGLE_LIMIT;i++){
		shingles[i] = (char*) malloc (k+1 * sizeof(char));
		strcpy(shingles[i], "");
	}
	for (i=0;i<SHINGLE_LIMIT;i++){
		for (j=0;j<DOC_LIMIT;j++){
			docs[i][j] = 0;
		}
	}
}

void getFiles (){
	FILE* fp;
	char fileName[10];
	int i=0;
	do {
		i++;
		sprintf(fileName, "%d.txt", i);
		fp = fopen(fileName, "r");
		if (fp != NULL){
			printf("Accessing %s...\n", fileName);
			getText(fp, i);
		}
	} while (fp != NULL);
	noOfFiles = i-1;
	printf("\n%d documents with %d %dkey-shingles were found and processed.\n\n", i-1, noOfShingles, k);
}

void getText (FILE* fp, int noOfFile){
	char text[2048];
	char c;
	int i = 0;
	int j;
	while (!feof(fp)){
		c = fgetc(fp);
		if (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))){
			if (c < 'a'){
				c += 32;
			}
			text[i] = c;
			i++;
		} else if ((c == ' ') && (text[i-1] != ' ')){
			text[i] = c;
			i++;
		}
	}
	text[i] = '\0';
	getShingles(noOfFile, text, i);
}

void getShingles (int noOfFile, char* text, int noOfLetters){
	char shingle[k+1];
	int i = 0;
	int j = 0;
	int shinglesPerDoc = 0;
	for (i=0;i<=noOfLetters-k;i++){
		for (j=0;j<k;j++){
			shingle[j] = text[i+j];
		}
		shingle[j] = '\0';
		if (placeShingle(noOfFile, shingle) != -1){
			shinglesPerDoc++;
		}
	}
	printf("%d unique %dk-shingles were found in %d.txt.\n", shinglesPerDoc, k, noOfFile);
}

int placeShingle (int noOfFile, char* shingle){
	int i = 0;
	while (i < noOfShingles && (strcmp(shingle, shingles[i]) != 0)){
		i++;
	}
	if (i >= noOfShingles){
		strcpy(shingles[i], shingle);
		docs[i][noOfFile] = 1;
		noOfShingles++;
		return 1;
	} else {
		if (docs[i][noOfFile] == 0){
			docs[i][noOfFile] = 1;
			return 1;
		}
		return -1;
	}
}

double calculateJaccardSimilarity (int doc1, int doc2){
	int i;
	double total = 0;
	double common = 0;
	for (i=0;i<noOfShingles;i++){
		if ((docs[i][doc1] == 1) || (docs[i][doc2] == 1)){
			total++;
			if ((docs[i][doc1] == 1) && (docs[i][doc2] == 1)){
				common++;
			}
		}
	}
	return common / total;
}

void printJaccardSimilarity (){
	int i, j;
	int matrix[noOfFiles][noOfFiles];
	printf("Jaccard Similarity Rates Between Files\n");
	for (i=1;i<=noOfFiles;i++){
		printf(" %4d", i);
	}
	printf("\n");
	for (i=1;i<=noOfFiles;i++){
		printf("%3d|", i);
		for (j=1;j<=noOfFiles;j++){
			printf("%.2lf|", calculateJaccardSimilarity(i, j));
		}
		printf("\n");
	}
}

void initHashCoefficients (int* hashCoefficients){
	int i;
	srand(time(0));
	for (i=0;i<SIG_LIMIT;i++){
		hashCoefficients[i] = rand() % noOfShingles;
	}
}

int findPrimeNo (){
	int i;
	int ok = 0;
	int primeNo = noOfShingles;
	while (ok == 0){
		i = 2;
		while ((i < primeNo) && (primeNo % i != 0)){
			i++;
		}
		if (i == primeNo){
			ok = 1;
		} else {
			primeNo++;
		}
	}
	return primeNo;
}

void initSigMatrix (int* hashCoefficients, int primeNumber){
	int i, j, k, hash;
	for (i=0;i<SIG_LIMIT;i++){
		for (j=0;j<DOC_LIMIT;j++){
			sigMatrix[i][j] = INT_MAX;
		}
	}
	for (i=0;i<noOfShingles;i++){
		for (j=0;j<=SIG_LIMIT;j++){
			hash = (hashCoefficients[j] * i + 1) % primeNumber;
			for (k=1;k<=DOC_LIMIT;k++){
				if (docs[i][k] == 1){
					if (hash < sigMatrix[j][k]){
						sigMatrix[j][k] = hash;
					}
				}
			}
		}
	}
}

double calculateSigSimilarity (int doc1, int doc2){
	int i;
	double total = 0;
	double common = 0;
	for (i=0;i<SIG_LIMIT;i++){
		if ((sigMatrix[i][doc1] != INT_MAX) || (sigMatrix[i][doc2] != INT_MAX)){
			total++;
			if (sigMatrix[i][doc1] == sigMatrix[i][doc2]){
				common++;
			}
		}
	}
	return common / total;
}

void printSigSimilarity (){
	int i, j;
	printf("\nSignature Similarity Rates Between Documents\n");
	for (i=1;i<=noOfFiles;i++){
		printf(" %4d", i);
	}
	printf("\n");
	for (i=1;i<=noOfFiles;i++){
		printf("%3d|", i);
		for (j=1;j<=noOfFiles;j++){
			printf("%.2lf|", calculateSigSimilarity(i, j));
		}
		printf("\n");
	}
}

void printSimilarDocs (double threshold){
	int i, j;
	int count = 0;
	double similarity;
	printf("\nDocuments With A Similarity Rate Greater Than %.2lf.\n", threshold);
	for (i=1;i<noOfFiles;i++){
		for (j=i+1;j<=noOfFiles;j++){
			similarity = calculateSigSimilarity(i, j);
			if (similarity >= threshold){
				printf(" %d.txt - %d.txt : %.2lf\n", i, j, similarity);
				count++;
			}
		}
	}
	printf("\n%d document pairs were too similar considering the given threshold.\n", count);
}

/*	UNUSED FUNCTIONS	

Function to create shingles based on words instead of letters.

void getShingles (int noOfFile, char* text, int noOfWords, int k){
	char words[noOfWords][20];
	char shingle[20 * k];
	int i = 0;
	int j = 0;
	int l = 0;
	int head = 0;
	while (text[l] != '\0'){
		if (text[l] != ' '){
			words[i][j] = text[l];
			j++;
			l++;
		} else {
			words[i][j] = '\0';
			i++;
			l++;
			j = 0;
		}
	}
	for (i=0;i<=noOfWords-k;i++){
		strcpy(shingle, "");
		for (j=i;j<i+k;j++){
			strcat(shingle, words[j]);
			strcat(shingle, " ");
		}
		printf("\n %s \n", shingle);
		//saveShingles(noOfFile, k, shingle);
	}
}


FOR DEBUG PURPOSES 

void printShingles (){
	int i;
	for (i=0;i<noOfShingles;i++){
		printf("%s\n", shingles[i]);
	}
}

void printDocs (){
	int i, j;
	for (i=0;i<noOfShingles;i++){
		for (j=0;j<noOfFiles;j++){
			printf("%d ", docs[i][j]);
		}
		printf("\n");
	}
}

*/
