///-------------------------------------------------------------------------------------------------
/// <summary>
/// File: main.c
/// 
/// This file contains the implementation of a tool to find code caves in PE (Portable Executable) files.
/// Code caves are continuous sequences of zero bytes within the file, which can potentially be used
/// for code injection or other purposes.
/// </summary>
///-------------------------------------------------------------------------------------------------
/// <author>Hence0x</author>
/// <date>2024-07-26</date>
///-------------------------------------------------------------------------------------------------
/// <description>
/// This tool analyzes PE files to find code caves of a specified minimum size.
/// It iterates through all sections of the PE file and reports the location and size of any caves found.
/// 
/// Features:
/// - Analyzes PE file structure
/// - Finds code caves in all sections
/// - Configurable minimum cave size
/// - Reports cave locations and sizes
/// 
/// Usage:
/// PECaveFinder.exe <PE_file> <min_cave_size>
/// 
///-------------------------------------------------------------------------------------------------
/// <dependencies>
/// - Windows API: For PE file structure definitions
/// </dependencies>
///-------------------------------------------------------------------------------------------------
/// <compilation>
/// Compile with a C compiler that supports C99 or later.
/// Ensure Windows SDK is available for necessary header files.
/// </compilation>
///-------------------------------------------------------------------------------------------------

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

///-------------------------------------------------------------------------------------------------
/// <summary>
/// Finds and prints code caves in a PE file.
/// </summary>
/// <param name="fname">The filename of the PE file to analyze.</param>
/// <param name="min_cave">The minimum size of code caves to report.</param>
/// <remarks>
/// This function reads the PE file, parses its structure, and searches for continuous
/// sequences of zero bytes (caves) in each section. It reports caves that are at least
/// as large as the specified minimum size.
/// </remarks>
///-------------------------------------------------------------------------------------------------
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

    // Allocate buffer and read file content
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

///-------------------------------------------------------------------------------------------------
/// <summary>
/// Main entry point of the program.
/// </summary>
/// <param name="argc">The number of command-line arguments.</param>
/// <param name="argv">Array of command-line argument strings.</param>
/// <returns>0 if the program executes successfully, 1 if there's an error in arguments.</returns>
/// <remarks>
/// This function handles command-line arguments and calls the fcaves function
/// to analyze the specified PE file.
/// </remarks>
///-------------------------------------------------------------------------------------------------
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
