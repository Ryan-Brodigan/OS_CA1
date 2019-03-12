# OS_CA1
Ryan Brodigan, 4th Year Operating Systems CA1

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
