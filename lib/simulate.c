#include <stdio.h>
#include <stdlib.h>

#define MAX_ADDRESSABLE_BYTES 65536
#define ADDRESS_SIZE_BYTES 2
#define PAGE_SIZE_BYTES 256

void dumpPhysicalMemoryToDisk(unsigned short *physical_memory);
unsigned short calculateAddressOffset(unsigned short address);

void simulate(){
	// Allocate a chunk of memory that represents the physical memory of our system
	unsigned short *physical_memory = malloc(MAX_ADDRESSABLE_BYTES * sizeof(short));
	//Allocate a chunk of memory that represents the virtual address space of our process
	unsigned short *virtual_memory = malloc(MAX_ADDRESSABLE_BYTES * sizeof(short));
	//Allocate a chunk of memory that represents the page table for our system
	unsigned short *page_table = malloc(PAGE_SIZE_BYTES * sizeof(short));

	unsigned short test = 257;

	//dumpPhysicalMemoryToDisk(physical_memory);
	unsigned short result = calculateAddressOffset(test);

	// When we are finished, clean up
	free(physical_memory);
	free(virtual_memory);
	free(page_table);
}

void dumpPhysicalMemoryToDisk(unsigned short *physical_memory){
	FILE * physical_memory_dump_file;
	physical_memory_dump_file = fopen("../data/physical_memory_dump.txt", "w");

	if(physical_memory_dump_file == NULL){
		printf("ERROR LOADING PHYSICAL MEMORY DUMP FILE\nExiting...\n");
		exit(1);
	}

	fprintf(physical_memory_dump_file, "	ADDRESS	|	FRAME	|	CONTENT\n");
	fprintf(physical_memory_dump_file, "-----------------------------\n");

	for(int i = 0; i < MAX_ADDRESSABLE_BYTES; i++){
		fprintf(physical_memory_dump_file, "	0x%X	|		%d	|	00\n", i, i >> 8);
	}

	fclose(physical_memory_dump_file);
}

unsigned short calculateAddressOffset(unsigned short address){
	unsigned short addressStart = (address >> 8) * PAGE_SIZE_BYTES;
	return (address - addressStart);
}
