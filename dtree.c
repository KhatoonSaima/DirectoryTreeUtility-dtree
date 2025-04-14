 /* 
 * 
 * C program dtree that performs actions based on various command line arguments
 *
 * Use of system call nftw(): File Tree Walk Function
 *
 * Features:
 * 1. dtree -ls [root_dir] -> List all files & directories
 * 2. dtree -ext [root_dir] [file_extension] -> List files with a specific extension
 * 3. dtree -fc [root_dir] -> Count all files
 * 4. dtree -dc [root_dir] -> Count all directories
 * 5. dtree -fs [root_dir] -> List sizes of all files
 * 6. dtree -cp [source_dir] [destination_dir] [file_extension] -> Copy contents of a directory, excluding a specific file type
 * 7. dtree -mv [source_dir] [destination_dir] -> Move a directory
 * 8. dtree -del [root_dir] [file_extension] -> Delete all files of a specific extension
 *
 * Usage:
 * ------
 * Compile: gcc dtree.c -o dtree
 * Run:     ./dtree < -ls | -ext | -fc | -dc | -fs | -cp | -mv | -del > <root_dir> [additional_args]
 *
 * Author: Saima Khatoon
 * Date: 01-04-2025
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ftw.h>
#include <fcntl.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define PATH_MAX 4096   // macro, used for storing path, defines the maximum length of a file path

// Global Variables for target file extension to search for
char target_extension[10];
char excluded_extension[10] = "";  // File extension to exclude (if any)

// Global variables for source and destination directories
char source_root[PATH_MAX]; // Max length 1024 for MAC
char dest_root[PATH_MAX];

// Global variables for counters to count files and directories
int count=0;

// Callback function declarations for nftw()
int list_files(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int find_files_with_extension(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int count_files_directories(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int count_directories(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int list_file_sizes(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int copy_files(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int move_directory(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int delete_original(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int delete_files_by_extension(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);

// Other function declaration
const char *get_file_extension(const char *filename);
void create_dest_directory(const char *src_path);
void copy_file(const char *src_path, const char *dest_path);

int main(int argc, char ** argv)
{
    // Number of arguments can not be less than 3 and greated than 5.
    // Either 3 or 4 or 5.
    if (argc < 3 || argc > 5) {
        fprintf(stderr, "Usage: %s < -ls | -ext | -fc | -dc | -fs | -cp | -mv | -del > <root_dir> [additional_args]\n", argv[0]);
        exit(EXIT_FAILURE); // EXIT_FAILURE = 1, represent standard failure status code when a program terminates due to an error.
    }
    
    // 1st Part
    // dtree -ls [root_dir]
    // Displays the full paths of all files and directories within the subtree starting from root_dir.
    if (!strcmp(argv[1],"-ls")) // strcmp returns 0 on success
    {
        // Accepts three arguments only
        if(argc != 3)
        {
            fprintf(stderr, "Usage: %s -ls <root_dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        // Traverse the directory tree starting from argv[2]
        // argv[2]: 3rd command line argument
        else
        {
            printf("All files and directories with its path and filetype :\n");
            if(nftw(argv[2], list_files, 10, FTW_PHYS) == -1) {
            perror("nftw error in listing files");
            exit(EXIT_FAILURE);
            }
           
        }
    }


    // 2nd Part
    // dtree -ext [root_dir] [file_extension]
    // Displays the absolute paths of all files with the specified extension in the subtree rooted at root_dir.
    if (!strcmp(argv[1],"-ext"))
    {
        if(argc != 4) //command line arguments: 0 1 2 3 , total=4
        {
            fprintf(stderr, "Usage: %s -ext <root_dir> <file_extension>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        else
        { 
            // Get the root directory and file extension from command-line arguments
            const char *root_dir = argv[2];
            strncpy(target_extension, argv[3], sizeof(target_extension) - 1); // Copy extension to target_extension e.g target_extension = .c
            target_extension[sizeof(target_extension) - 1] = '\0'; // Add null termination character at the end

            // For Debugging
            // printf("%c%c%c", target_extension[0], target_extension[1], target_extension[2]); 

            // Traverse the directory tree starting from argv[2]
            // argv[2]: 3rd command line argument
            if (nftw(argv[2], find_files_with_extension, 10, FTW_PHYS) == -1) {
                perror("nftw error in listing files");
                exit(EXIT_FAILURE);
            }
        }
    }

    // 3rd Part
    // dtree -fc [root_dir]
    // Prints the total number of files/directories in the subtree rooted at root_dir.
    if (!strcmp(argv[1],"-fc"))
    {
        if(argc != 3)
        {
            fprintf(stderr, "Usage: %s -fc <root_dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        else
        {
            // Traverse the directory tree starting from argv[2]
            // argv[2]: 3rd command line argument
            if(nftw(argv[2], count_files_directories, 10, FTW_PHYS) == -1) {
            perror("nftw error in counting files");
            exit(EXIT_FAILURE);
            }
        }
        printf("Total number of files and directories : %d\n",count);
    }

    // 4th Part
    // dtree -dc [root_dir]
    // Displays the count of all directories within the subtree starting from root_dir.
    if (!strcmp(argv[1],"-dc"))
    {
        if(argc != 3)
        {
            fprintf(stderr, "Usage: %s -dc <root_dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        else
        {
            // Traverse the directory tree starting from argv[2]
            // argv[2]: 3rd command line argument
            if(nftw(argv[2], count_directories, 10, FTW_PHYS) == -1) {
            perror("nftw error in counting directories");
            exit(EXIT_FAILURE);
            }
        }
        printf("Total number of directories : %d\n",count);
    }

    // 5th Part
    // dtree -fs [root_dir]
    // Retrieves and lists the size (in bytes) of all files within the entire directory tree, starting from the root directory (root_dir).
    if (!strcmp(argv[1],"-fs"))
    {
        if(argc != 3)
        {
            fprintf(stderr, "Usage: %s -fs <root_dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
         else
        {
            printf("All files with its path and size :\n");
            if(nftw(argv[2], list_file_sizes, 10, FTW_PHYS) == -1) {
            perror("nftw error");
            exit(EXIT_FAILURE);
            }
        }
    }

    // 6th Part
    // dtree -cp [source_dir] [destination_dir] [file_extension]
    // Copy the entire subdirectory rooted at source_dir to destination_dir Excluding the files provided in the file_extension
    if (!strcmp(argv[1],"-cp"))
    {
        if(argc < 4 || argc > 5 ) //0 1 2 3 4, total=5
        {
            fprintf(stderr, "Usage: %s -cp <source_dir> <destination_dir> <file_extension_to_exclude(optional)>\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        realpath(argv[2], source_root); // Copy absolute path to source_root GV
        realpath(argv[3], dest_root);   // Copy absolute path to dest_root GV
        printf("Copying '%s' to '%s'\n", source_root, dest_root);

         if(argc == 5)
        {
            // Extension is the last argument; argv[4]
            // Copy the last argument in excluded_extension global variable
            strncpy(excluded_extension, argv[4], sizeof(excluded_extension) - 1);
            excluded_extension[sizeof(excluded_extension) - 1] = '\0'; // Ensure null termination
        }
        // else target_extension is null already
     
        // Traverse the directory tree starting from argv[2]
        // Copy all files except files with extension given in last argument
        if (nftw(source_root, copy_files, 20, FTW_PHYS) == -1) {
            perror("Error during copy operation");
            exit(EXIT_FAILURE);
        }
    }


    // 7th Part
    // dtree -mv [source_dir] [destination_dir]
    // Move the entire subdirectory rooted at source_dir to destination_dir and delete the directory(and contents) rooted at source_dir.
    if (!strcmp(argv[1],"-mv"))
    {
        if(argc != 4)
        {
            fprintf(stderr, "Usage: %s -mv <source_dir> <destination_dir>\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        realpath(argv[2], source_root); // Resolve absolute path
        realpath(argv[3], dest_root);   // Resolve absolute path
        printf("Moving '%s' to '%s'\n", source_root, dest_root);

        // Traverse the directory tree starting from argv[2]
        // Move all contents inside the source_dir - both files and directories
        if (nftw(source_root, move_directory, 20, FTW_PHYS) == -1) {
            perror("Error during move operation");
            exit(EXIT_FAILURE);
        }

        // Delete the original directory structure
        if (nftw(source_root, delete_original, 10, FTW_DEPTH | FTW_PHYS) == -1) {
            perror("Error deleting source directory");
            exit(EXIT_FAILURE);
        }
    }


    // 8th Part
    // dtree -del [root_dir] [file_extension]
    // Deletes all files of a specific file extension in the subtree rooted at root_dir.
    if (!strcmp(argv[1],"-del"))
    {
        if(argc != 4)
        {
            fprintf(stderr, "Usage: %s -del <root_dir> <file_extension>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        else
        { 
        // Get the root directory from command-line argument; argv[2]
        const char *root_dir = argv[2];

        // Extension is the last argument; argv[4]
        // Copy the last argument in target_extension global variable
        strncpy(target_extension, argv[3], sizeof(target_extension) - 1);
        target_extension[sizeof(target_extension) - 1] = '\0'; // Ensure null termination   

        //For debugging
        //printf("%c%c%c", target_extension[0], target_extension[1], target_extension[2]); 

        // Traverse the directory tree starting from argv[2]
        if (nftw(argv[2], delete_files_by_extension, 10, FTW_PHYS) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        }
    }
}

/**
 * Callback function for nftw().
 * Recursively lists all files and directories along with their absolute paths.
 * This function is called for each file and directory found in the traversal.
 *
 * @param fpath     The full path of the current file or directory.
 * @param sb        Pointer to the struct stat containing file metadata.
 * @param typeflag  Specifies the type of file (FTW_F for files, FTW_D for directories, etc.).
 * @param ftwbuf    Provides additional information such as depth in the directory tree.
 *
 * @return 0 to continue traversal, or a nonzero value to stop.
 */
int list_files(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    // Print the file path and its type
    printf("%s | Type: ", fpath);
    count++;
    if (typeflag == FTW_F)
        printf("Regular File\n");
    else if (typeflag == FTW_D)
        printf("Directory\n");
    else if (typeflag == FTW_SL)
        printf("Symbolic Link\n");
    else
        printf("Other\n");

    return 0; // Continue the traversal
}


// Recursively searches for files with a specific extension and prints their absolute paths.
int find_files_with_extension(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    //printf("%s\n", fpath);
    if (typeflag == FTW_F) { 
        // Only process regular files
        const char *extension = get_file_extension(fpath);

        // For Debugging
        // printf("File:%s, Extension:%s\n", fpath,extension);

        // Match with the target extension
        if (strcmp(extension, target_extension) == 0) {
            printf("%s\n", fpath); // Print the absolute path of the matching file
        }
      
    }
    return 0; // Continue traversal
}

// Function to get the file extension
const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.'); // Find the last dot
    if (!dot || dot == filename) {
        return "No files found"; // No extension
    }
    return dot; // Skip the dot
}

// Recursively counts all files/directories in the directory tree starting from root_dir and prints the total count.
int count_files_directories(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    count++;
    return 0; // Continue the traversal
}

// Recursively counts all directories in the directory tree starting from root_dir and prints the total count.
int count_directories(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D)
        count++;
    return 0; // Continue the traversal
}

// Recursively lists the size (in bytes) of all files encountered during the traversal of the directory tree rooted at root_dir.
int list_file_sizes(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) { // Only process regular files
        printf("File: %s | Size: %lld bytes\n", fpath, sb->st_size);
    }
    return 0; // Continue traversal
}

// Recursively copies the subdirectory from source_dir to destination_dir, 
// omitting files with the specified file_extension.
int copy_files(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    char dest_path[PATH_MAX];

    // Compute the destination path
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_root, fpath + strlen(source_root) + 1);

    if (typeflag == FTW_D) {
        // Create directory in the destination
        create_dest_directory(fpath);
    } else if (typeflag == FTW_F) {
        // Get file extension
        const char *extension = get_file_extension(fpath);

        // Skip files if they match the excluded extension
        if (excluded_extension[0] != '\0' && strcmp(extension, excluded_extension) == 0) {
            printf("Skipping file: %s (excluded extension: .%s)\n", fpath, excluded_extension);
            return 0;
        }

        // Copy file
        printf("Copying: %s -> %s\n", fpath, dest_path);
        copy_file(fpath, dest_path);
    }
    
    return 0;
}

// Function to copy a file
void copy_file(const char *src_path, const char *dest_path) {
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        return;
    }

    int dest_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        close(src_fd);
        return;
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        write(dest_fd, buffer, bytes_read);
    }

    close(src_fd);
    close(dest_fd);
}

// Moves the entire subdirectory from source_dir to destination_dir.
int move_directory(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    char dest_path[PATH_MAX];

    // Construct the destination path
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_root, fpath + strlen(source_root)+1);

    // For debugging
    /*printf("fpath: %s\n", fpath);
    printf("fpath + strlen(source_root)+1:%s\n", fpath + strlen(source_root)+1);
    printf("dest_root: %s\n", dest_root);
    printf("dest_path: %s\n", dest_path);*/

    if (typeflag == FTW_D) {
        printf("Creating Directory: %s\n", dest_path);
        // Create the directory in the destination
        create_dest_directory(fpath);
    } else if (typeflag == FTW_F || typeflag == FTW_SL) {
        printf("Moving File from %s to %s\n", fpath, dest_path);
        // Move the file
        if (rename(fpath, dest_path) == -1) {
            perror("Error moving file");
            exit(EXIT_FAILURE);
        }
    }
    else {
        printf("Skipping unknown type: %s\n", fpath);
    }

    return 0;
}

// Deletes the original directory (and its contents) from source_dir during the directory traversal.
// Function to delete the original directory after moving contents
int delete_original(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (remove(fpath) == -1) {
        perror("Error deleting file or directory");
        exit(EXIT_FAILURE);
    }
    return 0;
}

// Function to create directories in the destination path
void create_dest_directory(const char *src_path) {
    char dest_path[PATH_MAX];

    // Create equivalent path in the destination directory
    snprintf(dest_path, sizeof(dest_path), "%s/%s", dest_root, src_path + strlen(source_root) + 1);

    printf("Creating directory dest_path: %s\n",dest_path);

    // Finally, create the full directory
    if (mkdir(dest_path, 0777) == -1 && errno != EEXIST) {
        perror("Error creating directory");
        exit(EXIT_FAILURE);
    }
}


// Removes all files with a specific extension in the directory tree rooted at root_dir.
int delete_files_by_extension(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) { // Only process regular files
        const char *extension = get_file_extension(fpath);
        if (strcmp(extension, target_extension) == 0) {
            printf("Deleting: %s\n", fpath);
            if (remove(fpath) == 0) {
                printf("Deleted successfully: %s\n", fpath);
            } else {
                perror("Error deleting file");
            }
        }
    }
    return 0; // Continue traversal
}