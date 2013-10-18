#include "fio.h"

#define MAX_COMMAND_LEN 1024

extern void receive_byte(char *);

void read_string(char *command){
	char echo_ch[2] = {0};
	while(1){
		receive_byte(echo_ch);
		
	}
}

void user_shell(){
	char command[MAX_COMMAND_LEN];
	while(1){
		read_string(command);
		fio_write(1, "evshary->", 10);	
	}
}
