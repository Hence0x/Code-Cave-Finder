#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

void fcaves(const char* fname, int min_cave) {
    FILE* f = fopen(fname, "rb");
    if (!f) {
        printf("Can't open file.\n");
        return;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char* buf = (unsigned char*)malloc(fsize);
    if (!buf) {
        printf("Memory allocation failed.\n");
        fclose(f);
        return;
    }
    fread(buf, 1, fsize, f);
    fclose(f);

    PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)buf;
    PIMAGE_NT_HEADERS nh = (PIMAGE_NT_HEADERS)(buf + dh->e_lfanew);
    

    PIMAGE_SECTION_HEADER sec = IMAGE_FIRST_SECTION(nh);
    for (int i = 0; i < nh->FileHeader.NumberOfSections; i++, sec++) {
        unsigned char* start = buf + sec->PointerToRawData;
        unsigned char* end = start + sec->SizeOfRawData;
        int csize = 0;
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
        if (csize >= min_cave) {
            printf("Cave found in %s at 0x%X, size: %d bytes\n",
                sec->Name, (unsigned int)(end - buf - csize), csize);
        }
    }
    free(buf);
}



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
