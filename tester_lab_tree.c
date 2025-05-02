#define TEST
#ifdef TEST
#include <stdio.h>
#include <string.h>
#define MAX_STRING_LENGTH 512
#define BUFFER_SIZE 1000

int main() {
    FILE* expected_output = fopen("expected_output.txt", "r");
    FILE* output = fopen("output.txt", "r");
    char string[MAX_STRING_LENGTH], expected[MAX_STRING_LENGTH];
    int k = 1;
    while (fgets(string, sizeof(string), output) && fgets(expected, sizeof(expected), expected_output)) {
        string[strcspn(string, "\n")] = '\0';
        expected[strcspn(expected, "\n")] = '\0';
        if (strncmp(string, expected, strlen(expected))) {
            printf("fail\n");
            printf("string %d\nexpected:%s\nyours:%s\n\n", k, expected, string);
        }
        k++;
    }
    printf("ok\n");
    fclose(output);
    fclose(expected_output);
    return 0;
}
#endif