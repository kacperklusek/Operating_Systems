#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "stdlib.h"
#include <ftw.h>


const char* get_type(mode_t m){
    if (S_ISDIR(m)) return "dir";
    else if (S_ISREG(m)) return "file";
    else if (S_ISSOCK(m)) return "sock";
    else if (S_ISFIFO(m)) return "fifo";
    else if (S_ISCHR(m)) return "char dev";
    else if (S_ISBLK(m)) return "block dev";
    else if (S_ISLNK(m)) return "slink";
    else return "unknown";
}
void print_header() {
    printf("%-75s", "path");
    printf("%-4s", "hl");
    printf("%-10s ", "file type");
    printf("%-13s", "size");
    printf("%-22s", "last access date");
    printf("%-22s\n", "last modification date");
}

void print_file_info(const char* dir_name, struct stat* buffer) {
    char path[512];
    char date[20];

    // abs path
    realpath(dir_name, path);

    printf("%-75s", path);

    // number of hardlinks
    printf("%-4ld", buffer->st_nlink);

    // file type
    printf("%-10s ", get_type(buffer->st_mode));

    // file size (bytes)
    printf("%-13ld", buffer->st_size);

    // last access date
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&buffer->st_atime));
    printf("%-22s", date);

    // last modification date
    strftime(date, 20, "%Y-%m-%d %H:%M:%S", localtime(&buffer->st_mtime));
    printf("%-22s\n", date);
}

void browse_rec(const char* dir_name, struct stat* buffer) {
    DIR* curr_dir = opendir(dir_name);
    if (curr_dir == NULL) {
        printf("Error opening directory %s\n", dir_name);
        exit(1);
    }

    struct dirent* dir_elem;

    while((dir_elem = readdir(curr_dir)) != NULL) {
        char name[strlen(dir_name) + 2 + strlen(dir_elem->d_name)];
        sprintf(name, "%s/%s", dir_name, dir_elem->d_name);

        if (strcmp(dir_elem->d_name, "..") == 0 || strcmp(dir_elem->d_name, ".") == 0){
            continue;
        }
        if (lstat(name, buffer) != 0) {
            printf("Error lstat'ing file %s\n", name);
        }

        print_file_info(name, buffer);

        if (S_ISDIR(buffer->st_mode)) {
            browse_rec(name, buffer);
        }

    }

}

void browse_directory (const char* dir_name) {
    struct stat buffer;
    print_header();
    browse_rec(dir_name, &buffer);
}

int handle_ftw_item(const char* path, struct stat* sb){
    print_file_info(path, sb);
    return 0;
}

void ftw_browse_directory (const char* dir_name) {
    print_header();
    nftw(dir_name, (__nftw_func_t) handle_ftw_item, 10, FTW_PHYS);
}


int main(int argc, char** argv) {

    if (argc < 2) {
        printf("M8 you have to add at least one parameter");
        exit(1);
    }

    const char* dir_name = argv[1];

    printf("using opendir, readdir\n");
    browse_directory(dir_name);
    printf("\nusing nftw\n");
    ftw_browse_directory(dir_name);


    return 0;
}