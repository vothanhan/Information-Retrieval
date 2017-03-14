#include "lib.h"

using namespace std;

int main()
{
    string path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\20_newsgroups";
    get_stop_word();
    int train_num=3000;
    cout<<"Train set: "<<train_num<<" documents"<<endl;
    initiate(path,train_num/20);
    sort_and_merge();
    get_idf();
    process_query_bayes();
    process_query();
}
