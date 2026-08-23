## This project is a from-scratch implementation of the LZW (Lempel–Ziv–Welch) compression algorithm in C, using a Trie-based dictionary for compression and a dynamic string table for decompression.

The goal of this project was to understand:

1.How real compression algorithms work internally
2.How dictionaries grow dynamically
3.How to manage variable-width codes (16 → 24 → 32 bits)
4.Memory management in C for non-trivial data structures

# Files
.
├── compress.c     // LZW compressor
├── decompress.c   // LZW decompressor
├── trie.c         // Trie implementation used by compressor
├── input.txt      // Input file for compression
├── compressed.dat // Output of compression
└── decompressed.dat // Output of decompression

# How It Works 
> Compression 

Initializes a dictionary with all single-byte characters (0–255)
Uses a Trie to efficiently check if a sequence exists
Reads input byte-by-byte
Outputs dictionary codes instead of raw bytes
Automatically switches code size:
16-bit → 24-bit → 32-bit
Stores offsets in the output file header so decompression knows when code sizes change

# Decompression

Reads header information (code-size offsets and max code)
Rebuilds the dictionary dynamically using a string table
Handles the special LZW case where a code is referenced before being fully defined
Reconstructs the original file exactly

# Compilation

Use GCC:

gcc -O1 compress.c -o compress
gcc -O1 decompress.c -o decompress

> trie.c is included directly in compress.c, so it does not need to be compiled separately.

# Usage
Compress a file
./compress


Reads from input.txt

Writes compressed output to compressed.dat

Decompress the file
./decompress

Author

Written as a learning project by Mukhyansh.
