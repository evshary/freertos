#include "fio.h"
#include "string-util.h"
#include "mmtest.h"
#include "FreeRTOS.h"
#include "task.h"

#define MAX_COMMAND_LEN 30
#define BACKSPACE 0x7f
#define BUF_SIZE 50

void hello_func();
void help_func();
void ps_func();
void system_func();
void mmtest_func();

extern char receive_byte();

enum {
	PS = 0,
	HELP,
	HELLO,
	SYSTEM,
	MMTEST,
	MAX_COMMANDS
};

typedef struct{
	char *name;
	char *description;
	void (*function)(void);
} shell_cmd;

shell_cmd commands[] = {
	{
		.name = "ps",
		.description = "show the process now",
		.function = ps_func
	},
	{
		.name = "help",
		.description = "show the introduction to the commands",
		.function = help_func
	},
	{
		.name = "hello",
		.description = "saying hello",
		.function = hello_func
	},
	{
		.name = "system",
		.description = "execute the host command",
		.function = system_func
	},
	{
		.name = "mmtest",
		.description = "test memory allocation",
		.function = mmtest_func
	}
};

void read_string(char *command){
	char ch[2] = {0};
	int curr_char = 0;
	int done = 0;
	
	do{
		ch[0] = receive_byte();
		if(curr_char >= MAX_COMMAND_LEN || (ch[0] == '\r') || (ch[0] == '\n')){
			command[curr_char] = '\0';
			printf("\n");
			done = 1;
		}else if(ch[0] == BACKSPACE){
			curr_char--;
			printf("\b \b\0");
		}else{
			command[curr_char++] = ch[0];
			printf("%c", ch[0]);
		}
	}while(!done);

}

void help_func(){
	int i;
	printf("This shell supports the commands following:\n");
	for(i = 0; i < MAX_COMMANDS; i++){
		printf("%s: %s\n", commands[i].name, commands[i].description);
	}
}

void hello_func(){
	printf("Hello World\n");
}

void ps_func(){
	char buf[BUF_SIZE];
	vTaskList(buf);
	printf("%s\n", buf);
}

void system_func(){
	char command[MAX_COMMAND_LEN];
	printf("Please enter your command:");
	read_string(command);
}

void mmtest_func(){
	mmtest();
}

void user_shell(){
	char command[MAX_COMMAND_LEN];
	int i;
	while(1){
		printf("evshary->");
		read_string(command);

		for(i = 0; i < MAX_COMMANDS; i++){
			if(!strcmp(commands[i].name, command)){
				commands[i].function();
				break;
			}
		}
	}
}
