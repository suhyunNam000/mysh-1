#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

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

    mysh_parse_command(buf, &n_commands, &commands);

//if commands->argv[argc] == '&' -> that command is background
    int bg = 0;
    int buflen = strlen(buf);
    char* bg_buf;

    if(!strcmp(commands->argv[n_commands-1],"&")){
printf("command want to become bg : %s\n", commands->argv[n_commands-1]);
        bg = 1;
        buf[buflen-1] = NULL;
        memset(bg_buf, 0, 1024);
	strcpy(bg_buf, buf);
      }
printf("bg cmd is %s\n", bg_buf);

    if(bg == 1) {

      int PID;
      PID = fork();

      if(PID == 0) {  //if child process

	char* argu[] = {bg_buf,0};

	execv(argu[0], argu);

	fprintf(stdout, "%d done %s\n",getpid(), argu[0]);
	n_commands = 0;
	exit(0);

      } else if (PID < 0) {
	  printf("bg : fork failed.\n");

      } else {  //parent don't wait
	  fprintf(stdout, "%d\n", PID);
	  free_commands(n_commands, &commands);
	  n_commands = 0;
      }
    }

    int ret = evaluate_command(n_commands, &commands);

//    free_commands(n_commands, &commands);
//    n_commands = 0;

    free_commands(n_commands, &commands);
    n_commands = 0;

    if (ret == 1) {
      break;
    }
  }

  return 0;
}
