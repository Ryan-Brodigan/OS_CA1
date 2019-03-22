#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define MAX_ADDRESSABLE_BYTES 65536
#define ADDRESS_SIZE_BYTES 2
#define PAGE_AND_FRAME_SIZE_BYTES 256
#define PRESENT_BIT_BITMASK 0x0010
#define ACCESSED_BIT_BITMASK 0x0001

//Function declarations
void translateAddress(unsigned short virtual_address, unsigned short *physical_memory);
void writePageTableToPhysicalMemory(unsigned short *physical_memory, unsigned short bytesOfWrittenData);
void dumpPhysicalMemoryToDisk(unsigned short *physical_memory);
void dumpPageTableToDisk(unsigned short *physical_memory);
unsigned short compilePFNWithOffset(unsigned short pfn, unsigned short offset);
unsigned short calculateAddressOffset(unsigned short address);
void randomWriteToPhysicalMemory(unsigned short *physical_memory);
unsigned short randomShortInRange(unsigned short min, unsigned short max);
void randomPrintPageTableEntry(unsigned short *physical_memory);
void printPageTableEntry(unsigned short *physical_memory, unsigned short pageNumber);
void swapFrameIntoMemory(unsigned short* physical_memory, unsigned short pfn, unsigned short vpn);

//Main method, entry point
void simulate(){

	//Seed our rand() function using the current time, otherwise we will generate the same values each time
	srand((unsigned int) time(0));

	// Allocate a chunk of memory that represents the physical memory of our system
	unsigned short *physical_memory = malloc(MAX_ADDRESSABLE_BYTES);

	if(physical_memory == NULL){
		printf("ERROR ALLOCATING PHYSICAL MEMORY\nExiting...");
		exit(1);
	}

	printf("\nPRELIMINARY STEPS\n\n");

	//Steps that are run on application startup
	randomWriteToPhysicalMemory(physical_memory);
	dumpPhysicalMemoryToDisk(physical_memory);
	dumpPageTableToDisk(physical_memory);
	randomPrintPageTableEntry(physical_memory);

	//4 Possible outcomes to this loop:
	// 1. We are able to translate the VPN to a PFN and display the contents
	// 2. We need to swap the frame into physical_memory from a file
	// 3. The memory does not belong to our process
	// 4. Invalid entry, enter again
	bool notDone = true;

	printf("\nENTERING USER ENTRY LOOP\n\n");

	do{
		printf("Enter a 16-Bit Hexadecimal Address to display it's contents in Physical Memory (If possible): ");
		unsigned short userEnteredAddress;
		scanf("%hx", &userEnteredAddress);
		translateAddress(userEnteredAddress, physical_memory);
	}while(notDone);

	//When we are finished, clean up
	free(physical_memory);
}

//Takes a virtual address and the physical memory address space and
//translates the virtual address to it's corresponding physical address
//printing it's contents in the process
void translateAddress(unsigned short virtual_address, unsigned short *physical_memory){

	printf("User Entered Address: 0x%X\n", virtual_address);

	//Step 1: Split virtual address into VPN + Offset
	unsigned short vpn = virtual_address >> 8;
	unsigned short offset = calculateAddressOffset(virtual_address);

	printf("VPN: %d, Offset: %d\n", vpn, offset);

	//Step 2: Translate the VPN to a Physical Frame Number, using our Page Table
	//Use the entry in our page table to do error checking
	//SWAPPING WILL BE CAUSED HERE
	unsigned short pfn = physical_memory[vpn*2];
	unsigned short pageTableEntry = physical_memory[(vpn*2)+1];

	//Check for invalid PFN, indicating memory our process does not have access to
	if(pfn == 0){
		printf("Cannot print content's of physical address, as it does not belong to this process!\n\n");
	}
	else{
		printf("Corresponding PFN: %d\n", pfn);

		//SWAPPING LOGIC
		if((pageTableEntry >> 4) == 0){
			printf("Present Bit is set to 0, Swapping Frame into Physical Memory...\n");

			//Swap the missing Frame into memory
			swapFrameIntoMemory(physical_memory, pfn, vpn);

			//Update Page Table Entry var
			pageTableEntry = physical_memory[(vpn*2)+1];
		}

		//Step 3: Add Offset to PFN, print the contents of the resulting address
		unsigned short physical_address = compilePFNWithOffset(pfn, offset);

		printf("Translated Physical Address: 0x%X\n", physical_address);

		//If Accessed bit is set to 0, set to 1
		if((pageTableEntry & ACCESSED_BIT_BITMASK) == 0){
				printf("Setting Accessed Bit for Page Table Entry to 1...\n");
				physical_memory[(vpn*2)+1] = (pageTableEntry | ACCESSED_BIT_BITMASK);
				dumpPageTableToDisk(physical_memory);
		}

		printf("Character at physical address: %C\n\n", physical_memory[physical_address]);
	}
}

//Writes the contents of the Page Table for our single process to memory
void writePageTableToPhysicalMemory(unsigned short *physical_memory, unsigned short bytesOfWrittenData){

	printf("Writing Page Table to Physical Memory...\n");

	//Calculate how many frames our process is occupying and then generate the Page Table entries
	//Based on this information, write the appropriate entries to our Page Table
	unsigned short framesOccupied = floor(bytesOfWrittenData / PAGE_AND_FRAME_SIZE_BYTES);

	printf("Writing Page Table Entries for %d Frames (Excluding Swapping Examples)...\n", framesOccupied);

	//First 512 bytes (2 Frames) of our Physical Memory are reserved by the Page Table for our 'process'
	//Given this, virtual address 0x0000 will actually map to the first available address in physical memory,
	//which will be at the start of frame 2
	for(int i = 0; i < framesOccupied; i++){
		//PFN
		physical_memory[i*2] = (i+2);
		//Page Table Entry
		physical_memory[(i*2)+1] = 0x10;
	}

	printf("Writing Page Table Entries for Swapping Examples...\n");

	//Store entries for pages not mapped to a valid PFN
	physical_memory[framesOccupied*2] = framesOccupied + 2;
	physical_memory[(framesOccupied*2)+1] = 0x0;
	physical_memory[((framesOccupied+1)*2)] = framesOccupied + 3;
	physical_memory[((framesOccupied+1)*2)+1] = 0x0;

	printf("Done...\n\n");
}

//Writes the contents of our physical memory out to a file
void dumpPhysicalMemoryToDisk(unsigned short *physical_memory){

	printf("Dumping contents of Physical Memory to disk...\n");

	FILE * physical_memory_dump_file;
	physical_memory_dump_file = fopen("data/physical_memory_dump.txt", "w");

	if(physical_memory_dump_file == NULL){
		printf("ERROR LOADING PHYSICAL MEMORY DUMP FILE\nExiting...\n");
		exit(1);
	}

  fprintf(physical_memory_dump_file, "ADDRESS  |  FRAME	| CONTENT\n");
	fprintf(physical_memory_dump_file, "-------------------------------\n");

	for(int i = 0; i < MAX_ADDRESSABLE_BYTES; i++){
		fprintf(physical_memory_dump_file, "0x%04X   | %4d   | %C\n", i, i >> 8, physical_memory[i]);
	}

	printf("Done...\n\n");

	fclose(physical_memory_dump_file);
}

//Dumps the content of our "process'" Page Table
void dumpPageTableToDisk(unsigned short *physical_memory){

	printf("Dumping the contents of the Page Table to disk...\n");

	FILE * page_table_dump_file;
	page_table_dump_file = fopen("data/page_table_dump.txt", "w");

	if(page_table_dump_file == NULL){
		printf("ERROR LOADING PAGE TABLE DUMP FILE\nExiting...\n");
		exit(1);
	}

	fprintf(page_table_dump_file, "		VPN	  |		PFN			|	Page Table Entry\n");
	fprintf(page_table_dump_file, "-----------------------------------------\n");

	for(int i = 0 ; i < 256; i++){

		unsigned short pfn = physical_memory[i*2];
		unsigned short pageTableEntry = physical_memory[(i*2)+1];

		if(pfn != 0){
			fprintf(page_table_dump_file, "		%4d	| %4d			|	", i, pfn);

			//Check Status of Present Bit
			bool present = false;

			if(((pageTableEntry & PRESENT_BIT_BITMASK) >> 4) == 1){
				present = true;
			}

			fprintf(page_table_dump_file, "Present: ");
			fprintf(page_table_dump_file, (present ? "true" : "false"));

			//Check Status of Accessed Bit
			bool accessed = false;

			if((pageTableEntry & ACCESSED_BIT_BITMASK) == 1){
				accessed = true;
			}

			fprintf(page_table_dump_file, " Accessed: ");
			fprintf(page_table_dump_file, (accessed ? "true\n" : "false\n"));
		}
	}

	printf("Done...\n\n");

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

	printf("Randomly writing data to physical memory...\n");

	//Generate random number between 2048-20480, the bytes of data we will write to our memory
	unsigned short randomBytes = randomShortInRange(2048, 20480);

	printf("Writing %d Bytes of data...\n", randomBytes);

	//Randomly write the aforementioned amount of data to our memory, using a loop
	//We will randomly write characters, which are 1 byte each, so loop for the amount of
	//bytes we randomly generated
	for(int i = 512; i < (randomBytes + 512); i++){
		//Randomly generate an ASCII character code (1 Byte) to write to the aforementioned address
		unsigned short randomCharCode = randomShortInRange(33, 126);

		//Assign the character to our physical memory address
		physical_memory[i] = randomCharCode;
	}

	printf("Done...\n\n");

	//Write the contents for the process' Page Table to Physical Memory, for the pages used by our process
	writePageTableToPhysicalMemory(physical_memory, randomBytes);
}

//Returns a random unsigned short between the specified min and max values
unsigned short randomShortInRange(unsigned short min, unsigned short max){
	return ((rand() % (max - min)) + min);
}

//Randomly prints an entry out of the first 8 entries in our process' Page Table
void randomPrintPageTableEntry(unsigned short *physical_memory){

	printf("Random Page Table Entry Example:\n\n");
	printf("   VPN  |   PFN    | Page Table Entry\n");
	printf("-----------------------------------------\n");

	printPageTableEntry(physical_memory, randomShortInRange(0,7));
}


//Prints the Page Table entry for the given Page Number
void printPageTableEntry(unsigned short *physical_memory, unsigned short pageNumber){
	unsigned short pfn = physical_memory[pageNumber*2];
	unsigned short pageTableEntry = physical_memory[(pageNumber*2)+1];

	if(pfn != 0){
		printf("   %3d 	| %4d     | ", pageNumber, pfn);

   	//Check Status of Present Bit
		bool present = false;

		if(((pageTableEntry & PRESENT_BIT_BITMASK) >> 4) == 1){
			present = true;
		}

		printf("Present: ");
		printf((present ? "true" : "false"));

		//Check Status of Accessed Bit
		bool accessed = false;

		if((pageTableEntry & ACCESSED_BIT_BITMASK) == 1){
			accessed = true;
		}

		printf(" Accessed: ");
		printf((accessed ? "true\n" : "false\n"));
	}
	else{
		printf("Page Table Entry does not point to a valid PFN\n");
	}
}

void swapFrameIntoMemory(unsigned short *physical_memory, unsigned short pfn, unsigned short vpn){

	//Generate Random Bytes in our Hypothetical "Frame", load this into our Physical Memory
	unsigned short randomBytes = randomShortInRange(1, 256);

	for(int j = 0; j < randomBytes; j++){
		physical_memory[(PAGE_AND_FRAME_SIZE_BYTES*pfn)+j] = randomShortInRange(33, 126);
	}

	//Update Page Table
	printf("Setting Present Bit for Page Table Entry to 1...\n");
	physical_memory[(vpn*2)+1] = ((physical_memory[(vpn*2)+1]) | PRESENT_BIT_BITMASK);
	dumpPageTableToDisk(physical_memory);
}
