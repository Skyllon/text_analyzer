#include "../include/analyzer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <unistd.h>
#include <limits.h>

static int64_t sentence_count = 0;

void analyze_file(
    const char *filename, 
    analysis_results *results, 
    word_freq frequencies[], 
    int *words_num)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error! Can't open file\nStatus");
        exit(EXIT_SUCCESS);
    }

    results->word_count = results->line_count = results->char_count      = 0;
    results->longest_sentence_length                                     = 0;
    results->shortest_sentence_length                                    = INT_MAX;
    results->average_word_length = results->average_sentence_length      = 0;

    // Counter for amount of character and sentence length
    static char character;
    static int64_t sentence_length = 0,
                   word_length     = 0,
                   total_word_char = 0;

    // Stores a word, flag and index of word's pos
    char word[WORD_LENGTH];
    int wordIndex = 0;
    int found = 0;

    while ((character = fgetc(file)) != EOF) {
        results->char_count++; 

        if (isspace(character)) {
            if (word_length > 0) {
                // Increment amount of words in text
                results->word_count++;

                // Amount of characters in word
                total_word_char += word_length;
                
                // Reset word length
                word_length = 0; 
            }
            if (character == '\n')
                // Increment line counter if it's new line
                results->line_count++;
        }
        
        else if (ispunct(character)) {
            if (character == '.' || character == '?' || character == '!') {
                // Check max length of sentence
                if (sentence_length > results->longest_sentence_length)
                    results->longest_sentence_length  = sentence_length;
                if (sentence_length < results->shortest_sentence_length)
                    results->shortest_sentence_length = sentence_length;
                    
                // Increment amount of sentences
                ++sentence_count;
                
                // Begin of new sentence
                sentence_length = 0;
            }
        } else {            
            ++sentence_length;
            ++word_length;
        }
        if (isalnum(character)) {
            word[wordIndex++] = tolower(character); 
            word[wordIndex] = '\0';
        } else if (wordIndex > 0) {
            found = 0;

            for (int i = 0; i < *words_num; i++) {
                if (strcmp(frequencies[i].word, word) == 0) {
                    frequencies[i].frequency++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (*words_num < MAX_WORDS) {
                    strcpy(frequencies[*words_num].word, word);
                    frequencies[*words_num].frequency = 1;
                    (*words_num)++;
                }
            }
            wordIndex = 0;
        }
    }
   
    
    if (word_length > 0) {
        results->word_count++;
        total_word_char += word_length;
    }

    if (sentence_count == 0)
        results->shortest_sentence_length = 0; // No sentences, set to 0

    results->average_word_length = (results->word_count > 0)
        ? (double)total_word_char / results->word_count : 0.0;

    results->average_sentence_length = (sentence_count > 0)
        ? (double)results->char_count / sentence_count : 0.0;

    // Check if file is empty
    if (results->char_count == 0) {
        perror("File is empty!\nStatus");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void export_to_csv(
    const char *filename, 
    const analysis_results *results, 
    const word_freq frequencies[], 
    int words_num
) 
{
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }

    // Write analysis info
    fprintf(file, "Type of output: Metric - Value\n\n");
    fprintf(file, "----RESULTS----\n\n");
    fprintf(file, "Word Count - %d\n", 
            results->word_count);
    fprintf(file, "Line Count - %d\n", 
            results->line_count);
    fprintf(file, "Character Count - %d\n", 
            results->char_count - 1); // Remove zero symbol of string
    fprintf(file, "Longest Sentence Length (symbols amount except whitespace) - %d\n", 
            results->longest_sentence_length);
    fprintf(file, "Shortest Sentence Length (symbols amount except whitespace) - %d\n", 
            results->shortest_sentence_length);
    fprintf(file, "Average Word Length - %.2f\n", 
            results->average_word_length);
    fprintf(file, "Average Sentence Length - %.2f\n", 
            results->average_sentence_length);
    fprintf(file, "\n---------------\n");

    //Add the bar chart output:
    int max_freq = 0;
    for (int i = 0; i < words_num; i++)
        if (frequencies[i].frequency > max_freq)
            max_freq = frequencies[i].frequency;

    fprintf(file, "\nWord Frequency Bar Chart:\n");
    for (int i = 0; i < words_num; i++) {
        int bar_length = (int)((float)frequencies[i].frequency / max_freq * 40); 
        if (bar_length < 1) bar_length = 1; 
        fprintf(file, "%-20s |", frequencies[i].word); 

        for (int j = 0; j < bar_length; j++)
            fprintf(file, "#");

        fprintf(file, " (%d)\n", frequencies[i].frequency);
    }

    fclose(file);
}

void display_results(const analysis_results *results) 
{
    printf("Word Count: %d\n", 
        results->word_count);
    printf("Line Count: %d\n", 
        results->line_count);
    printf("Character Count: %d\n", 
        results->char_count - 1);
    printf("Longest Sentence Length (symbols amount except whitespace): %d\n", 
        results->longest_sentence_length);
    printf("Shortest Sentence Length (symbols amount except whitespace): %d\n", 
        results->shortest_sentence_length);
    printf("Average Word Length: %.2f\n", 
        results->average_word_length);
    printf("Average Sentence Length: %.2f\n", 
        results->average_sentence_length);
}

void print_bar_chart(const word_freq frequencies[], int words_num) {
    if (words_num <= 0) {
        printf("No words to display.\n");
        return;
    }

    int max_freq = 0;

    for (int i = 0; i < words_num; i++)
        if (frequencies[i].frequency > max_freq)
            max_freq = frequencies[i].frequency;

    printf("\nWord Frequency Bar Chart:\n");
    for (int i = 0; i < words_num; i++) {
        int bar_length = (int)((float)frequencies[i].frequency / max_freq * 40); // Scale to 40 chars
        if (bar_length < 1) bar_length = 1; // Avoid zero-length bars.
        printf("%-20s |", frequencies[i].word);

        for (int j = 0; j < bar_length; j++)
            printf("#");

        printf(" (%d)\n", frequencies[i].frequency);
    }
}
