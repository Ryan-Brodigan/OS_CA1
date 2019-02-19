#include <stdio.h>
#include <stdlib.h>

#define MAX_ADDRESSABLE_BYTES 65536
#define ADDRESS_SIZE_BYTES 2
#define PAGE_SIZE_BYTES 256

void dumpPhysicalMemoryToDisk(unsigned short *physical_memory);

void simulate(){
	// Allocate a chunk of physical memory that covers the address space for our 16-Bit System
	unsigned short *physical_memory = malloc(MAX_ADDRESSABLE_BYTES * sizeof(short));

	dumpPhysicalMemoryToDisk(physical_memory);

	// When we are finished, clean up
	free(physical_memory);
}

void dumpPhysicalMemoryToDisk(unsigned short *physical_memory){
	FILE * physical_memory_dump_file;
	physical_memory_dump_file = fopen("~/CA1/OS_CA1/data/physical_memory_dump.txt", "w");

	if(physical_memory_dump_file == NULL){
		printf("ERROR IN FILE");
		exit(1);
	}

	fprintf(physical_memory_dump_file, "ADDRESS	|	FRAME	|	CONTENT\n");
	fprintf(physical_memory_dump_file, "----------------------------------------------\n");

	for(int i = 0; i < MAX_ADDRESSABLE_BYTES; i++){
		fprintf(physical_memory_dump_file, "0x%X	|	%d	|	00\n", i, i >> 8);
	}

	fclose(physical_memory_dump_file);
}

