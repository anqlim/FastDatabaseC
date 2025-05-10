#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_ERROR 1
#define MEMORY_ALLOCATION_ERROR 2
#define INCORRECT_INSTRUCTION 3
#define KEY_EXISTS 4
#define NOT_FOUND 5

#define MAX_STRING_LENGTH 512

int mallocCount = 0, reallocCount = 0, callocCount = 0, freeCount = 0;

typedef struct Record {
	unsigned long long number;
	char prefix;
	char* name;
} Record;

int keyIsCorrect(char* string) {
	if (*string == '\"') {
		string++;
		if (*string == '8' || *string == '+' && *(++string) == '7') {
			string++;
			int i;
			for (i = 0; string[i] != '\"'; i++) {
				if (!(string[i] >= '0' && string[i] <= '9')) {
					return 0;
				}
			}
			if (i == 10) {
				return 1;
			}
		}
	}
	return 0;
}

typedef struct Tree {
	Record* record;
	struct Tree* leftTree;
	struct Tree* rightTree;
	int height;
} Tree;

typedef struct Database {
	Tree* root;
	int size;
} Database;

int Max(int a, int b);
char* skipSpaces(char* string);
char* getNumber(Record* record, char* string);

void freeRecord(Record** record);
void freeNode(Tree** node);
void freeTree(Tree** root);

int getHeight(Tree* node);
void updateHeight(Tree* node);
int isBalanced(Tree* node);
Tree* rotateLL(Tree* y);
Tree* rotateRR(Tree* x);
Tree* rotateLR(Tree* node);
Tree* rotateRL(Tree* node);
Tree* balancing(Tree* node);
int insertion(Tree** current, Record* currentRecord);
char* searchName(Tree* current, unsigned long long* target);
Tree** findLeftMax(Tree** node);
Tree** findRightMin(Tree** node);
void copyRecord(Record* dst, Record* src);
int deletion(Tree** current, Record* key);
void printInfix(Tree* node, char** cursor);

int insert(Database* database, char* string);
char* find(Database* database, char* string);
int del(Database* database, char* string);
int tree(Database* database, FILE* outputFile);

int parsing(Database* database, FILE* inputFile, FILE* outputFile);

int main() {
	FILE* inputFile = fopen("input.txt", "r");
	FILE* outputFile = fopen("output.txt", "w");
	if (!(inputFile && outputFile)) {
		if (inputFile) {
			fclose(inputFile);
		}
		if (outputFile) {
			fclose(outputFile);
		}
		return FILE_ERROR;
	}

	Database* database = (Database*)calloc(1, sizeof(Database));
	callocCount++;
	if (!database) {
		fclose(inputFile);
		fclose(outputFile);
		return MEMORY_ALLOCATION_ERROR;
	}

	if (parsing(database, inputFile, outputFile) == MEMORY_ALLOCATION_ERROR) {
		return MEMORY_ALLOCATION_ERROR;
	}
	freeTree(&database->root);
	free(database);
	freeCount++;

	fclose(inputFile);
	fclose(outputFile);

	FILE* memstat = fopen("memstat.txt", "w");
	fprintf(memstat, "malloc:%d\nrealloc:%d\ncalloc:%d\nfree:%d", 
		mallocCount, reallocCount, callocCount, freeCount);
	fclose(memstat);

	return 0;
}

int Max(int a, int b) {
	return (a > b) ? a : b;
}
char* skipSpaces(char* string) {
	while (*string == ' ') string++;
	return string;
}
char* getNumber(Record* record, char* string) {
	string++;
	if (*string == '8') {
		record->prefix = '8';
		string++;
	}
	else {
		record->prefix = '7';
		string += 2;
	}

	while (*string != '\"') {
		record->number = record->number * 10 + (*string - '0');
		string++;
	}
	return string;
}

int parsing(Database* database, FILE* inputFile, FILE* outputFile) {
	char currentString[MAX_STRING_LENGTH];
	while (fgets(currentString, sizeof(currentString), inputFile)) {
		currentString[strcspn(currentString, "\n")] = '\0';
		if (!strlen(currentString)) {
			continue;
		}
		char* string = currentString;
		if (!strncmp(string, "insert", strlen("insert"))) {
			int res = insert(database, skipSpaces(string + strlen("insert")));
			switch (res) {
			case 0: {
				fprintf(outputFile, "insert:success\n");
				break;
			}
			case KEY_EXISTS: {
				fprintf(outputFile, "insert:e_exist\n");
				break;
			}
			case INCORRECT_INSTRUCTION: {
				fprintf(outputFile, "incorrect\n");
				break;
			}
			case MEMORY_ALLOCATION_ERROR: {
				return MEMORY_ALLOCATION_ERROR;
			}
			}
		}
		else if (!strncmp(string, "find", strlen("find"))) {
			char* res = find(database, skipSpaces(string + strlen("find")));
			if (!res)
				fprintf(outputFile, "incorrect\n");
			else 
				fprintf(outputFile, "find:%s\n", res);
		}
		else if (!strncmp(string, "delete", strlen("delete"))) {
			int res = del(database, skipSpaces(string + strlen("delete")));
			switch (res) {
			case 0: {
				fprintf(outputFile, "delete:success\n");
				break;
			}
			case INCORRECT_INSTRUCTION: {
				fprintf(outputFile, "incorrect\n");
				break;
			}
			case NOT_FOUND: {
				fprintf(outputFile, "delete:not_found\n");
				break;
			}
			}
		}
		else if (!strncmp(string, "tree", strlen("tree"))) {
			if (*(skipSpaces(string + strlen("tree"))) != '\0')
				fprintf(outputFile, "incorrect\n");
			else {
				int res = tree(database, outputFile);
				if (res == MEMORY_ALLOCATION_ERROR) return MEMORY_ALLOCATION_ERROR;
			}
		}
		else {
			fprintf(outputFile, "incorrect\n");
		}
	}
	return 0;
}

int insert(Database* database, char* string) {
	if (!keyIsCorrect(string)) return INCORRECT_INSTRUCTION;

	Record* currentRecord = (Record*)calloc(1, sizeof(Record));
	callocCount++;
	if (!currentRecord) {
		return MEMORY_ALLOCATION_ERROR;
	}
	string = skipSpaces(getNumber(currentRecord, string) + 1);
	if (*string != '\"')
		return INCORRECT_INSTRUCTION;
	string++;
	int slashes = 0, length;
	for (length = 0; !((string[length] == '\"' || string[length] == '\0') && !(slashes % 2)); length++) {
		if (string[length] == '\\') slashes++;
		else slashes = 0;
	}
	if(string[length] != '\"')
		return INCORRECT_INSTRUCTION;
	currentRecord->name = (char*)malloc((length + 3) * sizeof(char));
	mallocCount++;
	if (!currentRecord->name) {
		return MEMORY_ALLOCATION_ERROR;
	}
	strncpy(currentRecord->name, string - 1, length + 2);
	currentRecord->name[length + 2] = '\0';

	int res = insertion(&database->root, currentRecord);
	if (!res) database->size++;
	return res;
}
int insertion(Tree** current, Record* currentRecord) {
	if (!*current) {
		*current = (Tree*)calloc(1, sizeof(Tree));
		callocCount++;
		if (!*current) {
			free(currentRecord->name);
			free(currentRecord);
			freeCount += 2;
			return MEMORY_ALLOCATION_ERROR;
		}
		(*current)->record = currentRecord;
		(*current)->height = 1;
		return 0;
	}
	if (currentRecord->number > (*current)->record->number) {
		int res = insertion(&(*current)->rightTree, currentRecord);
		if (res) return res;
	}
	else if (currentRecord->number < (*current)->record->number) {
		int res = insertion(&(*current)->leftTree, currentRecord);
		if (res) return res;
	}
	else {
		free(currentRecord->name);
		free(currentRecord);
		freeCount += 2;
		return KEY_EXISTS;
	}

	updateHeight(*current);
	*current = balancing(*current);
	return 0;
}
int getHeight(Tree* node) {
	if (!node) return 0;
	return node->height;
}
void updateHeight(Tree* node) {
	if (node) {
		node->height = 1 + Max(getHeight(node->leftTree), getHeight(node->rightTree));
	}
}

Tree* balancing(Tree* node) {
	int balance = isBalanced(node);
	if (balance > 1) {
		if (getHeight(node->leftTree->leftTree) >= getHeight(node->leftTree->rightTree))
			return rotateLL(node);
		return rotateLR(node);
	}
	if (balance < -1) {
		if (getHeight(node->rightTree->rightTree) >= getHeight(node->rightTree->leftTree))
			return rotateRR(node);
		return rotateRL(node);
	}
	updateHeight(node);
	return node;
}
int isBalanced(Tree* node) {
	if (node == NULL)
		return 0;
	return getHeight(node->leftTree) - getHeight(node->rightTree);
}
Tree* rotateLL(Tree* y) {
	Tree* x = y->leftTree;
	Tree* t2 = x->rightTree;

	x->rightTree = y;
	y->leftTree = t2;

	updateHeight(y);
	updateHeight(x);

	return x;
}
Tree* rotateRR(Tree* x) {
	Tree* y = x->rightTree;
	Tree* t2 = y->leftTree;

	y->leftTree = x;
	x->rightTree = t2;

	updateHeight(x);
	updateHeight(y);

	return y;
}
Tree* rotateLR(Tree* node) {
	node->leftTree = rotateRR(node->leftTree); 
	return rotateLL(node);                   
}
Tree* rotateRL(Tree* node) {
	node->rightTree = rotateLL(node->rightTree); 
	return rotateRR(node);
}

char* find(Database* database, char* string) {
	if (!keyIsCorrect(string)) return NULL;

	Record targetRecord = { 0, 0, NULL };
	getNumber(&targetRecord, string);
	return searchName(database->root, &targetRecord.number);
}
char* searchName(Tree* current, unsigned long long* key) {
	if (!current) {
		return "not_found";
	}
	if (*key > current->record->number) {
		return searchName(current->rightTree, key);
	}
	if (*key < current->record->number) {
		return searchName(current->leftTree, key);
	}
	return current->record->name;
}

int del(Database* database, char* string) {
	if (!keyIsCorrect(string)) return INCORRECT_INSTRUCTION;

	Record targetRecord = { 0, 0, NULL };
	getNumber(&targetRecord, string);
	int res = deletion(&database->root, &targetRecord);
	if (!res) database->size--;
	return res;
}
int deletion(Tree** current, Record* key) {
	if (!*current)
		return NOT_FOUND;

	if (key->number > (*current)->record->number) {
		int res = deletion(&(*current)->rightTree, key);
		if (res) return res;
	}
	else if (key->number < (*current)->record->number) {
		int res = deletion(&(*current)->leftTree, key);
		if (res) return res;
	}
	else {
		Tree* nodeToDelete = *current;

		if (!nodeToDelete->leftTree && !nodeToDelete->rightTree) {
			freeNode(current);
		}
		else {
			Tree** swapNodePtr = (getHeight(nodeToDelete->leftTree) >= getHeight(nodeToDelete->rightTree))
				? findLeftMax(&nodeToDelete->leftTree)
				: findRightMin(&nodeToDelete->rightTree);
			
			copyRecord(nodeToDelete->record, (*swapNodePtr)->record);

			int res = deletion(swapNodePtr, (*swapNodePtr)->record);
			if (res) return res;
		}
	}
	updateHeight(*current);
	*current = balancing(*current);
	return 0;
}
void copyRecord(Record* dst, Record* src) {
	dst->prefix = src->prefix;
	dst->number = src->number;
	if (strlen(dst->name) >= strlen(src->name)) {
		strcpy(dst->name, src->name);
	}
	else {
		dst->name = (char*)realloc(dst->name, (strlen(src->name) + 1) * sizeof(char));
		reallocCount++;
		strcpy(dst->name, src->name);
	}
}
Tree** findLeftMax(Tree** node) {
	if ((*node)->rightTree) {
		Tree** res =  findLeftMax(&(*node)->rightTree);
		*node = balancing(*node);
		return res;
	}
	(*node)->height--;
	return node;
}
Tree** findRightMin(Tree** node) {
	if ((*node)->leftTree) {
		Tree** res = findRightMin(&(*node)->leftTree);
		*node = balancing(*node);
		return res;
	}
	(*node)->height--;
	return node;
}
void freeNode(Tree** node) {
	if (!*node) return;
	freeRecord(&(*node)->record);
	free(*node);
	freeCount++;
	*node = NULL;
}
void freeRecord(Record** record) {
	if (!*record) return;
	if ((*record)->name) {
		free((*record)->name);
		freeCount++;
	}
	free(*record);
	freeCount++;
	*record = NULL;
}
void freeTree(Tree** root) {
	if (!*root) return;
	freeTree(&(*root)->leftTree);
	freeTree(&(*root)->rightTree);
	freeNode(root);
}

int tree(Database* database, FILE* outputFile) {
	if (!database->root) {
		fprintf(outputFile, "tree:(null)\n");
		return 0;
	}

	char* buffer = (char*)malloc(database->size * 50 * sizeof(char));
	mallocCount++;
	if (!buffer) return MEMORY_ALLOCATION_ERROR;
	char* cursor = buffer;
	printInfix(database->root, &cursor);
	fprintf(outputFile, "tree:%s\n", buffer);
	free(buffer);
	freeCount++;
	return 0;
}
void printInfix(Tree* node, char** cursor) {
	if (!node) {
		int written = sprintf(*cursor, "(null)");
		*cursor += written;
		return;
	}
	int written = sprintf(*cursor, "(");
	*cursor += written;
	if (node->leftTree || node->rightTree) {
		printInfix(node->leftTree, cursor);
		written = sprintf(*cursor, ",");
		*cursor += written;
	}

	char prefix[3];
	if (node->record->prefix == '7')
		strcpy(prefix, "+7");
	else
		strcpy(prefix, "8");

	written = sprintf(*cursor, "\"%s%010llu\"",
		prefix, node->record->number);

	*cursor += written;

	if (node->leftTree || node->rightTree) {
		written = sprintf(*cursor, ",");
		*cursor += written;
		printInfix(node->rightTree, cursor);
	}
	written = sprintf(*cursor, ")");
	*cursor += written;
}