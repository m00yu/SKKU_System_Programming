// Bigram Analyzer
// Insertion sort to quick sort

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 100 // 한 단어당 max 글자수
#define HASH_TABLE_SIZE 1000 // hash bucket 개수
#define BUFFER_SIZE 1024*1024*6 //6MB

typedef struct _BigramNode {
    char bigram[2 * MAX_WORD_LENGTH]; // 2개 단어이기에 *2
    int frequency; // bigram 빈도수
    struct _BigramNode *next;
} BigramNode;

BigramNode *hashTable[HASH_TABLE_SIZE]; 

void printNodeToFile(BigramNode *node);

void InitHashTable(){
    for (int i=0; i<HASH_TABLE_SIZE; i++){
        hashTable[i] = NULL;
    }
}

int hash(char *str) {
    /*
    두 단어의 ASCII 합 % bucket 개수
    */
    int sum = 0;
    for (int i = 0; i<strlen(str); i++) { 
        sum += str[i]; 
    }
    int bucket_idx = sum % HASH_TABLE_SIZE;
    return bucket_idx;
}

void lower1 (char* str){
    /*
    lowercase로 변환 (SP05-1 코드와 동일)
    */
	for (int i=0; i<strlen(str); i++)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] -= ('A' - 'a');
}

void insertNewFront(char *bigram) {
    /*
    Linked List scan이후, 동일한 bigram node가 없는 경우,
    First insert 하기
    */
    int index = hash(bigram); // Get the bucket index

    BigramNode *newNode = (BigramNode *)malloc(sizeof(BigramNode));
    strcpy(newNode->bigram, bigram);
    newNode->frequency = 1;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;

}

BigramNode* rScanList(BigramNode *node, char *bigram) { 
    /*
    동일한 bigram이 있는지 list scanning
    Recursive way.
    */
   if (node == NULL){
        insertNewFront(bigram);
        node = hashTable[hash(bigram)];
        return node; 
    }
    
    if (strcmp(node->bigram, bigram) == 0) { // Bigram found and updated
        node->frequency++;
        return node; 
    }

    // Recursively check the next node
    return rScanList(node->next, bigram);
}

void Swap(BigramNode* a, BigramNode* b)
{
    char bigramTemp[2 * MAX_WORD_LENGTH];
    strcpy(bigramTemp, a->bigram); 
    int frequencyTemp = a->frequency;

    strcpy(a->bigram, b->bigram);
    a->frequency = b->frequency;

    strcpy(b->bigram, bigramTemp);
    b->frequency = frequencyTemp;
	// BigramNode temp = *a;
	// *a = *b;
	// *b = temp;
}

int Partition(BigramNode* data, int left, int right)
{   
	int pivot = data[left].frequency;
    int low = left+1, high=right;

    while(1){
        while (low<right && data[low].frequency >= pivot)
            low++;
        while (high>left && data[high].frequency < pivot)
            high--;
        
        if(low<high)
            Swap(&data[low], &data[high]);
        else break;
    }
    Swap(&data[left], &data[high]);
    return high;
}

void QuickSort(BigramNode* list, int left, int right){
    if (left<right){
        int mid = Partition(list, left, right);
        QuickSort(list, left, mid-1);
        QuickSort(list, mid+1, right);
    }
}

void QuickSortExe() {
    /*
    Bigram Analyzing
    모든 bigram을 frequency 순으로 정렬하기 (내림차순)
    */

	// 1. data가 들어있는 모든 bucket을 linked list로 연결하기
    int* validBucketIdx = (int*)malloc(sizeof(int) * HASH_TABLE_SIZE);
    int count = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            validBucketIdx[count++] = i;
        }
    }
    
    BigramNode* firstNode = hashTable[validBucketIdx[0]];
	BigramNode* temp = firstNode;

    for (int i = 0; i < count-1; i++) {
		while (temp->next != NULL){
			temp = temp->next;
		}
		temp->next = hashTable[validBucketIdx[i+1]];
	}
    free(validBucketIdx);

	// 2. 하나로 연결된 linked list를 Quick sort하기
    printf("quick sort를 위한 준비중\n");
    BigramNode* tempCount = firstNode;
    int numNode = 0;
    int num1 = 0;
    int num2 = 0;
    int num3 = 0;
    while (tempCount != NULL){
        if (tempCount->frequency == 1)
            num1++;
        else if (tempCount->frequency == 2)
            num2++;
        else if (tempCount->frequency == 3)
            num3++;
        else {
            numNode++;
        } 
        tempCount = tempCount->next;
    }
    printf("%d %d %d %d\n", num1, num2, num3, numNode);


    // Linked list를 array로
    BigramNode* curr = firstNode;
    BigramNode* nodeArray = (BigramNode*)malloc(sizeof(BigramNode)*numNode);
    BigramNode* nodeArray1 = (BigramNode*)malloc(sizeof(BigramNode)*num1);
    BigramNode* nodeArray2 = (BigramNode*)malloc(sizeof(BigramNode)*num2);
    BigramNode* nodeArray3 = (BigramNode*)malloc(sizeof(BigramNode)*num3);
    int idx=0, idx1=0, idx2=0, idx3=0;

    for (int i=0; i<(num1+num2+num3+numNode); i++){
        if (curr->frequency == 1){
            strcpy(nodeArray1[idx1].bigram, curr->bigram);
            nodeArray1[idx1].frequency = curr->frequency;
            idx1++;
        }
        else if (curr->frequency == 2){
            strcpy(nodeArray2[idx2].bigram, curr->bigram);
            nodeArray2[idx2].frequency = curr->frequency;
            idx2++;
        }
        else if (curr->frequency == 3){
            strcpy(nodeArray3[idx3].bigram, curr->bigram);
            nodeArray3[idx3].frequency = curr->frequency;
            idx3++;
        }
        else {
            strcpy(nodeArray[idx].bigram, curr->bigram);
            nodeArray[idx].frequency = curr->frequency;
            idx++;
        }
        curr = curr->next;
    }
    printf("복사완료 %d %d %d %d\n", idx1, idx2, idx3, idx);

    // quick sort
    QuickSort(nodeArray, 0, numNode);

    printf("최종 sorted array 합치는중..\n");
    BigramNode* finalArray = (BigramNode*)malloc(sizeof(BigramNode) * (numNode+num1+num2+num3));
    memcpy(finalArray, nodeArray, sizeof(BigramNode) * numNode);
    memcpy(finalArray + numNode, nodeArray1, sizeof(BigramNode) * num1);
    memcpy(finalArray + numNode + num1, nodeArray2, sizeof(BigramNode) * num2);
    memcpy(finalArray + numNode + num1 + num2, nodeArray3, sizeof(BigramNode) * num3);


    for (int i=0; i<30; i++){
        printf("%s: %d\n", finalArray[i].bigram, finalArray[i].frequency);
    }
    free(nodeArray);
    free(nodeArray1);
    free(nodeArray2);
    free(nodeArray3);
    free(finalArray);

	// hashTable[0] = head;
}

void printBigrams(BigramNode *node) {
    /*
    내림차순으로 정렬된 모든 node를 출력
    */
   for (int i=0; i<30; i++){
        printf("%s: %d\n", node->bigram, node->frequency);
        node = node->next;
        if (node == NULL)
            break;
    }
}

void printBigramsToFile(BigramNode *node) {
    /*
    내림차순으로 정렬된 모든 node를 txt로 저장
    */
    FILE *file = fopen("output.txt", "w"); 

    while (node != NULL) {
        fprintf(file, "%s: ", node->bigram); 
        fprintf(file, " (%d)\n", node->frequency);
        node = node->next;
    }
    fclose(file);
}

void printNodeToFile(BigramNode *node) {
    /*
    디버깅 용도
    */
    FILE *file = fopen("output.txt", "a"); 
    fprintf(file, "%s %d\n", node->bigram, node->frequency); 
    fclose(file);
}

void countUniqueBigram(BigramNode* firstNode){
    int cnt=0;
    while (firstNode != NULL){
        cnt += 1;
        firstNode = firstNode->next;
    }
    printf("Num unique bigram: %d\n", cnt);
}

int main() {
    time_t start = time(NULL);

    char input[BUFFER_SIZE];
    int fd;
    ssize_t rd_size;
    
    InitHashTable();
    
    if (0 < (fd = open("shakespeare_part_part.txt", O_RDONLY))){
        while( 0 < (rd_size = read(fd, input, BUFFER_SIZE-1))){
            input[rd_size] = '\0';
            int len = strlen(input);
            for(int i=0; i<len; i++){
                if(input[i] == '\n') input[i]=' ';
                else if(input[i]<32) input[i]=' ';
            }
            char* token = strtok(input, " \n");

            while(token!=NULL) {
                char* prev_word = strdup(token);
                token = strtok(NULL, " \n");
                if (token == NULL) break;
                char* word = strdup(token);
                char bigram[2* MAX_WORD_LENGTH];
                lower1(prev_word);
                lower1(word);

                strcpy(bigram, prev_word);
                strcat(bigram, " ");
                strcat(bigram, word);

                int bucketIndex = hash(bigram); 
                BigramNode* node = hashTable[bucketIndex];
                node = rScanList(node, bigram);
                // printNodeToFile(node);

                free(prev_word);
                free(word);
            }
        }
        close(fd);
    }
    else {
        printf("파일 열기에 실패했습니다");
    }
    
    QuickSortExe();
    // printBigrams(hashTable[0]);

    time_t end = time(NULL);
    int time_taken = (int)(end-start);
    int min = time_taken / 60;
    int sec = time_taken % 60;
    int hour = min / 24;
    min = min % 24;
    printf("Time: %dh %dm %ds\n", hour, min, sec);
    printf("\n");
    // countUniqueBigram(hashTable[0]);
    
    return 0;
}
