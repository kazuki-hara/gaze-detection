#include <string>

int check_dir_exist(std::string);

class FileOperator{
    public:
        FileOperator(void);
        ~FileOperator(void);

        bool check_dir_exist(std::string);
        int count_files_num(std::string);
    private:
};

int count_files_num(std::string);