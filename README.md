dtree - Directory Tree Utility

dtree is a command-line tool written in C that performs various operations on a directory tree using the nftw() system call. It allows listing, counting, copying, moving, and deleting files/directories under a specified root directory.

Features

List Files and Directories
    ./dtree -ls [root_dir]
    Recursively lists all files and directories (with paths) under root_dir.

List Files by Extension
    ./dtree -ext [root_dir] [file_extension]
    Lists all files with a specific extension (e.g., .c, .txt) under root_dir.

Count Files
    ./dtree -fc [root_dir]
    Outputs the total count of files in the entire subtree under root_dir.

Count Directories
    ./dtree -dc [root_dir]
    Outputs the total count of directories in the subtree.

Total File Size
    ./dtree -fs [root_dir]
    Displays the cumulative size (in bytes) of all files in the subtree.

Copy Directory Tree
    ./dtree -cp [source_dir] [destination_dir] [file_extension]
    Copies the directory tree from source_dir to destination_dir, optionally excluding files with the given extension. If file_extension is omitted, all files are copied.

Move Directory Tree
    ./dtree -mv [source_dir] [destination_dir]
    Moves the entire directory tree from source_dir to destination_dir, deleting the original after the move.

Delete Files by Extension
    ./dtree -del [root_dir] [file_extension]
    Deletes all files with the given extension under root_dir.

System Requirements

- OS: Unix/Linux
- Compiler: GCC or compatible
- Required Header: ftw.h (for nftw() function)

Compilation

Use the following command to compile:
    gcc -o dtree dtree.c -std=c99 -Wall

Notes

- Ensure the input paths are absolute and accessible.
- The program relies on nftw() for efficient directory tree traversal.
- File extension parameters should begin with a dot (e.g., .c).

Example

    ./dtree -ext /home/user/docs .pdf
    Lists all .pdf files under /home/user/docs.

Author

- Saima Khatoon
