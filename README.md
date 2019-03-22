# OS_CA1
Ryan Brodigan, 4th Year Operating Systems CA1

# Make and Run the App
From the project's root directory enter the command "make" to compile the source code into object code
and then type the command "./dist/main" to launch the application

# Technical Overview
16-Bit System (2^16 Addressable Bytes = 65536)

Page Size is 256 Bytes -> log2(256) = 8 or 2^8 Bytes to store
each individual Page Offset

2^16 / 2^8 = 2^8 bytes to store each page number

So, 8 Bits (Or 2 Nibbles / Two Hex Chars) to store an Individual
Page Number and 8 bits to store the Page offset for an Address,
16 bits in total

Can Store 2^8 Page numbers or 256 Pages (2^8 Bytes), each of which takes up
one 16-bit Address (2(^1) Bytes), so our Page table will be 512 bytes (256 * 2)
in size, by storing this in our physical memory starting at address 0x0000, the
Page Table for our "Process" will take up the first two Frames of our Physical Memory

# Which Page Table Entry to print?

Given that we generate a random amount of data to write to physical memory, that belongs
to our "process", we will write at the very least 2048 bytes of data to physical memory,
encompassing 8 frames in total (2048 / 256 = 8). When printing an example of a Page Table
entry to the console we can therefore elect to use the Page Table entries that point to the
first 8 frames of our physical memory

# Structure of Page Table Entry

The structure of our Page Table entries relay various piece of information to our system whilst
it is attempting to translate a Page Number to a Frame Number, such as whether or not the corresponding
frame is "present" (I.E. The data therein is currently loaded into physical memory).

I have elected to structure my Page Table entries as follows:

Example Entry: 0x000000XY

X: Our "Present" bit, indicating whether the corresponding Frame that the Page Table Entry points to
is loaded into our physical memory, if this is set to 0 we throw a Page Fault exception, load the contents
of the Frame back into our physical memory (A frame that is not already occupied) and then update our
Page Table to relect the new Frame Number & set our "Present" bit to 1.

Y: Our "Accessed" bit, indicates whether or not our system has accessed the contents of the corresponding Frame,
is initially set to 0 but will be set to 1 if we access the contents of a Frame (Through the process of Virtual Address
Translation).
