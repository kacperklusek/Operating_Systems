#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

char* join_path(char* path1, char* path2) {
  char* path = malloc(sizeof(char) * (strlen(path1) + strlen(path2)) + 2);
  sprintf(path, "%s/%s", path1, path2);
  return path;
}

void scan_dir(char* path) {
  if (!path) return;
  DIR* dir = opendir(path);
  if (!dir) return; // can not read

  struct dirent* d;
  struct stat s;
  while ((d = readdir(dir))) {
    if (strcmp(d->d_name, "..") == 0) continue;
    if (strcmp(d->d_name, ".") == 0) continue;

    char* dir = join_path(path, d->d_name);
    if (lstat(dir, &s) < 0) continue; // can not read
    if (S_ISDIR(s.st_mode) && fork() == 0) {
      printf("\npid(%i) path(%s)\n", getpid(), dir);
      execlp("ls", "ls", dir, "-l", NULL);
    } else wait(NULL); // wait for preatier printing

    free(dir);
  }
  closedir(dir);
}

int main(int argc, char** argv) {
  char* search_path = argc > 1 ? argv[1] : ".";
  scan_dir(search_path);
  return 0;
}