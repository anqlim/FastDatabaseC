#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_ERROR 1
#define MEMORY_ALLOCATION_ERROR 2
#define INCORRECT_INSTRUCTION 3
#define KEY_EXISTS 4

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
			for (i = 0; *string != '\"'; i++) {
				if (!(*string >= '0' && *string <= '9')) {
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

char* skipSpaces(char* string);

int getHeight(Tree* node);
void updateHeight(Tree* node);
int isBalanced(Tree* node);
Tree* rotateLL(Tree* y);
Tree* rotateRR(Tree* x);
Tree* rotateLR(Tree* node);
Tree* rotateRL(Tree* node);
Tree* balancing(Tree* node);
int insertion(Tree** current, Record* currentRecord);

int insert(Database* database, char* string);
char* find(Database* database, char* string);
int deletion(Database* database, char* string);
char* tree(Database* database, char* string);

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

	if (parsing(database, inputFile, outputFile)) {
		//parsing errors
	}

	fclose(inputFile);
	fclose(outputFile);

	FILE* memstat = fopen("memstat.txt", "w");
	fprintf(memstat, "malloc:%d\nrealloc:%d\ncalloc:%d\nfree:%d", 
		mallocCount, reallocCount, callocCount, freeCount);
	fclose(memstat);

	return 0;
}

char* skipSpaces(char* string) {
	while (*string == ' ') string++;
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
			else {
				fprintf(outputFile, "find:%s\n", res);
				free(res);
				freeCount++;
			}
		}
		else if (!strncmp(string, "delete", strlen("delete"))) {
			int res = deletion(database, skipSpaces(string + strlen("delete")));
		}
		else if (!strncmp(string, "tree", strlen("tree"))) {
			char* res = tree(database, skipSpaces(string + strlen("tree")));
		}
		else {
			fprintf(outputFile, "incorrect\n");
		}
	}
}

int insert(Database* database, char* string) {
	if (!keyIsCorrect(string)) return INCORRECT_INSTRUCTION;
	string++;

	Record* currentRecord = (Record*)calloc(1, sizeof(Record));
	callocCount++;
	if (!currentRecord) {
		return MEMORY_ALLOCATION_ERROR;
	}

	if (*string == '8') {
		currentRecord->prefix = '8';
		string++;
	}
	else {
		currentRecord->prefix = '7';
		string += 2;
	}

	while (*string != '\"') {
		currentRecord->number = currentRecord->number * 10 + (*string - '0');
		string++;
	}

	string = skipSpaces(++string);
	if (*string != '\"') return INCORRECT_INSTRUCTION;
	string++;
	int slashes = 0, length;
	for (length = 0; !(string[length] == '\"' && !(slashes % 2)); length++) {
		if (string[length] == '\\') slashes++;
		else slashes = 0;
	}
	currentRecord->name = (char*)malloc((length + 1) * sizeof(char));
	mallocCount++;
	if (!currentRecord->name) {
		return MEMORY_ALLOCATION_ERROR;
	}
	strncpy(currentRecord->name, string, length);
	currentRecord->name[length] = '\0';

	int res = insertion(&database->root, currentRecord);
	if (!res) database->size++;
	return res;
}

int insertion(Tree** current, Record* currentRecord) {
	if (!*current) {
		*current = (Tree*)calloc(1, sizeof(Tree));
		callocCount++;
		if (!current) {
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

	updateHeight(current);
	current = balancing(current);
	return 0;
}

int getHeight(Tree* node) {
	if (!node) return 0;
	return node->height;
}

void updateHeight(Tree* node) {
	if (node) {
		node->height = 1 + max(getHeight(node->leftTree), getHeight(node->rightTree));
	}
}

Tree* balancing(Tree* node) {
	int balance = isBalanced(node);
	if (balance > 1) {
		if (node->leftTree->leftTree->height > node->leftTree->rightTree->height) 
			return rotateLL(node);
		return rotateLR(node);
	}
	if (balance < -1) {
		if (node->rightTree->rightTree->height > node->rightTree->leftTree->height)
			return rotateRR(node);
		return rotateRL(node);
	}
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

