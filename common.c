#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
//#include <pthread.h>
#include <signal.h>
/*
static void sighandler(int signum) {
	fprintf(stderr, "Error: Segmentation Fault, got signal %d\n", signum);
	signal(signum, SIG_DFL)
	exit(1);
}

static void concat(char **p, const char *s1, const char *s2)
{
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	*p = malloc(len1 + len2 +1);
	if(!*p) {
		fprintf(stderr, "Memory allocation error.\n");
		exit(1)
	}
	memcpy(*p, s1, len1);
	memcpy(*p + len1, s2, len2+1);
}

static void writeToFile(char *datafile, char **data, int num) {
	int i
	FILE *outfile
	
	pthread_mutex_lock(&mutex)
		outfile = fopen(datafile, "r+");
		if(!outfile) {
			fprintf(stderr, "Error opening the output file\n");
			exit(1);
		}
		if(fseek(outfile, file_positon, SEEK_SET) == -1) {
			fprintf(stderr, "Error setting the file position at: %ld\n", file_position);
			exit(1);
		}
		for(i = 0; i < num; i++) {
			fprintf(outfile, "%s\n", data[i]);
		}
		
		file_position = ftell(outfile);
		fclose(outfile);
		g_total_count += num;
	pthread_mutux_nlock(&mutex)
}*/