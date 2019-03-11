#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define FILESIZ 10000

int main(int argc, char *argv[])
{
	//FILE *fp = fopen("textfile.txt","r");
	FILE* fp = fopen(argv[1], "r");

	char buffer[50];

	while(fgets(buffer,sizeof buffer,fp) != NULL)
	{
		printf("%s", buffer);

		//print every second word......
		char* token = strtok(buffer, " ");
		int i = 1;
		printf("Printing every 2nd word...\n");
		while(token != NULL)
		{
			if (i % 2 == 0)
			{
				printf("%s ", token);
			}
			i++;
			token = strtok(NULL, " ");
		}
	}

	fclose(fp);

}
