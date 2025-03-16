#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dragon.h"

#include "dshlib.h"
/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    cmd_buff->_cmd_buffer = NULL;
    memset(cmd_buff->argv, 0, sizeof(cmd_buff->argv));
    
    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->append_mode = false;
    
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
    }
    
    if (cmd_buff->input_file) {
        free(cmd_buff->input_file);
        cmd_buff->input_file = NULL;
    }
    
    if (cmd_buff->output_file) {
        free(cmd_buff->output_file);
        cmd_buff->output_file = NULL;
    }
    
    return OK;
}


int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    free_cmd_buff(cmd_buff);
    return alloc_cmd_buff(cmd_buff);
}
   
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    clear_cmd_buff(cmd_buff);
    char *ptr = cmd_line;
    
    while (*ptr != '\0') {
        while (isspace((unsigned char)*ptr)) {
            ptr++;
        }

        if (*ptr == '\0') {
            break;
        }

        // Check for redirection operators
        if (*ptr == '<') {
            ptr++; // Skip the '<' character
            while (isspace((unsigned char)*ptr)) {
                ptr++;
            }
            
            char *start = ptr;
            size_t len = 0;
            
            while (!isspace((unsigned char)*ptr) && *ptr != '\0' && 
                   *ptr != '<' && *ptr != '>') {
                ptr++;
                len++;
            }
            
            cmd_buff->input_file = strndup(start, len);
            if (cmd_buff->input_file == NULL) {
                return ERR_MEMORY;
            }
            continue;
        }
        
        if (*ptr == '>') {
            ptr++; 
            
            // Check for append mode '>>'
            if (*ptr == '>') {
                cmd_buff->append_mode = true;
                ptr++;
            } else {
                cmd_buff->append_mode = false;
            }
            
            // Parse through whatever it is appending
            while (isspace((unsigned char)*ptr)) {
                ptr++;
            }
            
            char *start = ptr;
            size_t len = 0;
            
            while (!isspace((unsigned char)*ptr) && *ptr != '\0' && 
                   *ptr != '<' && *ptr != '>') {
                ptr++;
                len++;
            }
            
            cmd_buff->output_file = strndup(start, len);
            if (cmd_buff->output_file == NULL) {
                return ERR_MEMORY;
            }
            continue;
        }

        char *start = ptr;
        size_t len = 0;
        char quote_char = '\0';

        if (*ptr == '"' || *ptr == '\'') {
            quote_char = *ptr;
            start = ++ptr; 

            while (*ptr != quote_char && *ptr != '\0') {
                ptr++;
                len++;
            }

            if (*ptr == '\0') {  // Missing closing quote
                printf("Error: Unmatched quote\n");
                return ERR_CMD_ARGS_BAD;
            }
            ptr++;
        } else {
            while (!isspace((unsigned char)*ptr) && *ptr != '\0' && 
                   *ptr != '<' && *ptr != '>') {
                ptr++;
                len++;
            }
        }

        if (cmd_buff->argc >= CMD_ARGV_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        cmd_buff->argv[cmd_buff->argc] = strndup(start, len);
        if (cmd_buff->argv[cmd_buff->argc] == NULL) {
            return ERR_MEMORY;
        }
        cmd_buff->argc++;
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

// Used to close any previous command buffs
int close_cmd_buff(cmd_buff_t *cmd_buff) {
    free_cmd_buff(cmd_buff);
    free(cmd_buff);
    return OK;
}

// This just loops through and gets the commands and stores it in a list - I'm guessing to make it easier to parse
// through the commands
int build_cmd_list(char *cmd_line, command_list_t *clist) {
    memset(clist, 0, sizeof(command_list_t));
    clist->num = 0;

    char *ptr = cmd_line;

    // Checks for white space
    while (isspace((unsigned char)*ptr) && *ptr != '\0') {
        ptr++;
    }

    if (*ptr == '\0') {
        return WARN_NO_CMDS;
    }

    char *save_ptr;
    char *token = strtok_r(ptr, PIPE_STRING, &save_ptr);
    
    while (token != NULL) {
        if (clist->num >= CMD_ARGV_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Allocate and initialize a new command buffer
        cmd_buff_t *new_cmd = (cmd_buff_t *)malloc(sizeof(cmd_buff_t));
        if (new_cmd == NULL) {
            return ERR_MEMORY;
        }
        alloc_cmd_buff(new_cmd);
        
        int build_rc = build_cmd_buff(token, new_cmd);
        if (build_rc != OK) {
            free_cmd_buff(new_cmd);
            free(new_cmd);
            return build_rc;
        }

        clist->commands[clist->num] = *new_cmd;
        free(new_cmd);

        clist->num++;
        token = strtok_r(NULL, PIPE_STRING, &save_ptr);
    }

    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    if (cmd_lst == NULL) {
        return ERR_MEMORY;
    }

    for (int i = 0; i < cmd_lst->num; i++) {
        free_cmd_buff(&cmd_lst->commands[i]);
    }
    
    cmd_lst->num = 0;
    return OK;
}

// Prototype for executing one command
int exec_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds type = exec_built_in_cmd(cmd);
    if (type == BI_NOT_BI) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            return -1;
        } else if (pid == 0) {
            // Handle input redirection
            if (cmd->input_file) {
                int fd = open(cmd->input_file, O_RDONLY);
                if (fd == -1) {
                    perror("Input redirection failed");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    perror("Input redirection failed");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }
            
            // Handle output redirection
            if (cmd->output_file) {
                int flags = O_WRONLY | O_CREAT;
                
                if (cmd->append_mode) {
                    flags |= O_APPEND;
                } else {
                    flags |= O_TRUNC;
                }
                
                int fd = open(cmd->output_file, flags, 0644);
                if (fd == -1) {
                    perror("Output redirection failed");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Output redirection failed");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }
            
            if (execvp(cmd->argv[0], cmd->argv) == -1) {
                perror("Command execution failed");
                free_cmd_buff(cmd);
                exit(EXIT_FAILURE);
            }
        } else {
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("Wait failed");
                return -1;
            }
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            }
            return -1;
        }
    }
    return OK;
}

// Prototype to execute consecutve commands
int execute_pipeline(command_list_t *clist) {
    if (clist->num == 1) {
        return exec_cmd(&clist->commands[0]);
    }

    int pipes[clist->num - 1][2];
    pid_t pids[clist->num];

    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }

    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return -1;
        }
        
        if (pids[i] == 0) {
            if (i == 0 && clist->commands[i].input_file) {
                int fd = open(clist->commands[i].input_file, O_RDONLY);
                if (fd == -1) {
                    perror("Input redirection failed");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDIN_FILENO) == -1) {
                    perror("Input redirection failed");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            } else if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            
            // Last command can have output redirection
            if (i == clist->num - 1 && clist->commands[i].output_file) {
                int flags = O_WRONLY | O_CREAT;
                
                if (clist->commands[i].append_mode) {
                    flags |= O_APPEND;
                } else {
                    flags |= O_TRUNC;
                }
                
                int fd = open(clist->commands[i].output_file, flags, 0644);
                if (fd == -1) {
                    perror("Output redirection failed");
                    exit(EXIT_FAILURE);
                }
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("Output redirection failed");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            } else if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (execvp(clist->commands[i].argv[0], clist->commands[i].argv) == -1) {
                perror("Command execution failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < clist->num; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds type = match_command(cmd->argv[0]);

    switch (type) {
        case BI_CMD_EXIT:
            exit(0);
        case BI_CMD_CD:
            if(cmd->argc > 1) {
                if(chdir(cmd->argv[1]) != 0) {
                    perror("cd failed");
                }
            }
            break;
        case BI_CMD_DRAGON:
            print_dragon();
            break;
        default:
            break;
    }
    return type;
}

int exec_local_cmd_loop()
{
    int rc = 0;
    char cmd_buff[SH_CMD_MAX + 1];
    command_list_t cmd_list;

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }
        
        rc = build_cmd_list(cmd_buff, &cmd_list);
        if (rc == WARN_NO_CMDS) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf("%s\n", CMD_ERR_PIPE_LIMIT);
            continue;
        } else if (rc == ERR_MEMORY) {
            fprintf(stderr, "Memory allocation error.\n");
            continue;
        }

        int exec_rc = execute_pipeline(&cmd_list);
        if (exec_rc != OK) {
            printf("%d\n", ERR_EXEC_CMD);
            rc = exec_rc;
        }
        free_cmd_list(&cmd_list);
    }
    return rc;
}

