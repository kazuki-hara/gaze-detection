#include <string>
#include <iostream>
#include <iomanip>
#include <dirent.h>
#include <typeinfo>
#include "utils.h"

int count_files_num(std::string dir_name){
    DIR* dir;       // ディレクトリへのポインタ
    struct dirent* entry; // readdir() で返されるエントリーポイント

    dir = opendir(dir_name.c_str());
    if (dir==NULL) return -1;
    else{
        int count = 0;
        do {
            entry = readdir(dir);
            if (entry != NULL) count+=1;
        } while (entry != NULL);
        return count - 2; // /. と /..を除く
    }
}

void print_file_list(std::string dir_name){
    const char* path = "/share/home/hara/Data/fove/gaze/old_data/20211026/02/image/frame/";
    DIR* dir;       // ディレクトリへのポインタ
    struct dirent* entry; // readdir() で返されるエントリーポイント

    dir = opendir(path);
    if (dir==NULL) return;
    do {
        entry = readdir(dir);
        if (entry != NULL) printf("%s\n", entry->d_name);
    } while (entry != NULL);
}

bool check_dir_exist(const char* dir_name){
    DIR* dir;
    dir = opendir(dir_name);
    if(dir == NULL) return false;
    else return true;
}

std::string get_data_string(void){
    time_t t = time(nullptr);
    const tm* local_time = localtime(&t);
    std::stringstream s;
    s << local_time -> tm_year + 1900;
    s << std::setw(2) << std::setfill('0') << local_time->tm_mon + 1;
    s << std::setw(2) << std::setfill('0') << local_time->tm_mday;
    return s.str();
}

void get_year_month_day(void){
    
}