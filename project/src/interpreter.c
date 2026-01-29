#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "interpreter.h"
#include "shellmemory.h"
#include "shell.h"

int MAX_ARGS_SIZE = 3;

int badcommand() {
    printf("Unknown Command\n");
    return 1;
}

// For source command only
int badcommandFileDoesNotExist() {
    printf("Bad command: File not found\n");
    return 3;
}

int help();
int quit();
int set(char *var, char *value);
int print(char *var);
int source(char *script);
int badcommandFileDoesNotExist();
//echo function
int echo(char *text);
int my_ls();
int my_mkdir(char *dir);
int my_touch(char *file);
int my_cd(char *dir);
int run(char *args[], int args_size);

// Interpret commands and their arguments
int interpreter(char *command_args[], int args_size) {
    int i;

    if (args_size < 1 || args_size > MAX_ARGS_SIZE) {
        return badcommand();
    }

    for (i = 0; i < args_size; i++) {   // terminate args at newlines
        command_args[i][strcspn(command_args[i], "\r\n")] = 0;
    }

    if (strcmp(command_args[0], "help") == 0) {
        //help
        if (args_size != 1)
            return badcommand();
        return help();

    } else if (strcmp(command_args[0], "quit") == 0) {
        //quit
        if (args_size != 1)
            return badcommand();
        return quit();

    } else if (strcmp(command_args[0], "set") == 0) {
        //set
        if (args_size != 3)
            return badcommand();
        return set(command_args[1], command_args[2]);

    } else if (strcmp(command_args[0], "print") == 0) {
        if (args_size != 2)
            return badcommand();
        return print(command_args[1]);

    } else if (strcmp(command_args[0], "source") == 0) {
        if (args_size != 2)
            return badcommand();
        return source(command_args[1]);

    } else if (strcmp(command_args[0], "echo") == 0){
        if (args_size != 2){
            return badcommand();
        }
        return echo(command_args[1]);
    } else if (strcmp(command_args[0], "my_ls") == 0) {
        if (args_size != 1)
            return badcommand();
        return my_ls();

    } else if (strcmp(command_args[0], "my_mkdir") == 0) {
        if (args_size != 2)
            return badcommand();
        return my_mkdir(command_args[1]);

    } else if (strcmp(command_args[0], "my_touch") == 0) {
        if (args_size != 2)
            return badcommand();
        return my_touch(command_args[1]);

    } else if (strcmp(command_args[0], "my_cd") == 0) {
        if (args_size != 2)
            return badcommand();
        return my_cd(command_args[1]);

    } else if (strcmp(command_args[0], "run") == 0) {
        if (args_size < 2)
            return badcommand();
        return run(command_args, args_size);

    }
    else
        return badcommand();
}

int help() {

    // note the literal tab characters here for alignment
    char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
source SCRIPT.TXT	Executes the file SCRIPT.TXT\n ";
    printf("%s\n", help_string);
    return 0;
}

int quit() {
    printf("Bye!\n");
    exit(0);
}

int set(char *var, char *value) {
    // Challenge: allow setting VAR to the rest of the input line,
    // possibly including spaces.

    // Hint: Since "value" might contain multiple tokens, you'll need to loop
    // through them, concatenate each token to the buffer, and handle spacing
    // appropriately. Investigate how `strcat` works and how you can use it
    // effectively here.

    mem_set_value(var, value);
    return 0;
}


int print(char *var) {
    printf("%s\n", mem_get_value(var));
    return 0;
}

int source(char *script) {
    int errCode = 0;
    char line[MAX_USER_INPUT];
    FILE *p = fopen(script, "rt");      // the program is in a file

    if (p == NULL) {
        return badcommandFileDoesNotExist();
    }

    fgets(line, MAX_USER_INPUT - 1, p);
    while (1) {
        errCode = parseInput(line);     // which calls interpreter()
        memset(line, 0, sizeof(line));

        if (feof(p)) {
            break;
        }
        fgets(line, MAX_USER_INPUT - 1, p);
    }

    fclose(p);

    return errCode;
}

int echo(char *text) {
    if (text[0] == '$') {
        char *varName = text + 1;
        char *value = mem_get_value(varName);
        if (value != NULL && strcmp(value, "Variable does not exist") != 0) {   // to avoid changing memory get
            printf("%s\n", value);
            return 0;
        }
        else {
            printf("\n");
            return 0;
        }
    }
    printf("%s\n", text);
    return 0;
}

int compare(const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

int my_ls() {
    struct dirent *entry;
    DIR *dir = opendir(".");

    if (dir == NULL) {
        return badcommand();
    }

    char* buffer[100];
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        buffer[count] = strdup(entry->d_name);
        count++;
    }

    closedir(dir);
    qsort(buffer, count, sizeof(char *), compare);
    for (int i = 0; i < count; i++) {
        printf("%s\n", buffer[i]);
    }

    return 0;
}

int is_alphanumeric(char* str) {
    if (str == NULL || strlen(str) == 0) {
        return 0; 
    }

    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalnum(str[i])) {
            return 0; 
        }
    }
    return 1;
}   

int my_mkdir(char *dir) {
    char *newdir = NULL;

    // case variable reference
    if (dir[0] == '$') {
        char* var_name = dir + 1;
        char* value = mem_get_value(var_name);
        // if variable doesnt exist
        if (strcmp(value, "Variable does not exist") == 0) {
            printf("Bad command: my_mkdir\n");
            return 1;
        }

        if (!is_alphanumeric(value)) {
            printf("Bad command: my_mkdir\n");
            return 1;
        }
        newdir = value;

    } else { 
        // case direct input
        if (!is_alphanumeric(dir)) {
            printf("Bad command: my_mkdir\n");
            return 1;
        }
        newdir = dir;
    }

    // create dir
    if (mkdir(newdir, 0755) == -1) {
        printf("Bad command: my_mkdir\n");
        return 1;
    }

    return 0;
}

int my_touch(char *file) {
    FILE *fp = fopen(file, "a");
    if (fp == NULL) {
        printf("Bad command: my_touch\n"); // need ????? (dont see it in test case)
        return badcommand();
    }
    fclose(fp);
    return 0;
}

int my_cd(char *dir) {
    struct stat sb;
    // check if dir exists
    if (stat(dir, &sb) != 0 || !S_ISDIR(sb.st_mode)) {
        printf("Bad command: my_cd\n");
        return 1;
    }
    // change dir
    if (chdir(dir) == -1) {
        printf("Bad command: my_cd\n");
        return 1;
    }
    return 0;
}

int run(char *args[], int args_size) {
    pid_t pid = fork();
    if (pid < 0) {
        // fork failed
        return badcommand();
    }

    if (pid == 0) {
        // child: build argv for execvp from args[1..]
        int exec_argc = args_size - 1;
        char **exec_args = malloc((exec_argc + 1) * sizeof(char *));
        if (exec_args == NULL) {
            return badcommand();
        }
        for (int i = 0; i < exec_argc; i++) {
            exec_args[i] = args[i + 1];
        }
        exec_args[exec_argc] = NULL;

        execvp(exec_args[0], exec_args);
        // if execvp returns, error
        return badcommand();   
    } else {
        // parent wait for child 
        int status = 0;
        waitpid(pid, &status, 0);
        return 0;
    }
}
