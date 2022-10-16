main:
	gcc -g main.c prompt.c command.c cd.c history.c runner.c pinfo.c ls.c discover.c jobs.c bg.c fg.c -o fmlsh
	
clean:
