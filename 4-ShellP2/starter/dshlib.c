#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "dragon.h"
#include "dshlib.h"

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
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
        cmd_buff->_cmd_buffer = NULL;
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

        size_t len = 0;
        char *start = ptr;
        if (*ptr == '"') {  
            start = ++ptr; 
            while (*ptr != '\0' && *ptr != '"') {
                ptr++;
                len++;
            }
            if (*ptr == '"') {
                ptr++;
            } else {
                printf("Error: Missing closing quote\n");
                return ERR_CMD_ARGS_BAD;
            }
        } else {
            while (*ptr != '\0' && !isspace((unsigned char)*ptr)) {
                ptr++;
                len++;
            }
        }

        if (cmd_buff->argc >= CMD_ARGV_MAX - 1) {
            return ERR_TOO_MANY_COMMANDS;
        }

        cmd_buff->argv[cmd_buff->argc] = strndup(start, len);
        if (cmd_buff->argv[cmd_buff->argc] == NULL) { 
            return ERR_MEMORY;
        }
        cmd_buff->argc++;

        while (isspace((unsigned char)*ptr)) {
            ptr++;
        }
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

int last_return_code = 0; // Store last return code

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(input, "rc") == 0) {
        return BI_RC;
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
            if (cmd->argc > 1) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd failed");
                }
            }
            break;
        case BI_CMD_DRAGON:
            print_dragon();
            break;
        case BI_RC:
            printf("Last return code: %d\n", last_return_code);
            break;
        default:
            break;
    }
    return type;
}

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX + 1];
    cmd_buff_t cmd;

    if (alloc_cmd_buff(&cmd) == ERR_MEMORY) {
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }

        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';

        if (strlen(cmd_buff) == 0) {
            continue;
        }

        int rc = build_cmd_buff(cmd_buff, &cmd);
        if (rc != OK) {
            last_return_code = rc;
            continue;
        }

        Built_In_Cmds exe = exec_built_in_cmd(&cmd);
        if (exe != BI_NOT_BI) {
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            last_return_code = -1;
            continue;
        } else if (pid == 0) {
            execvp(cmd.argv[0], cmd.argv);
            int err = errno;

            switch (err) {
                case ENOENT:
                    fprintf(stderr, "Error: Command not found\n");
                    break;
                case EACCES:
                    fprintf(stderr, "Error: Permission denied\n");
                    break;
                default:
                    fprintf(stderr, "Error: Execution failed (errno %d)\n", err);
            }

            exit(err);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                last_return_code = WEXITSTATUS(status);
            } else {
                last_return_code = -1;
            }
        }
    }

    return OK;
}