#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>

#define FILE_OPEN_ERROR 1
#define READ_ERROR 2
#define PRINT_ERROR 3
#define ARGUMENTS_ERROR 4
#define PM_ERROR 5
#define PARSE_ERROR 6

#define FILENAME_LENGTH 256
#define LINE_LENGTH 256
#define HEADING "\033[31;1;4m"
#define NORMAL "\033[0m"

typedef struct {
	long low;
	long high;
	char title[LINE_LENGTH];
} maps_entry_t;

typedef struct {
	uint64_t pfn : 55;
	unsigned int soft_dirty : 1;
	unsigned int file_page : 1;
	unsigned int swapped : 1;
	unsigned int present : 1;
} pagemap_t;

int print_heading(char *title, int color, FILE *file)
{
	if (color)
		return fprintf(file, HEADING"%s\n"NORMAL, title);
	else
		return fprintf(file, "===%s===\n", title);
}

int print_environ(char *filename, int color, FILE *output)
{
	char buf[LINE_LENGTH];
	int len, i;
	FILE *f;


	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	f = fopen(filename, "r");
	while ((len = fread(buf, 1, LINE_LENGTH, f)) > 0)
	{
		for (i = 0; i < len; i++)
			if (buf[i] == 0)
				buf[i] = 10;
		buf[len]= 0;
		fprintf(output, "%s", buf);
	}
	fclose(f);
	return 0;
}


int print_file_new_line(char *filename, int color, FILE *output)
{
	char line[LINE_LENGTH];
	FILE *infile;

	if ((infile = fopen(filename, "r")) == NULL)
		return FILE_OPEN_ERROR;
	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	while (fgets(line, sizeof(line), infile)) 
		if (fprintf(output, "%s\n", line) < 0)
		{
			fclose(infile);
			return PRINT_ERROR;
		}

	fclose(infile);

	return 0;
}

int print_file(char *filename, int color, FILE *output)
{
	char line[LINE_LENGTH];
	FILE *infile;

	if ((infile = fopen(filename, "r")) == NULL)
		return FILE_OPEN_ERROR;
	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	while (fgets(line, sizeof(line), infile)) 
		if (fprintf(output, "%s", line) < 0)
		{
			fclose(infile);
			return PRINT_ERROR;
		}

	fclose(infile);

	return 0;
}

int print_mapsfile(char *filename, int color, FILE *output)
{
	char line[LINE_LENGTH];
	FILE *infile;
	long long unsigned totalSize = 0;

	if ((infile = fopen(filename, "r")) == NULL)
		return FILE_OPEN_ERROR;
	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	long long sum = 0;
	char line_current[LINE_LENGTH] = "";
	char type[LINE_LENGTH] = "a";
	while (fgets(line, sizeof(line), infile)) 
	{
		long long low, high;
		sscanf(line, "%llx-%llx", &low, &high);
		/* fprintf(output, "\tsize = %llu\n", high - low); */
		if (sscanf(line, "%*s %*s %*s %*s %*d %s", type) != 1)
			*type = 0;

		if (strlen(line_current) == 1)
			strcpy(line_current, type);
		/* printf("%s====%s\n", line_current, type); */
		if (strcmp(line_current, type) != 0 || strlen(line_current) != strlen(type))
		{
			if (sum > 0)
				fprintf(output, "	%s size = %llu; pages = %llu\n", line_current, sum, sum / 4092);
			sum = 0;
			strcpy(line_current, type);
		}
		if (fprintf(output, "%s", line) < 0)
		{
			fclose(infile);
			return PRINT_ERROR;
		}
		sum += high - low;
		totalSize += high - low;
	}
	if (sum > 0)
		fprintf(output, "	%s size = %llu; pages = %llu\n", line_current, sum, sum / 4092);

	fprintf(output, "	total_size = %llu; pages = %llu\n", totalSize, totalSize / 4092);
	fclose(infile);

	return 0;
}

int print_link(char *filename, int color, FILE *output)
{
	char line[LINE_LENGTH];

	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	if (readlink(filename, line, sizeof(line)) == -1)
		return READ_ERROR;

	fprintf(output, "Points to %s", line);

	return 0;
}

int print_dir_links(char *filename, int color, FILE *output)
{
	struct dirent *ptr;
	DIR *dir;
	char line_1[LINE_LENGTH];
	char line_2[LINE_LENGTH];
	
	if ((dir = opendir(filename)) == NULL)
		return FILE_OPEN_ERROR;


	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	while ((ptr = readdir (dir)) != NULL)
	{
		strcpy(line_1, filename);
		strcat(line_1, "/");
		strcat(line_1, ptr->d_name);

		if (readlink(line_1, line_2, sizeof(line_2)) == -1)
			continue;

		fprintf(output, "%s -> %s\n", line_1, line_2);
	}
	closedir(dir);

	return 0;
}

int parse_stat(char *filename, int color, FILE *output)
{
	FILE *infile;

	if ((infile = fopen(filename, "r")) == NULL)
		return FILE_OPEN_ERROR;
	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	pid_t pid;
	fscanf(infile, "%d", &pid);
	fprintf(output, "pid: %d\n", pid);
	char comm[LINE_LENGTH];
	fscanf(infile, "%s", comm);
	fprintf(output, "comm: %s\n", comm);
	char state;
	fscanf(infile, "%c", &state);
	fscanf(infile, "%c", &state);
	fprintf(output, "state: %c\n", state);
	int ppid;
	fscanf(infile, "%d", &ppid);
	fprintf(output, "ppid: %d\n", ppid);
	int pgrp;
	fscanf(infile, "%d", &pgrp);
	fprintf(output, "pgrp: %d\n", pgrp);
	int session;
	fscanf(infile, "%d", &session);
	fprintf(output, "session: %d\n", session);
	int tty_nr;
	fscanf(infile, "%d", &tty_nr);
	fprintf(output, "tty_nr: %d\n", tty_nr);
	int tpgid;
	fscanf(infile, "%d", &tpgid);
	fprintf(output, "tpgid: %d\n", tpgid);
	unsigned flags;
	fscanf(infile, "%u", &flags);
	fprintf(output, "flags: %u\n", flags);
	long unsigned minflt;
	fscanf(infile, "%lu", &minflt);
	fprintf(output, "minflt: %lu\n", minflt);
	long unsigned cminflt;
	fscanf(infile, "%lu", &cminflt);
	fprintf(output, "cminflt: %lu\n", cminflt);
	long unsigned majflt;
	fscanf(infile, "%lu", &majflt);
	fprintf(output, "majflt: %lu\n", majflt);
	long unsigned cmajflt;
	fscanf(infile, "%lu", &cmajflt);
	fprintf(output, "cmajflt: %lu\n", cmajflt);
	long unsigned utime;
	fscanf(infile, "%lu", &utime);
	fprintf(output, "utime: %lu\n", utime);
	long unsigned stime;
	fscanf(infile, "%lu", &stime);
	fprintf(output, "stime: %lu\n", stime);
	long cutime;
	fscanf(infile, "%ld", &cutime);
	fprintf(output, "cutime: %ld\n", cutime);
	long cstime;
	fscanf(infile, "%ld", &cstime);
	fprintf(output, "cstime: %ld\n", cstime);
	long priority;
	fscanf(infile, "%ld", &priority);
	fprintf(output, "priority: %ld\n", priority);
	long nice;
	fscanf(infile, "%ld", &nice);
	fprintf(output, "nice: %ld\n", nice);
	long num_threads;
	fscanf(infile, "%ld", &num_threads);
	fprintf(output, "num_threads: %ld\n", num_threads);
	long itrealvalue;
	fscanf(infile, "%ld", &itrealvalue);
	fprintf(output, "itrealvalue: %ld\n", itrealvalue);
	long long unsigned starttime;
	fscanf(infile, "%llu", &starttime);
	fprintf(output, "starttime: %llu\n", starttime);
	long unsigned vsize;
	fscanf(infile, "%lu", &vsize);
	fprintf(output, "vsize: %lu\n", vsize);
	long unsigned rss;
	fscanf(infile, "%ld", &rss);
	fprintf(output, "rss: %ld\n", rss);
	long unsigned rsslim;
	fscanf(infile, "%lu", &rsslim);
	fprintf(output, "rsslim: %lu\n", rsslim);
	long unsigned startcode;
	fscanf(infile, "%lu", &startcode);
	fprintf(output, "startcode: %lu\n", startcode);
	long unsigned endcode;
	fscanf(infile, "%lu", &endcode);
	fprintf(output, "endcode: %lu\n", endcode);
	fprintf(output, "	codesize: %lu\n", endcode - startcode);
	long unsigned startstack;
	fscanf(infile, "%lu", &startstack);
	fprintf(output, "startstack: %lu\n", startstack);
	long unsigned kstkesp;
	fscanf(infile, "%lu", &kstkesp);
	fprintf(output, "kstkesp: %lu\n", kstkesp);
	long unsigned kstkeip;
	fscanf(infile, "%lu", &kstkeip);
	fprintf(output, "kstkeip: %lu\n", kstkeip);
	long unsigned signal;
	fscanf(infile, "%lu", &signal);
	fprintf(output, "signal: %lu\n", signal);
	long unsigned blocked;
	fscanf(infile, "%lu", &blocked);
	fprintf(output, "blocked: %lu\n", blocked);
	long unsigned sigignore;
	fscanf(infile, "%lu", &sigignore);
	fprintf(output, "sigignore: %lu\n", sigignore);
	long unsigned sigcatch;
	fscanf(infile, "%lu", &sigcatch);
	fprintf(output, "sigcatch: %lu\n", sigcatch);
	long unsigned wchan;
	fscanf(infile, "%lu", &wchan);
	fprintf(output, "wchan: %lu\n", wchan);
	long unsigned nswap;
	fscanf(infile, "%lu", &nswap);
	fprintf(output, "nswap: %lu\n", nswap);
	long unsigned cnswap;
	fscanf(infile, "%lu", &cnswap);
	fprintf(output, "cnswap: %lu\n", cnswap);
	int exit_signal;
	fscanf(infile, "%d", &exit_signal);
	fprintf(output, "exit_signal: %d\n", exit_signal);
	int processor;
	fscanf(infile, "%d", &processor);
	fprintf(output, "processor: %d\n", processor);
	unsigned rt_priority;
	fscanf(infile, "%u", &rt_priority);
	fprintf(output, "rt_priority: %u\n", rt_priority);
	unsigned policy;
	fscanf(infile, "%u", &policy);
	fprintf(output, "policy: %u\n", policy);
	long long unsigned delayacct_blkio_ticks;
	fscanf(infile, "%llu", &delayacct_blkio_ticks);
	fprintf(output, "delayacct_blkio_ticks: %llu\n", delayacct_blkio_ticks);
	long unsigned guest_time;
	fscanf(infile, "%lu", &guest_time);
	fprintf(output, "guest_time: %lu\n", guest_time);
	long cguest_time;
	fscanf(infile, "%ld", &cguest_time);
	fprintf(output, "cguest_time: %ld\n", cguest_time);
	long unsigned start_data;
	fscanf(infile, "%lu", &start_data);
	fprintf(output, "start_data: %lu\n", start_data);
	long unsigned end_data;
	fscanf(infile, "%lu", &end_data);
	fprintf(output, "end_data: %lu\n", end_data);
	fprintf(output, "	data_size: %lu\n", end_data - start_data);
	long unsigned start_brk;
	fscanf(infile, "%lu", &start_brk);
	fprintf(output, "start_brk: %lu\n", start_brk);
	long unsigned arg_start;
	fscanf(infile, "%lu", &arg_start);
	fprintf(output, "arg_start: %lu\n", arg_start);
	long unsigned arg_end;
	fscanf(infile, "%lu", &arg_end);
	fprintf(output, "arg_end: %lu\n", arg_end);
	fprintf(output, "	arg_size: %lu\n", arg_end - arg_start);
	long unsigned env_start;
	fscanf(infile, "%lu", &env_start);
	fprintf(output, "env_start: %lu\n", env_start);
	long unsigned env_end;
	fscanf(infile, "%lu", &env_end);
	fprintf(output, "env_end: %lu\n", env_end);
	fprintf(output, "	enf_size: %lu\n", env_end - env_start);
	int exit_code;
	fscanf(infile, "%d", &exit_code);
	fprintf(output, "exit_code: %d\n", exit_code);

	fclose(infile);

	return 0;
}

int print_task(char *filename, int color, FILE *output)
{
	struct dirent *ptr;
	DIR *dir;
	char line_1[LINE_LENGTH];
	char line_2[LINE_LENGTH];
	
	if ((dir = opendir(filename)) == NULL)
		return FILE_OPEN_ERROR;


	if (print_heading(filename, color, output) < 0)
		return PRINT_ERROR;

	while ((ptr = readdir (dir)) != NULL)
	{
		if (*ptr->d_name == '.') continue;

		strcpy(line_1, filename);
		strcat(line_1, "/");
		strcat(line_1, ptr->d_name);

		strcpy(line_2, "/proc/");
		strcat(line_2, ptr->d_name);

		fprintf(output, "%s -> %s\n", line_1, line_2);
	}
	closedir(dir);

	return 0;
}


int parse_maps_line(char *line, maps_entry_t *result)
{
	int counter = 0;

	if (sscanf(line, "%lx-%lx", &(result->low), &(result->high)) < 0)
		return PARSE_ERROR;

	while (*line != 0 && counter < 5)
		if (isspace(*line)) 
		{
			counter++;
			while (isspace(*line)) line++;
		}
		else
			line++;

	strncpy(result->title, line, sizeof(result->title));

	return 0;
}

int pagemap_get_entry(pagemap_t *entry, int pagemap_fd, uintptr_t vaddr)
{
	size_t nread = 0;
	ssize_t ret;
	uint64_t data;

	while (nread < sizeof(data)) {
		ret = pread(pagemap_fd, ((uint8_t*)&data) + nread, sizeof(data) - nread,
				(vaddr / sysconf(_SC_PAGE_SIZE)) * sizeof(data) + nread);
		nread += ret;
		if (ret <= 0)
			return 1;
	}

	entry->pfn = data & (((uint64_t)1 << 55) - 1);
	entry->soft_dirty = (data >> 55) & 1;
	entry->file_page = (data >> 61) & 1;
	entry->swapped = (data >> 62) & 1;
	entry->present = (data >> 63) & 1;

	return 0;
}

int print_pagemap_entry(maps_entry_t m_entry, int pagemap_fd, FILE *outfile)
{
	pagemap_t pm_entry;

	for (uintptr_t addr = m_entry.low; addr < m_entry.high; addr += sysconf(_SC_PAGE_SIZE))
		if (!pagemap_get_entry(&pm_entry, pagemap_fd, addr)) 
			fprintf(outfile,
					"%.12jx\t%.7jx\t%u\t\t\t%u\t\t\t%u\t\t%u\t\t%s\n",
					(uintmax_t)addr,
					(uintmax_t)pm_entry.pfn,
					pm_entry.soft_dirty,
					pm_entry.file_page,
					pm_entry.swapped,
					pm_entry.present,
					m_entry.title
			);

	return 0;
}

int print_pagemap(pid_t pid, int color, FILE *outfile)
{
	char maps_filename[FILENAME_LENGTH];
	char pagemap_filename[FILENAME_LENGTH];
	char line[LINE_LENGTH];
	char *pch;
	FILE *maps_file;
	int pagemap_fd;
	maps_entry_t maps_entry;
	int rc = 0;

	snprintf(maps_filename, sizeof(maps_filename), "/proc/%d/maps", pid);
	snprintf(pagemap_filename, sizeof(pagemap_filename), "/proc/%d/pagemap", pid);

	if ((maps_file = fopen(maps_filename, "r")) == NULL)
		return FILE_OPEN_ERROR;

	if ((pagemap_fd = open(pagemap_filename, 'r')) < 0)
	{
		fclose(maps_file);
		return FILE_OPEN_ERROR;
	}

	print_heading("pagemap", color, outfile);
	fprintf(outfile, "addr\t\t\tpfn\t\tsoft-dirty\tfile/shared\tswapped\tpresent\tlibrary\n");

	while (fgets(line, sizeof(line), maps_file)) 
	{
		if ((pch = strstr(line, "\n")) != NULL)
			strncpy(pch, "\0", 1);

		if ((rc = parse_maps_line(line, &maps_entry)) != 0)
			break;
		if ((rc = print_pagemap_entry(maps_entry, pagemap_fd, outfile)) != 0)
			break;
	}

	fclose(maps_file);
	close(pagemap_fd);

	return rc;
}

int read_proc(pid_t pid, FILE *outfile, int color)
{
	char filename[FILENAME_LENGTH];
	int rc;

	snprintf(filename, sizeof(filename), "/proc/%d/environ", pid);
	if ((rc = print_environ(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n\n");

	snprintf(filename, sizeof(filename), "/proc/%d/cmdline", pid);
	if ((rc = print_file(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n\n");

	snprintf(filename, sizeof(filename), "/proc/%d/cwd", pid);
	if ((rc = print_link(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n\n");

	snprintf(filename, sizeof(filename), "/proc/%d/exe", pid);
	if ((rc = print_link(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n\n");

	snprintf(filename, sizeof(filename), "/proc/%d/root", pid);
	if ((rc = print_link(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n\n");

	snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);
	if ((rc = parse_stat(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n");

	snprintf(filename, sizeof(filename), "/proc/%d/fd", pid);
	if ((rc = print_dir_links(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n");

	snprintf(filename, sizeof(filename), "/proc/%d/task", pid);
	if ((rc = print_task(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n");

	snprintf(filename, sizeof(filename), "/proc/%d/maps", pid);
	if ((rc = print_mapsfile(filename, color, outfile)) != 0)
		return rc;
	fprintf(outfile, "\n");

	rc = print_pagemap(pid, color, outfile);

	return rc;
}

int main(int argc, char *argv[]) 
{

	FILE *outfile = stdout;
	int color = 1;
	pid_t pid;
	int rc;

	switch (argc) {
		case 2:
			if (sscanf(argv[1], "%d", &pid) < 0)
				return ARGUMENTS_ERROR;
			break;
		case 3:
			color = 0;
			if (sscanf(argv[2], "%d", &pid) < 0)
			{
				printf("Usage: %s [<file>] <pid>\n", argv[0]);
				return ARGUMENTS_ERROR;
			}
			if ((outfile = fopen(argv[1], "w")) < 0)
			{
				printf("Failed to open file\n");
				return FILE_OPEN_ERROR;
			}
			break;
		default:
			printf("Usage: %s [<file>] <pid>\n", argv[0]);
			return ARGUMENTS_ERROR;
	}

	if ((rc = read_proc(pid, outfile, color)) != 0)
		printf("Error occured!\n");
	fclose(outfile);

	return rc;
}
