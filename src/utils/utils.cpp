#include <string>
#include <iostream>
#include <filesystem>
#include "utils.h"
#define DATA_DIR "/share/home/hara/Data/"


void check_dir_exist(std::string dir_name){
    const char* path = "/share/home/hara/Data/fove/gaze/old_data/20211026/02/image/frame/";
    DIR *dp;       // ディレクトリへのポインタ
    dirent* entry; // readdir() で返されるエントリーポイント

    dp = opendir(path);
    if (dp==NULL) exit(1);
    do {
        entry = readdir(dp);
        if (entry != NULL)
        std::cout << std::typeid(entry->d_name) << std::endl;
    } while (entry != NULL);
}