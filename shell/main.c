#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_COMMAND_LEN 1000
#define MAX_NUM_ARGUMENTS 64

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

// Function to execute a command
void execute_command(char **command) {
    pid_t pid;
    int status;
    
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
        // Parent process - wait for child
        waitpid(pid, &status, 0);
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
    printf("\nPress Ctrl+C to interrupt a running command\n");
    printf("Press Ctrl+D or type 'exit' to quit the shell\n");
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
        
        args = parse_input(line);
        
        // Check for built-in commands first
        status = execute_builtin(args);
        if (status == -1) {
            // Not a built-in, execute external command
            execute_command(args);
            status = 1;
        }
        
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