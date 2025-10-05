#include "../include/analyzer.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define MAX_PATH_LENGTH (1 << 10) 

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        fprintf(stderr, "Attention! Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    word_freq frequencies[MAX_WORDS]; 
    int words_num = 0;            

    // File to analyze
    analysis_results results;
    analyze_file(argv[1], &results, frequencies, &words_num);

    // Display and export results of analysis
    display_results(&results);

    // Display bar charts
    print_bar_chart(frequencies, words_num);

    printf("Do you want to save your results of analysis? (y/n): ");
    char user_choice;
    scanf("%c", &user_choice);
    while (getchar() != '\n'); // Clear the input buffer
    user_choice = tolower(user_choice);

    char output_path[MAX_PATH_LENGTH];

    if (user_choice == 'n')
        exit(EXIT_SUCCESS);

    printf("Where you want to save results? Enter the full path: ");

    if (fgets(output_path, MAX_PATH_LENGTH, stdin) == NULL) {
        perror("Error while reading input!\n");
        exit(EXIT_FAILURE);
    }

    output_path[strcspn(output_path, "\n")] = '\0';

    export_to_csv(output_path, &results, frequencies, words_num);
    printf("\nAnalysis was successfuly exported to %s!\n", output_path);

    return EXIT_SUCCESS;
}
