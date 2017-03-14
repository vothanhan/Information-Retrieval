#include <stdio.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <list>
#include <map>
#include <queue>
#include <iterator>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <time.h>

using namespace std;

struct Word_Doc{
    int term_id,doc_id;
    Word_Doc(int _term_id=0,int _doc_id=0):term_id(_term_id),doc_id(_doc_id){}
    friend bool operator != (const Word_Doc &x, const Word_Doc &y)
    {
        return make_pair(x.term_id,x.doc_id) != make_pair(y.term_id,y.doc_id);
    }
    friend bool operator < (const Word_Doc &x, const Word_Doc &y)
    {
        return make_pair(x.term_id,x.doc_id) < make_pair(y.term_id,y.doc_id);
    }
    friend bool operator <= (const Word_Doc &x, const Word_Doc &y)
    {
        return make_pair(x.term_id,x.doc_id) <= make_pair(y.term_id,y.doc_id);
    }
};
int write_block_to_file(FILE* &FileSort);
vector<string> openfile(string path);
void initiate(string path,int train_num);
void get_stop_word();
void read_next_word_tag(int i);
int write_block_to_file(FILE* &FileSort);
void sort_and_merge();
void process_query();
void get_idf();
void process_query_bayes();

