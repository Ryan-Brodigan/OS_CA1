#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ADDRESSABLE_BYTES 65536
#define ADDRESS_SIZE_BYTES 2

//Function declarations
void translateAddress(unsigned short virtual_address, unsigned short *physical_memory);
void writePageTableToPhysicalMemory(unsigned short *physical_memory);
void dumpPhysicalMemoryToDisk(unsigned short *physical_memory);
void dumpPageTableToDisk(unsigned short *physical_memory);
unsigned short compilePFNWithOffset(unsigned short pfn, unsigned short offset);
unsigned short calculateAddressOffset(unsigned short address);
void randomWriteToPhysicalMemory(unsigned short *physical_memory);
unsigned short randomShortInRange(unsigned short min, unsigned short max);

//Main method, entry point
void simulate(){

	//Seed our rand() function using the current time, otherwise we will generate the same values each time
	srand((unsigned int) time(0));

	// Allocate a chunk of memory that represents the physical memory of our system
	unsigned short *physical_memory = malloc(MAX_ADDRESSABLE_BYTES);

	//Steps that are run on application startup
	writePageTableToPhysicalMemory(physical_memory);
	randomWriteToPhysicalMemory(physical_memory);
	dumpPhysicalMemoryToDisk(physical_memory);
	dumpPageTableToDisk(physical_memory);

	//TODO LOOP THAT ALLOWS USER TO CHECK CONTENTS OF VIRTUAL ADDRESS

	//When we are finished, clean up
	free(physical_memory);
}

//Takes a virtual address and the physical memory address space and
//translates the virtual address to it's corresponding physical address
//printing it's contents in the process
void translateAddress(unsigned short virtual_address, unsigned short *physical_memory){
	//Step 1: Split virtual address into VPN + Offset
	unsigned short vpn = virtual_address >> 8;
	unsigned short offset = calculateAddressOffset(virtual_address);

	printf("VPN: %d, Offset: %d\n", vpn, offset);

	//Step 2: Translate the VPN to a Physical Frame Number, using our Page Table
	//Use the entry in our page table to do error checking
	unsigned short pfn = physical_memory[vpn];

	//Step 3: Add Offset to PFN, print the contents of the resulting address
	unsigned short physical_address = compilePFNWithOffset(pfn, offset);

	printf("Character at physical address: %C\n", physical_memory[physical_address]);
}

//Writes the contents of the Page Table for our single process to memory
void writePageTableToPhysicalMemory(unsigned short *physical_memory){
	//First 512 bytes (2 Frames) of our Physical Memory are reserved by the Page Table for our 'process'
	//Given this, virtual address 0x0000 will actually map to the first available address in physical memory,
	//which will be at the start of frame 2
	for(int i = 0; i < 256; i++){
		physical_memory[i*2] = (i+2);
		//TODO Generate Page Table Entry
		physical_memory[(i*2)+1] = 0x00000000;
	}
}

//Writes the contents of our physical memory out to a file
void dumpPhysicalMemoryToDisk(unsigned short *physical_memory){
	FILE * physical_memory_dump_file;
	//TODO MAKE THIS RELATIVE
	physical_memory_dump_file = fopen("/home/ryanb/CA1/OS_CA1/data/physical_memory_dump.txt", "w");

	if(physical_memory_dump_file == NULL){
		printf("ERROR LOADING PHYSICAL MEMORY DUMP FILE\nExiting...\n");
		exit(1);
	}

	fprintf(physical_memory_dump_file, "ADDRESS   | FRAME   | CONTENT\n");
	fprintf(physical_memory_dump_file, "-------------------------------\n");

	for(int i = 0; i < MAX_ADDRESSABLE_BYTES; i++){
		fprintf(physical_memory_dump_file, "0x%X   | %d   | %C\n", i, i >> 8, physical_memory[i]);
	}

	fclose(physical_memory_dump_file);
}

//Dumps the content of our "process'" Page Table
void dumpPageTableToDisk(unsigned short *physical_memory){
	FILE * page_table_dump_file;
	//TODO MAKE THIS RELATIVE
	page_table_dump_file = fopen("/home/ryanb/CA1/OS_CA1/data/page_table_dump.txt", "w");

	if(page_table_dump_file == NULL){
		printf("ERROR LOADING PAGE TABLE DUMP FILE\nExiting...\n");
		exit(1);
	}

	fprintf(page_table_dump_file, "		VPN		|		PFN		|	Page Table Entry\n");
	fprintf(page_table_dump_file, "-----------------------------------------\n");

	for(int i = 0 ; i < 256; i++){
		fprintf(page_table_dump_file, "		%d	|		%d		|	0x%X\n", i, physical_memory[i*2], physical_memory[(i*2)+1]);
	}

	fclose(page_table_dump_file);
}

//Calculates the address offset for a given address
unsigned short calculateAddressOffset(unsigned short address){
	unsigned short offset_mask = 0x00FF;
	return (address & offset_mask);
}

//Put PFN and address offset back together
unsigned short compilePFNWithOffset(unsigned short pfn, unsigned short offset){
	pfn <<= 8;
	return (pfn | offset);
}

//Initialize the physical memory, write random amount of bytes to random addresses
void randomWriteToPhysicalMemory(unsigned short *physical_memory){

	//Generate random number between 2048-20480, the bytes of data we will write to our memory
	unsigned short randomBytes = randomShortInRange(2048, 20480);

	//Choose a random

	//Randomly write the aforementioned amount of data to our memory, using a loop
	//We will randomly write characters, which are 1 byte each, so loop for the amount of
	//bytes we randomly generated
	for(int i = 512; i < (randomBytes + 512); i++){
		//Randomly generate an ASCII character code (1 Byte) to write to the aforementioned address
		unsigned short randomCharCode = randomShortInRange(33, 126);

		//Assign the character to our physical memory address
		physical_memory[i] = randomCharCode;
	}
}

//Returns a random unsigned short between the specified min and max values
unsigned short randomShortInRange(unsigned short min, unsigned short max){
	return ((rand() % (max - min)) + min);
}
