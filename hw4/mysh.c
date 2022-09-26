/* 
Clarence Jiang, yjia279
THIS  CODE  WAS MY OWN WORK , IT WAS  WRITTEN  WITHOUT  CONSULTING  ANYSOURCES  OUTSIDE  OF  THOSE  APPROVED  BY THE  INSTRUCTOR. 
Clarence Jiang  */


#include "tokens.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>


char * usage_error = "Error: Usage: mysh [prompt]\n";
char * ampersand_error = "Error: \"&\" must be last token on command line\n";
char * input_ambiguous_error = "Error: Ambiguous input redirection.\n";
char * input_missing_error = "Error: Missing filename for input redirection.\n";
char * output_ambiguous_error = "Error: Ambiguous output redirection.\n";
char * output_missing_error = "Error: Missing filename for output redirection.\n";
char * invalid_error = "Error: Invalid null command.\n";
char * open_error = "Error: open(\"%s\"): %s\n";

struct command
{
    char * input_file;
    char * output_file;
    char * subcommands[1024]; //It should be fine as we know the upper restricted 
    int append; 
    int pipeLeft;
    int pipeRight;

};


int main(int argc, char const *argv[])
{
    while(1){
        char * check; //used to check if library package methods function normally 
        int background_check = 0; //keep track if this command takes place in backend.
        int count_command = 1; //keep track how many individual commands exist in a pipe command.
        int error_check = 0;
        int check2;
        int count_foreground = 0;
        int status;

        if(argc == 1){
            fprintf(stdout, "mysh:");
        }
        else if(argc == 2){
            if (strcmp(argv[1], "-")){
                fprintf(stdout, "%s:", argv[1]);
            }
            else{
                fprintf(stdout, "%s", "");
            }
        }
        else{
            fprintf(stderr, "%s", usage_error);
            exit(1);
        }

        char shell_input[1024]; //store input 
        check = fgets(shell_input, sizeof(shell_input), stdin);  
        //Check with the check value 
        if ((strcmp(shell_input, "exit\n")==0) || (!check)){
            pid_t background_running = wait(&status);
            if (background_running > 0){
                kill(background_running, SIGTERM);
            }
            while (background_running != -1){
                background_running = wait(&status);
                if (background_running>0){
                    kill(background_running, SIGTERM);
                }
            }
           return 0;
        }

        struct command * cmd = calloc(1, sizeof(struct command));
        struct command ** command_list = calloc(1, sizeof(struct command*)*count_command);
        command_list[0] = cmd;

        char ** shell_input_divided = get_tokens(shell_input);

        int j = 0;
        for (int i=0; shell_input_divided[i]; i++){
            if(strcmp(shell_input_divided[i], "&") == 0){
                if (i == 0){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (shell_input_divided[i+1] && (strcmp(shell_input_divided[i+1], "|")==0
                ||strcmp(shell_input_divided[i+1], "&")==0
                ||strcmp(shell_input_divided[i+1], "<")==0
                ||strcmp(shell_input_divided[i+1], ">")==0
                ||strcmp(shell_input_divided[i+1], ">>")==0)){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if(shell_input_divided[i+1]!=NULL){
                    fprintf(stderr, "%s", ampersand_error);
                    error_check = 1;
                    break;
                }
                else{
                    background_check = 1;
                }
            }
            else if(strcmp(shell_input_divided[i], "<") == 0){
                if (i == 0){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (shell_input_divided[i+1] && (strcmp(shell_input_divided[i+1], "|")==0
                ||strcmp(shell_input_divided[i+1], "&")==0
                ||strcmp(shell_input_divided[i+1], "<")==0
                ||strcmp(shell_input_divided[i+1], ">")==0
                ||strcmp(shell_input_divided[i+1], ">>")==0)){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (cmd->input_file == NULL){
                    //check double ambiguous
                    int k = i;
                    while (k>=0){
                        if (strcmp(shell_input_divided[k], "|")==0){
                            fprintf(stderr, "%s", input_ambiguous_error);
                            error_check = 1;
                            break;
                        }
                        k--;
                    }

                    if (shell_input_divided[++i]!=NULL){
                        cmd->input_file = shell_input_divided[i];
                    }
                    else{
                        fprintf(stderr, "%s", input_missing_error);
                        error_check = 1;
                        break;
                    }
                }
                else{
                    fprintf(stderr, "%s", input_ambiguous_error);
                    error_check = 1;
                    break;
                }

            }
            else if(strcmp(shell_input_divided[i], ">") == 0){
                if (i == 0){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (shell_input_divided[i+1] && (strcmp(shell_input_divided[i+1], "|")==0
                ||strcmp(shell_input_divided[i+1], "&")==0
                ||strcmp(shell_input_divided[i+1], "<")==0
                ||strcmp(shell_input_divided[i+1], ">")==0
                ||strcmp(shell_input_divided[i+1], ">>")==0)){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (cmd->output_file == NULL){    
                    if (shell_input_divided[++i]!=NULL){
                        cmd->output_file = shell_input_divided[i];
                    }
                    else{
                        fprintf(stderr, "%s", output_missing_error);
                        error_check = 1;
                        break;
                    }
                }
                else{
                    fprintf(stderr, "%s", output_ambiguous_error);
                    error_check = 1;
                    break;
                }
            }
            else if(strcmp(shell_input_divided[i], ">>") == 0){
                if (i == 0){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (shell_input_divided[i+1] && (strcmp(shell_input_divided[i+1], "|")==0
                ||strcmp(shell_input_divided[i+1], "&")==0
                ||strcmp(shell_input_divided[i+1], "<")==0
                ||strcmp(shell_input_divided[i+1], ">")==0
                ||strcmp(shell_input_divided[i+1], ">>")==0)){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (cmd->output_file == NULL){
                    if (shell_input_divided[++i]!=NULL){
                        cmd->append = 1;
                        cmd->output_file = shell_input_divided[i];
                    }
                    else{
                        fprintf(stderr, "%s", output_missing_error);
                        error_check = 1;
                        break;
                    }
                }
                else{
                    fprintf(stderr, "%s", output_ambiguous_error);
                    error_check = 1;
                    break;
                }
            }
            else if(strcmp(shell_input_divided[i], "|") == 0){
                if (i == 0){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                if (shell_input_divided[i+1] && (strcmp(shell_input_divided[i+1], "|")==0
                ||strcmp(shell_input_divided[i+1], "&")==0
                ||strcmp(shell_input_divided[i+1], "<")==0
                ||strcmp(shell_input_divided[i+1], ">")==0
                ||strcmp(shell_input_divided[i+1], ">>")==0)){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                int k = i;
                while (k>=0){
                    if (strcmp(shell_input_divided[k], ">")==0){
                        fprintf(stderr, "%s", output_ambiguous_error);
                        error_check = 1;
                        break;
                    }
                    k--;
                }

                // no pipe can begin without an argument. 
                if (cmd->subcommands[0] == NULL){
                    fprintf(stderr, "%s", invalid_error);
                    error_check = 1;
                    break;
                }
                count_command++;

                command_list = realloc(command_list, sizeof(struct command *)*count_command);
                cmd->pipeLeft = 1;
                struct command * cmd2 = calloc(1, sizeof(struct command));
                command_list[count_command-1] = cmd2;
                command_list[count_command] = NULL;
                cmd2->pipeRight = 1;
                cmd = cmd2;
                j=0;
                
            }
            else{
                cmd->subcommands[j] = shell_input_divided[i];
                j++;
            }
        }

        if (error_check){
            continue;
        }
      
        //Deal with Pipe here    
        for(int i =0; command_list[i]; i++){
            if (command_list[i]->pipeLeft){
                int file_descriptors[2] = {-1, -1};
                check2 = pipe(file_descriptors);
                if (check2<0){
                    perror("Error: pipe");
                    error_check=1;
                    break;
                }
                else{
                    command_list[i]->pipeLeft = file_descriptors[1];
                } 
            
                if (command_list[i+1]){
                    command_list[i+1]->pipeRight = file_descriptors[0];
                }
            }    
        }
        if (error_check){
            continue;
        }

        //Execuate
        pid_t * pid_list;
        for (int i=0; command_list[i]; i++){
            pid_t pid = fork();
            struct command * current_command = command_list[i];
            // pid_list

            //child case
            if (pid == 0){
                if(current_command->input_file){
                    int input_descriptor = open(current_command->input_file, O_RDONLY);
                    if (input_descriptor == -1){
                        fprintf(stderr, open_error, current_command->input_file, strerror(errno));
                        exit(1);
                    }
                    check2 = dup2(input_descriptor, 0);
                    if (check2 != -1){
                        close(input_descriptor);
                    }
                    else{
                        perror("Error: dup2");
                        exit(1);
                    }
                }
                if (current_command->output_file){            
                    if (current_command->append){
                        int output_descriptor=open(current_command->output_file,O_WRONLY|O_CREAT|O_APPEND, 
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                        if (output_descriptor == -1){
                            fprintf(stderr, open_error, current_command->output_file, strerror(errno));
                            exit(1);
                        }
                        check2 = dup2(output_descriptor, 1);
                        if (check2 != -1){
                            close(output_descriptor);
                        }
                        else{
                            perror("Error: dup2");
                            exit(1);
                        }
                    }
                    else{
                        int output_descriptor=open(current_command->output_file, O_CREAT|O_WRONLY,
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                        if (output_descriptor == -1){
                            fprintf(stderr, open_error, current_command->output_file, strerror(errno));
                            exit(1);
                        }
                        check2 = dup2(output_descriptor, 1);
                        if (check2 != -1){
                            close(output_descriptor);
                        }
                        else{
                            perror("Error: dup2");
                            exit(1);
                        }
                    }
                }


                if(current_command->pipeLeft){
                    check2 = dup2(current_command->pipeLeft, 1);
                    if (check2!= -1){
                        close(current_command->pipeLeft);
                    }
                    else{
                        perror("Error: dup2");
                        exit(1);
                    }
                }
                if(current_command->pipeRight){
                    check2 = dup2(current_command->pipeRight, 0);
                    if (check2!= -1){
                        close(current_command->pipeRight);
                    }
                    else{
                        perror("Error: dup2");
                        exit(1);
                    }
                }
            
                execvp(current_command->subcommands[0], current_command->subcommands);
                exit(1);
            }
            //parent case
            else if(pid>0){
                if (current_command->pipeRight){
                    close(current_command->pipeRight);
                }
                if (current_command->pipeLeft){
                    close(current_command->pipeLeft);
                }

            }
            else{
                perror("Error: fork\n");
                exit(1);
            }
        }
        if (background_check == 0){
            for (int i =0; command_list[i];i++){
                wait(& status);
            }
        }
    }
}

