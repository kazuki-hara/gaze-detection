#include <string>
#include <iostream>
#include <dirent.h>
#include <typeinfo>
#include "utils.h"


int count_files_num(const char* dir_name){
    DIR* dir;       // ディレクトリへのポインタ
    struct dirent* entry; // readdir() で返されるエントリーポイント

    dir = opendir(dir_name);
    if (dir==NULL) return -1;
    else{
        int count = 0;
        do {
            entry = readdir(dir);
            if (entry != NULL) count+=1;
        } while (entry != NULL);
        return count - 2;
    }
}

void print_file_list(std::string dir_name){
    const char* path = "/share/home/hara/Data/fove/gaze/old_data/20211026/02/image/frame/";
    DIR* dir;       // ディレクトリへのポインタ
    struct dirent* entry; // readdir() で返されるエントリーポイント

    dir = opendir(path);
    if (dir==NULL) exit(1);
    do {
        entry = readdir(dir);
        if (entry != NULL) printf("%s\n", entry->d_name);
    } while (entry != NULL);
}