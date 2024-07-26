/*
 * File: main.c
 * Author: Hence0x
 * Date: 7/26/2024
 * 
 * Description:
 * This program analyzes Portable Executable (PE) files to find "caves" - 
 * contiguous areas of zero bytes within the file. It's useful for identifying
 * potential areas where additional code or data could be inserted without
 * changing the file's size.
 * 
 * Usage:
 * pe_cave_finder.exe <PE_file> <min_cave_size>
 * 
 * Compilation:
 * Compile with Visual Studio or using gcc with the following command:
 * gcc -o pe_cave_finder pe_cave_finder.c
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/**
 * @brief Finds and prints information about caves in a PE file.
 * 
 * This function reads a PE file, analyzes its sections, and identifies
 * contiguous areas of zero bytes (caves) that are at least as large as
 * the specified minimum size.
 * 
 * @param fname The filename of the PE file to analyze.
 * @param min_cave The minimum size of caves to report, in bytes.
 * 
 * @remarks This function uses Windows-specific structures to parse the PE file.
 *          It assumes a valid PE file format and does minimal error checking.
 */
void fcaves(const char* fname, int min_cave) {
    FILE* f = fopen(fname, "rb");
    if (!f) {
        printf("Can't open file.\n");
        return;
    }

    // Determine file size
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate buffer and read file contents
    unsigned char* buf = (unsigned char*)malloc(fsize);
    if (!buf) {
        printf("Memory allocation failed.\n");
        fclose(f);
        return;
    }
    fread(buf, 1, fsize, f);
    fclose(f);

    // Parse PE headers
    PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)buf;
    PIMAGE_NT_HEADERS nh = (PIMAGE_NT_HEADERS)(buf + dh->e_lfanew);
    
    // Iterate through sections
    PIMAGE_SECTION_HEADER sec = IMAGE_FIRST_SECTION(nh);
    for (int i = 0; i < nh->FileHeader.NumberOfSections; i++, sec++) {
        unsigned char* start = buf + sec->PointerToRawData;
        unsigned char* end = start + sec->SizeOfRawData;
        int csize = 0;

        // Search for caves in the section
        for (unsigned char* p = start; p < end; p++) {
            if (*p == 0) {
                csize++;
            }
            else {
                if (csize >= min_cave) {
                    printf("Cave found in %s at 0x%X, size: %d bytes\n",
                        sec->Name, (unsigned int)(p - buf - csize), csize);
                }
                csize = 0;
            }
        }

        // Check for cave at the end of the section
        if (csize >= min_cave) {
            printf("Cave found in %s at 0x%X, size: %d bytes\n",
                sec->Name, (unsigned int)(end - buf - csize), csize);
        }
    }

    free(buf);
}

/**
 * @brief Main entry point for the PE cave finder program.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * 
 * @return int 0 if the program executed successfully, 1 otherwise.
 * 
 * @remarks Usage: pe_cave_finder.exe <PE_file> <min_cave_size>
 * 
 * Example:
 * @code
 * pe_cave_finder.exe example.exe 100
 * @endcode
 */
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <PE_file> <min_cave_size>\n", argv[0]);
        return 1;
    }

    int min_cave = atoi(argv[2]);
    if (min_cave <= 0) {
        printf("Minimum cave size must be a positive integer.\n");
        return 1;
    }

    fcaves(argv[1], min_cave);
    return 0;
}
