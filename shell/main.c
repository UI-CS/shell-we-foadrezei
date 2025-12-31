#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_COMMAND_LEN 1000
#define MAX_NUM_ARGUMENTS 64
#define HISTORY_COUNT 10

// Global variables for command history
char history[HISTORY_COUNT][MAX_COMMAND_LEN];
int history_count = 0;

// Function to add command to history
void add_to_history(char *command) {
    if (strlen(command) > 0 && strcmp(command, "history") != 0 && strcmp(command, "!!") != 0) {
        strncpy(history[history_count % HISTORY_COUNT], command, MAX_COMMAND_LEN - 1);
        history[history_count % HISTORY_COUNT][MAX_COMMAND_LEN - 1] = '\0';
        history_count++;
    }
}

// Function to get last command from history
char* get_last_command() {
    if (history_count == 0) {
        return NULL;
    }
    return history[(history_count - 1) % HISTORY_COUNT];
}

// Function to tokenize input into command and arguments
char **parse_input(char *input) {
    char **tokens = malloc(MAX_NUM_ARGUMENTS * sizeof(char*));
    char *token;
    int i = 0;
    
    if (!tokens) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    
    token = strtok(input, " \t\n\r");
    while (token != NULL && i < MAX_NUM_ARGUMENTS - 1) {
        tokens[i] = malloc(strlen(token) + 1);
        strcpy(tokens[i], token);
        i++;
        token = strtok(NULL, " \t\n\r");
    }
    tokens[i] = NULL;
    return tokens;
}

// Function to free parsed command
void free_command(char **command) {
    int i = 0;
    while (command[i] != NULL) {
        free(command[i]);
        i++;
    }
    free(command);
}

// Function to check if command should run in background
int is_background_command(char **command) {
    int i = 0;
    while (command[i] != NULL) {
        if (strcmp(command[i], "&") == 0) {
            free(command[i]);
            command[i] = NULL; // Remove & from command
            return 1;
        }
        i++;
    }
    return 0;
}

// Function to execute a command
void execute_command(char **command) {
    pid_t pid;
    int status;
    int background = is_background_command(command);
    
    pid = fork();
    
    if (pid == 0) {
        // Child process
        if (execvp(command[0], command) == -1) {
            printf("Command not found: %s\n", command[0]);
            exit(1);
        }
    } else if (pid < 0) {
        // Fork failed
        perror("fork");
    } else {
        // Parent process
        if (!background) {
            waitpid(pid, &status, 0);
        } else {
            printf("[Process %d started in background]\n", pid);
        }
    }
}

// Built-in command: exit
int builtin_exit(char **args) {
    return 0; // Signal to exit the shell
}

// Built-in command: cd
int builtin_cd(char **args) {
    if (args[1] == NULL) {
        // No argument provided, go to home directory
        if (chdir(getenv("HOME")) != 0) {
            perror("cd");
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
        }
    }
    return 1;
}

// Built-in command: pwd
int builtin_pwd(char **args) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
    return 1;
}

// Built-in command: help
int builtin_help(char **args) {
    printf("Unix Shell - Available Commands:\n");
    printf("  exit       - Exit the shell\n");
    printf("  cd [dir]   - Change directory (default: home)\n");
    printf("  pwd        - Print working directory\n");
    printf("  help       - Show this help message\n");
    printf("  history    - Show command history\n");
    printf("  !!         - Execute last command\n");
    printf("\nFeatures:\n");
    printf("  [command] &     - Run command in background\n");
    printf("\nPress Ctrl+C to interrupt a running command\n");
    printf("Press Ctrl+D or type 'exit' to quit the shell\n");
    return 1;
}

// Built-in command: history
int builtin_history(char **args) {
    printf("Command History:\n");
    if (history_count == 0) {
        printf("No commands in history\n");
        return 1;
    }
    
    int start = (history_count > HISTORY_COUNT) ? history_count - HISTORY_COUNT : 0;
    for (int i = start; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i % HISTORY_COUNT]);
    }
    return 1;
}

// Check if command is built-in and execute it
int execute_builtin(char **args) {
    if (args[0] == NULL) {
        return 1; // Empty command
    }
    
    if (strcmp(args[0], "exit") == 0) {
        return builtin_exit(args);
    }
    
    if (strcmp(args[0], "cd") == 0) {
        return builtin_cd(args);
    }
    
    if (strcmp(args[0], "pwd") == 0) {
        return builtin_pwd(args);
    }
    
    if (strcmp(args[0], "help") == 0) {
        return builtin_help(args);
    }
    
    if (strcmp(args[0], "history") == 0) {
        return builtin_history(args);
    }
    
    return -1; // Not a built-in command
}

// Function to read input from user
char *read_line(void) {
    char *line = NULL;
    size_t bufsize = 0;
    
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS); // EOF (Ctrl+D)
        } else {
            perror("readline");
            exit(EXIT_FAILURE);
        }
    }
    
    return line;
}

// Main shell loop
void shell_loop(void) {
    char *line;
    char **args;
    int status = 1;
    
    do {
        printf("unixsh> ");
        line = read_line();
        
        // Remove newline character
        line[strcspn(line, "\n")] = 0;
        
        // Handle history command
        if (strcmp(line, "!!") == 0) {
            char *last_cmd = get_last_command();
            if (last_cmd == NULL) {
                printf("No commands in history\n");
                free(line);
                continue;
            }
            printf("%s\n", last_cmd);
            free(line);
            line = malloc(strlen(last_cmd) + 1);
            strcpy(line, last_cmd);
        } else {
            add_to_history(line);
        }
        
        args = parse_input(line);
        
        // Check for built-in commands first
        status = execute_builtin(args);
        if (status == -1) {
            // Not a built-in, execute external command
            execute_command(args);
            status = 1;
        }
        
        // Reap background processes (prevent zombies)
        while (waitpid(-1, NULL, WNOHANG) > 0);
        
        free(line);
        free_command(args);
        
    } while (status);
}

// Main function
int main(void) {
    printf("Unix Shell v1.0 - Basic Implementation\n");
    printf("Type 'help' for available commands\n");
    printf("Use Ctrl+D or 'exit' to quit\n\n");
    
    // Run the main shell loop
    shell_loop();
    
    printf("Shell terminated. Goodbye!\n");
    return EXIT_SUCCESS;
}