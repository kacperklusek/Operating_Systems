#include <dirent.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <stdlib.h>

void print_header(const char* target_str) {
    printf("%-75s", "path");
    printf("%-12s", "PID");
    printf("%-8s '%s'", "contains", target_str);
    printf("\n");
}

int check_search(const char* path, const char* target_str) {
    FILE *fp = fopen(path, "r");
    char* line;
    size_t len = 0;

    if (!fp) {
        puts("ERROR READING FILE");
        exit(1);
    }
    while (getline(&line, &len, fp) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strstr(line, target_str)) {
            fclose(fp);
            if (line) free(line);
            return 0;
        }
    }
    fclose(fp);
    if(line) free(line);
    return 1;
}

void print_search_result(const char* dir_name, struct stat* buffer, const char* target_str) {
    char path[256];

    // abs path
    realpath(dir_name, path);

    int search_result = check_search(path, target_str);

    printf("%-75s", path);
    printf("%-12d", getpid());
    if(search_result == 0) {
        printf("%-9s", "true");
    } else {
        printf("%-8s", "false");
    }
    printf("\n");


}

void browse_rec(const char* dir_name, struct stat* buffer, const char* target_str, int depth) {
    if (depth == 0) {
        return;
    }
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

        if (S_ISDIR(buffer->st_mode)) {
            if (fork() == 0) {
                browse_rec(name, buffer, target_str, depth-1);
                exit(0);
            }
        } else if (S_ISREG(buffer->st_mode)) {
            print_search_result(name, buffer, target_str);
        }
    }

}

void browse_directory (const char* dir_name, const char* target_str, int max_depth) {
    struct stat buffer;
    print_header(target_str);
    browse_rec(dir_name, &buffer, target_str, max_depth);
}


int main(int argc, char** argv) {

    if (argc < 2) {
        printf("M8 you have to add at least one parameter");
        exit(1);
    }

    const char* dir_name = argv[1];
    const char* target_str = argv[2];
    int max_depth = atoi(argv[3]);

    browse_directory(dir_name, target_str, max_depth);

    return 0;
}
