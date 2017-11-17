#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <wait.h>

#include "commands.h"
#include "signal_handlers.h"
#include "built_in.h"
#include "utils.h"

int main()
{
  char buf[8096];

  while (1) {
    fgets(buf, 8096, stdin);

    catch_sigint(SIGINT);
    catch_sigtstp(SIGTSTP);

    struct single_command commands[512];  //commmands->argv
    int n_commands = 0;  //=argc
    int ret;

    mysh_parse_command(buf, &n_commands, &commands);

//argv[0], [1] parsing OK

    if(commands->argc == 1) {//if user do not want to bg prcs

      ret = evaluate_command(n_commands, &commands);

      free_commands(n_commands, &commands);
     
      n_commands = 0;
    } else if (strcmp(commands->argv[1], "&") == 0) {  //if user want to bg processing

    int buflen = strlen(buf);
    char* bg_buf = malloc(sizeof(char)*(buflen-1));
      strcpy(bg_buf, commands->argv[0]);
      strcat(bg_buf, "\0");
//printf("bg cmd = %s\n", bg_buf);OK
      char* argu[] = {bg_buf, 0};

 //in bg processing
	int PID;
      	PID = fork();
	if(PID == 0) {  //if child process

	  int bg_pid = fork();
          if(bg_pid == 0) { //real bg process
	    printf("%d\n", getpid());
	    execv(argu[0], argu);
          } else {  //real bg process ends, print pid and ohters
  	    wait(NULL);
	    printf("%d done %s\n", bg_pid, argu[0]);

	    free_commands(n_commands, &commands);
	    n_commands = 0;
          }
	exit(0);

      } else {  //parent don't wait

      }

    free(bg_buf);
    }

    if (ret == 1) {

      break;
    }
  }

  return 0;
}
