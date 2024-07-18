# Code Cave Finder

This tool finds code caves in PE (Portable Executable) files.

## How it works

The program scans through the sections of a PE file, looking for continuous sequences of zero bytes. When it finds a sequence longer than the minimum cave size, it reports the location and size of the cave.

## Usage

1. Run the program with a PE file as an argument:

   ```
   cave_finder.exe <path_to_pe_file> <size>
   ```

## Output

The program will print out the locations and sizes of any code caves it finds, like this:

```
cave found in .text at 0x1000, size: 100 bytes
cave found in .data at 0x2000, size: 200 bytes
```
