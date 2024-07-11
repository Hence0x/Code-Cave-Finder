#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define MIN_CAVE 50 // min size of code cave to find (bytes)

void fcaves(const char* fname) {
    FILE* f = fopen(fname, "rb");
    if (!f) {
        printf("cant open file.\n");
        return;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char* buf = (unsigned char*)malloc(fsize);
    if (!buf) {
        printf("mem alloc failed.\n");
        fclose(f);
        return;
    }

    fread(buf, 1, fsize, f);
    fclose(f);

    PIMAGE_DOS_HEADER dh = (PIMAGE_DOS_HEADER)buf;
    PIMAGE_NT_HEADERS nh = (PIMAGE_NT_HEADERS)(buf + dh->e_lfanew);

    // go through sections
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
                if (csize >= MIN_CAVE) {
                    printf("cave found in %s at 0x%X, size: %d bytes\n",
                        sec->Name, (unsigned int)(p - buf - csize), csize);
                }
                csize = 0;
            }
        }

        // check for cave at section end
        if (csize >= MIN_CAVE) {
            printf("cave found in %s at 0x%X, size: %d bytes\n",
                sec->Name, (unsigned int)(end - buf - csize), csize);
        }
    }

    free(buf);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <PE_file>\n", argv[0]);
        return 1;
    }

    fcaves(argv[1]);
    return 0;
}
