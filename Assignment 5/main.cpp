#include "lib.h"

using namespace std;

int main()
{
    string path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\20_newsgroups";
    get_stop_word();
    initiate(path);
    sort_and_merge();
    get_idf();
    process_query();
}
