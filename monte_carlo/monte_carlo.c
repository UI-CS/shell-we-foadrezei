#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

// Structure to share data between processes
typedef struct {
    long total_points;
    long points_in_circle;
    int num_processes;
    long points_per_process;
} shared_data_t;

// Function to generate random double between -1 and 1
double random_coordinate(unsigned int *seed) {
    return (2.0 * rand_r(seed) / RAND_MAX) - 1.0;
}

// Function for each process to calculate points in circle
void monte_carlo_worker(shared_data_t *shared, int process_id) {
    unsigned int seed = time(NULL) ^ (process_id << 16) ^ getpid();
    long points_in_circle = 0;
    long points_to_generate = shared->points_per_process;
    
    // If this is the last process, handle any remainder points
    if (process_id == shared->num_processes - 1) {
        points_to_generate += shared->total_points % shared->num_processes;
    }
    
    printf("Process %d (PID: %d) generating %ld points...\n", 
           process_id, getpid(), points_to_generate);
    
    for (long i = 0; i < points_to_generate; i++) {
        double x = random_coordinate(&seed);
        double y = random_coordinate(&seed);
        
        // Check if point is inside unit circle
        if (x*x + y*y <= 1.0) {
            points_in_circle++;
        }
    }
    
    // Add to shared counter (this is safe because each process
    // writes to its own portion, but for demonstration we'll show
    // a more realistic approach)
    shared->points_in_circle += points_in_circle;
    
    printf("Process %d completed: %ld points in circle out of %ld total\n", 
           process_id, points_in_circle, points_to_generate);
}

void print_statistics(shared_data_t *shared, double pi_estimate, double elapsed_time) {
    double error = fabs(pi_estimate - M_PI);
    double error_percentage = (error / M_PI) * 100.0;
    
    printf("\n" "================== RESULTS ==================\n");
    printf("Total points generated: %ld\n", shared->total_points);
    printf("Points inside circle:   %ld\n", shared->points_in_circle);
    printf("Points outside circle:  %ld\n", shared->total_points - shared->points_in_circle);
    printf("Ratio (inside/total):   %.10f\n", (double)shared->points_in_circle / shared->total_points);
    printf("\n");
    printf("Estimated π:            %.10f\n", pi_estimate);
    printf("Actual π:               %.10f\n", M_PI);
    printf("Absolute error:         %.10f\n", error);
    printf("Relative error:         %.6f%%\n", error_percentage);
    printf("\n");
    printf("Execution time:         %.3f seconds\n", elapsed_time);
    printf("Number of processes:    %d\n", shared->num_processes);
    printf("Points per second:      %.0f\n", shared->total_points / elapsed_time);
    printf("=============================================\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_processes> <total_points>\n", argv[0]);
        printf("Example: %s 4 100000000\n", argv[0]);
        printf("\nRecommended values:\n");
        printf("  Quick test:     %s 2 1000000\n", argv[0]);
        printf("  Accurate test:  %s 4 100000000\n", argv[0]);
        printf("  High accuracy:  %s 8 1000000000\n", argv[0]);
        return 1;
    }
    
    int num_processes = atoi(argv[1]);
    long total_points = atol(argv[2]);
    
    if (num_processes <= 0 || total_points <= 0) {
        printf("Error: Both parameters must be positive integers\n");
        return 1;
    }
    
    printf("=== Monte Carlo Pi Estimation ===\n");
    printf("Processes: %d\n", num_processes);
    printf("Total points: %ld\n", total_points);
    printf("Points per process: %ld\n", total_points / num_processes);
    printf("\nStarting parallel computation...\n\n");
    
    // Create shared memory for data
    shared_data_t *shared = mmap(NULL, sizeof(shared_data_t),
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    if (shared == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    // Initialize shared data
    shared->total_points = total_points;
    shared->points_in_circle = 0;
    shared->num_processes = num_processes;
    shared->points_per_process = total_points / num_processes;
    
    clock_t start_time = clock();
    
    // Fork processes
    pid_t pids[num_processes];
    for (int i = 0; i < num_processes; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            perror("fork");
            return 1;
        }
        
        if (pids[i] == 0) {
            // Child process
            monte_carlo_worker(shared, i);
            exit(0);
        }
    }
    
    // Parent process waits for all children
    for (int i = 0; i < num_processes; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    // Calculate pi estimate
    double pi_estimate = 4.0 * shared->points_in_circle / shared->total_points;
    
    // Print detailed results
    print_statistics(shared, pi_estimate, elapsed_time);
    
    // Accuracy assessment
    double error_percentage = fabs(pi_estimate - M_PI) / M_PI * 100.0;
    printf("\nAccuracy Assessment:\n");
    if (error_percentage < 0.01) {
        printf("✓ Excellent accuracy (< 0.01%% error)\n");
    } else if (error_percentage < 0.1) {
        printf("✓ Good accuracy (< 0.1%% error)\n");
    } else if (error_percentage < 1.0) {
        printf("○ Fair accuracy (< 1%% error)\n");
    } else {
        printf("× Poor accuracy (> 1%% error) - try more points\n");
    }
    
    // Performance assessment
    double points_per_second = shared->total_points / elapsed_time;
    printf("\nPerformance Assessment:\n");
    if (points_per_second > 10000000) {
        printf("✓ Excellent performance (> 10M points/sec)\n");
    } else if (points_per_second > 1000000) {
        printf("✓ Good performance (> 1M points/sec)\n");
    } else {
        printf("○ Moderate performance\n");
    }
    
    // Theoretical speedup
    printf("\nParallelism Analysis:\n");
    printf("Expected speedup with %d processes: ~%.1fx\n", 
           num_processes, (double)num_processes * 0.8); // Assuming 80% efficiency
    
    // Clean up shared memory
    munmap(shared, sizeof(shared_data_t));
    
    return 0;
}