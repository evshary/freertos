#include "fio.h"

#define MAX_COMMAND_LEN 30
#define BACKSPACE 0x7f

extern char receive_byte();

enum {
	PS = 0,
	HELP,
	HELLO,
	SYSTEM,
	MAX_COMMANDS
};

typedef struct{
	char* name;
	void (*function)(void);
} shell_cmd;

int strlen(char *string){
	int len = 0;
	char *current = string;
	while(current[len] != '\0'){
		len++;
	}
	return len;
}

int strcmp(char *str1, char *str2){
	char *cur1 = str1;
	char *cur2 = str2;
	int i = 0;
	while(cur1[i] == cur2[i]){
		if(cur1[i] == '\0'){
			return 0;
		}
		i++;
	}
	return -1;
}

void print(char * print_str){
	fio_write(1, print_str, strlen(print_str));
	if(print_str[strlen(print_str)-1] == '\n')
	{
		fio_write(1, "\r", 1);
	}
}

void read_string(char *command){
	char ch[2] = {0};
	int curr_char = 0;
	int done = 0;
	
	do{
		ch[0] = receive_byte();
		if(curr_char >= MAX_COMMAND_LEN || (ch[0] == '\r') || (ch[0] == '\n')){
			command[curr_char] = '\0';
			print("\n");
			done = 1;
		}else if(ch[0] == BACKSPACE){
			curr_char--;
			print("\b \b\0");
		}else{
			command[curr_char++] = ch[0];
			print(ch);
		}
	}while(!done);

}

void help_func(){
	print("What can I help you?\n");
}

void hello_func(){
	print("Hello World\n");
}

void ps_func(){
	print("ps:\n");
}

void system_func(){
	char command[MAX_COMMAND_LEN];
	print("Please enter your command:");
	read_string(command);
	//union sys_param func_param[2] = {
	//	{.pCHAR = command},
	//	{.pINT = strlen(command)}
	//};
	//host_call(0x12, func_param);
}

shell_cmd commands[] = {
	{
		.name = "ps",
		.function = ps_func
	},
	{
		.name = "help",
		.function = help_func
	},
	{
		.name = "hello",
		.function = hello_func
	},
	{
		.name = "system",
		.function = system_func
	}
};

void user_shell(){
	char command[MAX_COMMAND_LEN];
	int i;
	while(1){
		print("evshary->");
		read_string(command);

		for(i = 0; i < MAX_COMMANDS; i++){
			if(!strcmp(commands[i].name, command)){
				commands[i].function();
				break;
			}
		}
	}
}
