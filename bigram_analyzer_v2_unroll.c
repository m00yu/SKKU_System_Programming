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
	int x = data[right].frequency;
	int i = (left - 1);
    int j;
    for (j = left; j <= right - 4; j += 4)
    {
        if (data[j].frequency - x>0)
        {
            i++;
            Swap(&data[i], &data[j]);
        }
        if (data[j+1].frequency - x>0)
        {
            i++;
            Swap(&data[i], &data[j + 1]);
        }
        if (data[j+2].frequency - x>0)
        {
            i++;
            Swap(&data[i], &data[j + 1]);
        }
        if (data[j+3].frequency - x>0)
        {
            i++;
            Swap(&data[i], &data[j + 1]);
        }
    }
    for (; j <= right - 1; j++)
    {
        if (data[j].frequency > x)
        {
            i++;
            Swap(&data[i], &data[j]);
        }
    }


	Swap(&data[i + 1], &data[right]);

	return (i + 1);
}

void QuickSortIterative(BigramNode* data, int count) {
	int startIndex = 0;
	int endIndex = count - 1;
	int top = -1;
	int* stack = (int*)malloc(sizeof(int) * count);

	stack[++top] = startIndex;
	stack[++top] = endIndex;

	while (top >= 0)
	{
		endIndex = stack[top--];
		startIndex = stack[top--];

		int p = Partition(data, startIndex, endIndex);

		if (p - 1 > startIndex)
		{
			stack[++top] = startIndex;
			stack[++top] = p - 1;
		}

		if (p + 1 < endIndex)
		{
			stack[++top] = p + 1;
			stack[++top] = endIndex;
		}
	}

	free(stack);
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

    // //디버깅 - 일단 linked list 다 출력하기
    // FILE *file = fopen("output.txt", "a"); 
    // BigramNode* temp2 = firstNode;
    // fprintf(file, "----------------\n"); 
    // while (temp2 != NULL){
    //     fprintf(file, "%s %d | ", temp2->bigram, temp2->frequency);
    //     temp2 = temp2->next;
    // }
    // fclose(file);
    // exit(1);

	// 2. 하나로 연결된 linked list를 Quick sort하기
    printf("quick sort를 위한 준비중\n");
    BigramNode* tempCount = firstNode;
    int numNode = 0;
    while (tempCount != NULL){
        numNode++;
        tempCount = tempCount->next;
    }
    printf("%d\n", numNode);


    // Linked list를 array로
    BigramNode* curr = firstNode;
    BigramNode* nodeArray = (BigramNode*)malloc(sizeof(BigramNode)*numNode);
    for (int i=0; i<numNode; i++){
        strcpy(nodeArray[i].bigram, curr->bigram);
        nodeArray[i].frequency = curr->frequency;
        curr = curr->next;
    }
    printf("복사완료\n");
    
    // quick sort
    QuickSortIterative(nodeArray, numNode);

    for (int i=0; i<30; i++){
        printf("%s: %d\n", nodeArray[i].bigram, nodeArray[i].frequency);
    }
    free(nodeArray);
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
