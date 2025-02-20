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

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) {
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
    
    switch(type) {
        case BI_CMD_EXIT:
            exit(0);
            break;
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                chdir(cmd->argv[1]);
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
    char *cmd_buff[SH_CMD_MAX + 1];
    if (cmd_buff == NULL) {
        return ERR_MEMORY;
    }

    int rc = 0;
    cmd_buff_t cmd;
    int alloc = alloc_cmd_buff(&cmd);
    if (alloc == ERR_MEMORY) {
        return ERR_MEMORY;
    }
    // TODO IMPLEMENT MAIN LOOP

    // TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

    // TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
    // the cd command should chdir to the provided directory; if no directory is provided, do nothing

    // TODO IMPLEMENT if not built-in command, fork/exec as an external command
    // for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
    while(1){
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }
        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
        
        if (strlen(cmd_buff) == 0) {
            continue;
        }
        
        //IMPLEMENT THE REST OF THE REQUIREMENTS
        rc = build_cmd_buff(cmd_buff, &cmd);
        switch(rc) {
            case ERR_TOO_MANY_COMMANDS:
                return ERR_TOO_MANY_COMMANDS;
            case ERR_MEMORY:
                return ERR_MEMORY;
            case ERR_CMD_ARGS_BAD:
                return ERR_CMD_ARGS_BAD;
            default:
                break;
        }

        Built_In_Cmds exe = exec_built_in_cmd(&cmd);
        if (exe == BI_NOT_BI) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork failed");
                continue;
            } else if (pid == 0) {
                execvp(cmd.argv[0], cmd.argv);
                perror("Error executing external command");
                exit(1);
            } else {
                waitpid(pid, NULL, 0);
            }
        }
    }

    
    return OK;
}
