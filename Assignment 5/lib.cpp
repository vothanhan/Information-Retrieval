#include "lib.h"

using namespace std;

int num_of_doc;
int word_per_portions=500000;
priority_queue <pair<Word_Doc,int>, vector<pair<Word_Doc,int> >, greater<pair<Word_Doc, int> > > q;
pair<FILE *,int> *portions;
int doc_count=0;
vector<int>ground_truth_value;
map<int,double> idfs;
map<int, string> doc_id;
map<string,int>classes;
map<int,vector<int> >classes_list;
vector<int> class_file;
map<string,int> index_word;
vector<string> stopword;
bool double_int_comp(pair<double,int> i, pair<double,int> j);
int wordcount=0;
vector<pair<int,int> > word_memory;

vector<string> openfile(string path) {

    DIR*    dir;
    dirent* pdir;
    vector<string> files;

    dir = opendir(path.c_str());

    while (pdir = readdir(dir)) {
        files.push_back(pdir->d_name);
    }
    return files;
}


void read_next_word_tag(int i)
{
    if(portions[i].second!=0)
    {
        Word_Doc temp;
        fread(&temp,sizeof(Word_Doc),1,portions[i].first);
        portions[i].second--;
        if(temp.term_id==0)
        {
            int j=0;
        }
        q.push({temp,i});
    }
    else
    {
        fclose(portions[i].first);
    }
}


void initiate(string path)
{
    int k=0;
    int j=0;
    int wordcount=0;
    int coun=0;
    ofstream FileRaw;
    ofstream Term_ID;
    FileRaw.open("D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Raw.txt",ios::binary);
    Term_ID.open("D:\\Academic Files\\CS\\CS419\\Assignment 5\\Term_ID.txt");
    string p;
    vector<string> paths=openfile(path);
    for(int i=2;i<paths.size();++i)
    {
        int classID=i-2;
        p=path+"\\"+paths[i];
        vector<string> files=openfile(p);
        classes.insert(pair<string,int>(paths[i],classID));
        for(int b=2;b<files.size();++b)
        {
            doc_id.insert(pair<int,string>(doc_count,files[i]));
            class_file.push_back(classID);
            if(classes_list.count(classID)==0)
            {
                vector<int> tmp;
                tmp.push_back(doc_count);
                classes_list.insert(pair<int,vector<int> >(classID,tmp));
            }
            else
            {
                classes_list[classID].push_back(doc_count);
            }
            ++doc_count;
            string filepath=p+"\\"+files[b];
            fstream f;
            f.open(filepath.c_str());
            string word;
            word.clear();
            char c;
            while(!f.eof())
            {
                c=f.get();
                while((isalpha(c))&& c!=ifstream::traits_type::eof())
                {
                    c=tolower(c);
                    word+=c;
                    c=f.get();
                }
                if (word.length()!=0)
                {
                    if(find(stopword.begin(),stopword.end(),word)==stopword.end()) //2.1 Remove Stopword
                    {

                        if(wordcount>=word_per_portions*(1+k))
                        {
                            for(int l=0+word_per_portions*k;l<word_per_portions*(k+1);++l)
                            {
                                ++coun;
                                FileRaw.write((char*)&word_memory[l].first,sizeof(int));
                                FileRaw.write((char*)&word_memory[l].second,sizeof(int));
                            }
                            ++k;
                        }
                        ++wordcount;
                        if(index_word.count(word)==0)
                        {
                            index_word.insert(pair<string,int>(word,j));
                            ++j;
                        }
                        word_memory.push_back(pair<int,int>(index_word.find(word)->second,doc_count));
                    }
                }
                word.clear();
            }
            f.close();
            cout<<"READING..."<<files[b]<<endl;
        }
    }
    if(wordcount<word_per_portions*(1+k))
        {
            for(int i=word_per_portions*k;i<wordcount;++i)
            {
                ++coun;
                FileRaw.write((char*)&word_memory[i].first,sizeof(int));
                FileRaw.write((char*)&word_memory[i].second,sizeof(int));
            }
        }
        FileRaw.close();
    map<string,int>::iterator IndexIterator;
    for(IndexIterator=index_word.begin();IndexIterator!=index_word.end();++IndexIterator)
    {
        Term_ID<<IndexIterator->first<<" "<<IndexIterator->second<<endl;
    }
    Term_ID.close();
}


void sort_portion(FILE* &File_Raw,FILE* &File_Sort,int num_terms)
{
    Word_Doc *term=new Word_Doc[num_terms];
    fread(term,sizeof(Word_Doc),num_terms,File_Raw);
    for(int i=0;i<num_terms;++i)
    {
        cout<<term[i].term_id<<" "<<term[i].doc_id<<endl;
    }
    sort(term,term+num_terms);
    fwrite(term,sizeof(Word_Doc),num_terms,File_Sort);

    delete term;
}


void sort_and_merge()
{
    string sort_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Sort.txt";
    string raw_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Raw.txt";
    FILE *File_Raw=fopen(raw_path.c_str(),"rb");
    int n_portions=10;
    fseek(File_Raw,0,SEEK_END);
    int file_size=ftell(File_Raw);
    int n_words=file_size/sizeof(Word_Doc);
    int terms_per_portion=(n_words-1)/n_portions-1;

    fseek(File_Raw,0,SEEK_END);
    fclose(File_Raw);

    vector<pair<int,int> > Word_Sorted;
    int word_id;
    int docname;
    int k=1;
    int j=0;
    vector<pair<int,int> >::iterator temp;
    ifstream File_Raw_O;
    File_Raw_O.open("D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Raw.txt",ios::binary|ios::in);
    ofstream FileSort;
    FileSort.open("D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Sort.txt",ios::binary);
    File_Raw_O.read((char*)&word_id,sizeof(int));
    //int bufferpos=1;
    int written=0;
    while(j<n_words)
    {
        File_Raw_O.read((char*)&docname,sizeof(int));
        if(j>=k*terms_per_portion)
        {
            if(k==1)
            {
                temp=Word_Sorted.begin();
            }
            else
            {
                temp=Word_Sorted.begin()+(k-1)*terms_per_portion;
            }
            sort(temp,Word_Sorted.end());
            for(int i=0;i<terms_per_portion;++i)
            {
                FileSort.write((char*)&Word_Sorted[i+(k-1)*terms_per_portion].first,sizeof(int));
                FileSort.write((char*)&Word_Sorted[i+(k-1)*terms_per_portion].second,sizeof(int));
                ++written;
            }
            ++k;
        }
        ++j;
        Word_Sorted.push_back(pair<int,int>(word_id,docname));
        File_Raw_O.read((char*)&word_id,sizeof(int));
    }
    if(written<n_words)
    {
        temp=Word_Sorted.begin()+(k-1)*terms_per_portion;
        sort(temp,Word_Sorted.end());
        for(int i=0;i<n_words-written;++i)
            {
                FileSort.write((char*)&Word_Sorted[i+(k-1)*terms_per_portion].first,sizeof(int));
                FileSort.write((char*)&Word_Sorted[i+(k-1)*terms_per_portion].second,sizeof(int));
            }
    }
    File_Raw_O.close();
    FileSort.close();
    ////////////////////////////////////////
    //Merging
    ////////////////////////////////////////
    string index_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Index.txt";
    FILE *File_Index = fopen(index_path.c_str(),"wb");
    portions = new pair <FILE *,int>[n_portions];

    for(int portions_i=0,remain_words=n_words;portions_i<n_portions;++portions_i)
    {
        portions[portions_i]={fopen(sort_path.c_str(),"rb"),min(remain_words,terms_per_portion)};
        fseek(portions[portions_i].first,portions_i*terms_per_portion*sizeof(Word_Doc),SEEK_SET);
        remain_words-=terms_per_portion;
    }
    while(!q.empty())
        q.pop();
    for(int i=0;i<n_portions;i++)
    {
        read_next_word_tag(i);
    }

    Word_Doc prev(-1,-1);
    int f=0;
    while(!q.empty())
    {
        pair<Word_Doc,int> curr=q.top();
        q.pop();
        read_next_word_tag(curr.second);
        if(prev != curr.first)
        {
            if(f!=0)
            {
                fwrite(&prev,sizeof(Word_Doc),1,File_Index);
                fwrite(&f,sizeof(int),1,File_Index);
            }
            prev=curr.first;
            f=1;
        }
        else
            ++f;
    }
    if (f!=0)
    {
        fwrite(&prev,sizeof(Word_Doc),1,File_Index);
        fwrite(&f,sizeof(int),1,File_Index);
    }
    delete portions;

    fclose(File_Index);
    return;
}

void get_idf()
{
    string index_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Index.txt";
    FILE *File_Index = fopen(index_path.c_str(),"rb");
    int word_id=0;
    int doc_num=doc_id.size();
    int doc_id=0;
    int prev_word_id=-1;
    int reverse_index=0;
    int f=0;
    int i=0;
    int term_num=word_memory.size();
    while(i<term_num)
    {
        fread(&word_id,sizeof(int),1,File_Index);
        fread(&doc_id,sizeof(int),1,File_Index);
        fread(&f,sizeof(int),1,File_Index);
        if(prev_word_id!=word_id)
        {
            if(prev_word_id!=-1)
            {
                idfs[prev_word_id]=log((double)doc_num/(double)reverse_index);
            }
            reverse_index=0;
            prev_word_id=word_id;
        }
        ++reverse_index;
        ++i;
    }
    idfs[prev_word_id]=log((double)doc_num/(double)reverse_index);
}

void get_stop_word()
{
    string path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\stopwords_en.txt";
    fstream file;
    file.open(path.c_str());
    string word;
    while (file>>word)
    {
        stopword.push_back(word);
    }
    return;
}
vector<pair<double,int> >query (string file_query)
{
    string path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\Query\\";
    int doc_num=doc_id.size();
    int term_num=index_word.size();
    vector<double> numerator(term_num,0);
    vector<double> denominator(term_num,0);
    int freq[term_num];
    memset(freq,0,sizeof(freq));

    path+=file_query;
    fstream fin;
    fin.open(path.c_str());
    char c;
    string word;
    word.clear();
    while(!fin.eof())
    {
        c=fin.get();
        while((isalpha(c))&& c!=ifstream::traits_type::eof())
        {
            c=tolower(c);
            word+=c;
            c=fin.get();
        }
        if (word.length()!=0)
        {
            if(find(stopword.begin(),stopword.end(),word)==stopword.end()) //2.1 Remove Stopword
            {
                if(index_word.count(word)!=0)
                {
                    freq[index_word[word]]++;
                }

            }
        }
        word.clear();
    }
    fin.close();

    vector<double> query_tf_idf(term_num,0);
    double query_norm=0;
    for(int i=0;i<term_num;++i)
    {
        double tf=0;
        if(freq[i]>0)
        {
            tf=1+log(freq[i]);
        }
        query_tf_idf[i]=tf*idfs[i];
    }
    FILE* file;
    string file_index="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Index.txt";
    file=fopen(file_index.c_str(),"rb");

    for(Word_Doc i;fread(&i,sizeof(Word_Doc),1,file);)
    {
        int frequ;
        fread(&frequ,sizeof(int),1,file);
        double tf=0;
        if(frequ>0)
        {
            tf=1+log(frequ);
        }
        numerator[i.doc_id]+=tf*idfs[i.term_id]*query_tf_idf[i.term_id];
        denominator[i.doc_id]+=pow(tf*idfs[i.term_id],2);
    }
    fclose(file);
    for(int i=0;i<doc_num;++i)
    {
        denominator[i]=sqrt(denominator[i]);
    }
    vector<pair<double,int> >temp;
    for(int i=0;i<doc_num;++i)
    {
        temp.push_back(pair<double,int>((numerator[i]/denominator[i]),i));
    }
    sort(temp.rbegin(),temp.rend());
    return temp;
}
bool double_int_comp(pair<double,int> i,pair<double,int> j)
{
    return i.first<j.first;
}
void ground_truth()
{
    string q_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\Query";
    vector<string> qs=openfile(q_path);

    int query_num=qs.size()-2;
    for(int i=2;i<qs.size();++i)
    {
        int class_id=classes[qs[i]];
        vector<pair<double,int> > rel=query(qs[i]);
        int relevance=0;
        for(int j=0;j<rel.size();++j)
        {
            if(class_file[rel[j].second]==class_id)
            {
                if(rel[j].first>0)
                    relevance++;
            }
        }
        ground_truth_value.push_back(relevance);
        cout<<"Process ground truth for "<<qs[i]<<endl;
    }
}
void process_query()
{
    string q_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\Query";
    vector<string> qs=openfile(q_path);
    ground_truth();
    double MAP=0;
    int query_num=qs.size()-2;
    for(int i=2;i<qs.size();++i)
    {
        int class_id=classes[qs[i]];
        vector<pair<double,int> > rel=query(qs[i]);
        double AP=0;
        string PR_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\PR\\"+qs[i]+".txt";
        string F_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\F\\"+qs[i]+".txt";
        ofstream PR;
        ofstream PR1;
        ofstream F;
        PR.open(PR_path.c_str());
        F.open(F_path.c_str());
        int correct_num=0,predict_num=0,total=classes_list[classes[qs[i]]].size(),relevance=0;
        double prev_precision=1,prev_recall=0,precision,recall;
        cout<<"Process query "<<qs[i]<<endl;
        PR<<prev_recall<<" "<<prev_precision<<endl;
        for(int j=0;j<rel.size();++j)
        {
            ++predict_num;
            if(class_file[rel[j].second]==class_id)
            {
                if(rel[j].first>0)
                    relevance++;
                ++correct_num;
                recall=double(relevance)/ground_truth_value[i-2];
                precision=double(relevance)/predict_num;

                double F_measure=2*precision*recall/(precision+recall);

                PR<<recall<<" "<<precision<<endl;
                F<<predict_num<<" "<<F_measure<<endl;
            }
            AP+=(recall-prev_recall)*(precision);
            prev_recall=recall;
            prev_precision=precision;
        }
        PR.close();
        F.close();
        MAP+=AP;
    }
    string path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\MAP.txt";
    ofstream fout;
    fout.open(path.c_str());
    fout<<MAP/query_num;
    fout.close();
}
