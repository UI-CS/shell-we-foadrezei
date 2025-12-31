#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 11  // 9 subgrids + 1 rows + 1 columns
#define BOARD_SIZE 9

// Global Sudoku board
int sudoku[BOARD_SIZE][BOARD_SIZE];

// Global validation results
int valid[NUM_THREADS];

// Thread parameter structure
typedef struct {
    int row;
    int column;
    int thread_id;
} parameters;

// Function to validate rows
void *validate_rows(void *param) {
    parameters *data = (parameters *) param;
    int thread_id = data->thread_id;
    
    for (int row = 0; row < BOARD_SIZE; row++) {
        int checker[BOARD_SIZE + 1] = {0}; // Index 0 unused, 1-9 for digits
        
        for (int col = 0; col < BOARD_SIZE; col++) {
            int value = sudoku[row][col];
            if (value < 1 || value > 9 || checker[value] == 1) {
                valid[thread_id] = 0;
                pthread_exit(NULL);
            }
            checker[value] = 1;
        }
    }
    
    valid[thread_id] = 1;
    pthread_exit(NULL);
}

// Function to validate columns
void *validate_columns(void *param) {
    parameters *data = (parameters *) param;
    int thread_id = data->thread_id;
    
    for (int col = 0; col < BOARD_SIZE; col++) {
        int checker[BOARD_SIZE + 1] = {0}; // Index 0 unused, 1-9 for digits
        
        for (int row = 0; row < BOARD_SIZE; row++) {
            int value = sudoku[row][col];
            if (value < 1 || value > 9 || checker[value] == 1) {
                valid[thread_id] = 0;
                pthread_exit(NULL);
            }
            checker[value] = 1;
        }
    }
    
    valid[thread_id] = 1;
    pthread_exit(NULL);
}

// Function to validate a 3x3 subgrid
void *validate_subgrid(void *param) {
    parameters *data = (parameters *) param;
    int start_row = data->row;
    int start_col = data->column;
    int thread_id = data->thread_id;
    
    int checker[BOARD_SIZE + 1] = {0}; // Index 0 unused, 1-9 for digits
    
    for (int row = start_row; row < start_row + 3; row++) {
        for (int col = start_col; col < start_col + 3; col++) {
            int value = sudoku[row][col];
            if (value < 1 || value > 9 || checker[value] == 1) {
                valid[thread_id] = 0;
                pthread_exit(NULL);
            }
            checker[value] = 1;
        }
    }
    
    valid[thread_id] = 1;
    pthread_exit(NULL);
}

// Function to print the Sudoku board
void print_sudoku() {
    printf("\nSudoku Board:\n");
    printf("┌───────┬───────┬───────┐\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i == 3 || i == 6) {
            printf("├───────┼───────┼───────┤\n");
        }
        printf("│ ");
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%d ", sudoku[i][j]);
            if (j == 2 || j == 5) {
                printf("│ ");
            }
        }
        printf("│\n");
    }
    printf("└───────┴───────┴───────┘\n");
}

// Function to load Sudoku from file
int load_sudoku_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (fscanf(file, "%d", &sudoku[i][j]) != 1) {
                printf("Error: Invalid file format\n");
                fclose(file);
                return 0;
            }
        }
    }
    
    fclose(file);
    return 1;
}

// Function to create a sample valid Sudoku
void create_sample_sudoku() {
    int sample[BOARD_SIZE][BOARD_SIZE] = {
        {5, 3, 4, 6, 7, 8, 9, 1, 2},
        {6, 7, 2, 1, 9, 5, 3, 4, 8},
        {1, 9, 8, 3, 4, 2, 5, 6, 7},
        {8, 5, 9, 7, 6, 1, 4, 2, 3},
        {4, 2, 6, 8, 5, 3, 7, 9, 1},
        {7, 1, 3, 9, 2, 4, 8, 5, 6},
        {9, 6, 1, 5, 3, 7, 2, 8, 4},
        {2, 8, 7, 4, 1, 9, 6, 3, 5},
        {3, 4, 5, 2, 8, 6, 1, 7, 9}
    };
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            sudoku[i][j] = sample[i][j];
        }
    }
}

// Function to create a sample invalid Sudoku
void create_invalid_sudoku() {
    int sample[BOARD_SIZE][BOARD_SIZE] = {
        {5, 3, 4, 6, 7, 8, 9, 1, 2},
        {6, 7, 2, 1, 9, 5, 3, 4, 8},
        {1, 9, 8, 3, 4, 2, 5, 6, 7},
        {8, 5, 9, 7, 6, 1, 4, 2, 3},
        {4, 2, 6, 8, 5, 3, 7, 9, 1},
        {7, 1, 3, 9, 2, 4, 8, 5, 6},
        {9, 6, 1, 5, 3, 7, 2, 8, 4},
        {2, 8, 7, 4, 1, 9, 6, 3, 5},
        {3, 4, 5, 2, 8, 6, 1, 7, 5}  // Invalid: 5 appears twice in last row
    };
    
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            sudoku[i][j] = sample[i][j];
        }
    }
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    parameters thread_params[NUM_THREADS];
    int thread_index = 0;
    
    printf("=== Parallel Sudoku Validator ===\n");
    
    // Initialize validation results
    for (int i = 0; i < NUM_THREADS; i++) {
        valid[i] = 0;
    }
    
    // Load Sudoku puzzle
    if (argc == 2) {
        if (!load_sudoku_from_file(argv[1])) {
            printf("Using sample valid Sudoku instead.\n");
            create_sample_sudoku();
        }
    } else if (argc == 1) {
        char choice;
        printf("No input file provided. Choose an option:\n");
        printf("v - Use valid sample Sudoku\n");
        printf("i - Use invalid sample Sudoku\n");
        printf("Choice: ");
        scanf(" %c", &choice);
        
        if (choice == 'i' || choice == 'I') {
            create_invalid_sudoku();
            printf("Using sample invalid Sudoku for testing.\n");
        } else {
            create_sample_sudoku();
            printf("Using sample valid Sudoku.\n");
        }
    } else {
        printf("Usage: %s [sudoku_file]\n", argv[0]);
        printf("   or: %s (for interactive sample selection)\n", argv[0]);
        return 1;
    }
    
    print_sudoku();
    
    printf("\nValidating Sudoku using %d threads...\n", NUM_THREADS);
    
    // Create thread to validate all rows
    thread_params[thread_index].row = 0;
    thread_params[thread_index].column = 0;
    thread_params[thread_index].thread_id = thread_index;
    pthread_create(&threads[thread_index], NULL, validate_rows, &thread_params[thread_index]);
    thread_index++;
    
    // Create thread to validate all columns
    thread_params[thread_index].row = 0;
    thread_params[thread_index].column = 0;
    thread_params[thread_index].thread_id = thread_index;
    pthread_create(&threads[thread_index], NULL, validate_columns, &thread_params[thread_index]);
    thread_index++;
    
    // Create 9 threads to validate each 3x3 subgrid
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            thread_params[thread_index].row = i * 3;
            thread_params[thread_index].column = j * 3;
            thread_params[thread_index].thread_id = thread_index;
            pthread_create(&threads[thread_index], NULL, validate_subgrid, &thread_params[thread_index]);
            thread_index++;
        }
    }
    
    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Check results
    printf("\nValidation Results:\n");
    printf("- Rows:     %s\n", valid[0] ? "Valid" : "Invalid");
    printf("- Columns:  %s\n", valid[1] ? "Valid" : "Invalid");
    printf("- Subgrids: ");
    
    int subgrids_valid = 1;
    for (int i = 2; i < NUM_THREADS; i++) {
        if (!valid[i]) {
            subgrids_valid = 0;
            break;
        }
    }
    printf("%s\n", subgrids_valid ? "Valid" : "Invalid");
    
    // Final result
    int is_valid_sudoku = 1;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (!valid[i]) {
            is_valid_sudoku = 0;
            break;
        }
    }
    
    printf("\n");
    if (is_valid_sudoku) {
        printf("✓ RESULT: This is a VALID Sudoku solution!\n");
    } else {
        printf("✗ RESULT: This is NOT a valid Sudoku solution.\n");
    }
    
    return 0;
}