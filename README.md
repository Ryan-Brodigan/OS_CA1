# OS_CA1
Ryan Brodigan, 4th Year Operating Systems CA1

# Technical Overview
16-Bit System (2^16 Addressable Bytes = 65536)

Page Size is 256 Bytes -> log2(256) = 8 or 2^8 Bytes to store
each individual Page Offset

2^16 / 2^8 = 2^8 bytes to store each page number

So, 8 Bits (Or 2 Nibbles / Two Hex Chars) to store an Individual
Page Numbers and the Page offset for an Address

Can Store 2^8 Page numbers or 256 Pages (2^8 Bytes), each of which takes up
one 16-bit Address (2^1 Bytes), therefore the page table must be 2^8*2^1
or 2^9 bytes (4096 bytes) in size or 4 KiB
