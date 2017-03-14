#include "lib.h"

using namespace std;

int num_of_doc;
int word_per_portions=50000;
priority_queue <pair<Word_Doc,int>, vector<pair<Word_Doc,int> >, greater<pair<Word_Doc, int> > > q;
pair<FILE *,int> *portions;
int doc_count=0;
vector<string> test_set;
vector<string> test_set_name;
vector<int> class_start;
int train_set_size=0;
vector<int> real_class_test;
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

map<int,string> index_word2000;

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


void initiate(string path,int train_num)
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
    train_set_size=train_num*20;
    for(int i=2;i<paths.size();++i)
    {
        int classID=i-2;
        p=path+"\\"+paths[i];
        vector<string> files=openfile(p);
        vector<string> tmp_doc=files;
        vector<string> train_set;
        //if(train_num==1000/20)
        //{
            for(int m=0;m<250;++m)
            {
                srand(156789);
                int num=rand()%tmp_doc.size()+2;
                test_set.push_back(p+"\\"+files[num]);
                test_set_name.push_back(files[num]);
                real_class_test.push_back(classID);
                tmp_doc.erase(tmp_doc.begin()+num);
            }
        //}
        int doc_num=min(train_num,int(files.size()));
        for(int m=0;m<doc_num;++m)
        {
            //srand(time(NULL));
            //int num=rand()%tmp_doc.size()+2;
            train_set.push_back(files[m+2]);
            tmp_doc.erase(tmp_doc.begin()+m+2);
        }
        classes.insert(pair<string,int>(paths[i],classID));
        for(int b=2;b<files.size();++b)
        {
            if (find(train_set.begin(),train_set.end(),files[b])==train_set.end())
            {
                continue;
            }
            doc_id.insert(pair<int,string>(doc_count,files[b]));
            class_file.push_back(classID);
            if(classes_list.count(classID)==0)
            {
                vector<int> tmp;
                tmp.push_back(doc_count);
                class_start.push_back(doc_count);
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
    map<string,int> tmp=index_word;
    //if(train_num==1000/20)
    //{
        for (int i=0;i<2000;++i)
        {
            map<string,int>::iterator base=tmp.begin();
            srand(5454646);
            int index=rand()%tmp.size();
            advance(base,index);
            index_word2000.insert(pair<int,string>(base->second,base->first));
            tmp.erase(base);
        }
    //}

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
    string index_path2="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Index2.txt";
    FILE *File_Index = fopen(index_path.c_str(),"wb");
    portions = new pair <FILE *,int>[n_portions];
    ofstream fout;
    fout.open(index_path2.c_str());
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
        if(index_word2000.count(curr.first.term_id)>0)
        {
            if(prev != curr.first)
            {
                if(f!=0)
                {
                    fwrite(&prev,sizeof(Word_Doc),1,File_Index);
                    fwrite(&f,sizeof(int),1,File_Index);
                    fout<<prev.term_id<<" "<<prev.doc_id<<" "<<f<<endl;
                }
                prev=curr.first;
                f=1;
            }
            else
                ++f;

        }
    }
    if (f!=0)
    {
        fwrite(&prev,sizeof(Word_Doc),1,File_Index);
        fwrite(&f,sizeof(int),1,File_Index);
    }
    delete portions;
    fout.close();
    fclose(File_Index);
    return;
}

void get_idf()
{
    string index_path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Index.txt";
    FILE *File_Index = fopen(index_path.c_str(),"rb");
    int n_portions=10;
    fseek(File_Index,0,SEEK_END);
    int file_size=ftell(File_Index);
    int n_words=file_size/(3*sizeof(int));

    fseek(File_Index,0,SEEK_END);
    fclose(File_Index);
    File_Index = fopen(index_path.c_str(),"rb");
    int word_id=0;
    int doc_num=doc_id.size();
    int doc_id=0;
    int prev_word_id=-1;
    int reverse_index=0;
    int f=0;
    int i=0;
    int term_num=n_words;
    while(i<term_num)
    {
        fread(&word_id,sizeof(int),1,File_Index);
        fread(&doc_id,sizeof(int),1,File_Index);
        fread(&f,sizeof(int),1,File_Index);
        if(prev_word_id!=word_id)
        {
            if(prev_word_id!=-1)
            {
                idfs[prev_word_id]=1.0+log((double)doc_num/(double)reverse_index);
            }
            reverse_index=0;
            prev_word_id=word_id;
        }
        ++reverse_index;
        ++i;
    }
    idfs[prev_word_id]=1+log((double)doc_num/(double)reverse_index);
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
    int doc_num=doc_id.size();
    int term_num=index_word.size();
    vector<double> numerator(term_num,0);
    vector<double> denominator(term_num,0);
    int freq[term_num];
    memset(freq,0,sizeof(freq));

    fstream fin;
    fin.open(file_query.c_str());
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
                    if(index_word2000.count(index_word[word])!=0)
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
        query_norm+=query_tf_idf[i]*query_tf_idf[i];
    }
    query_norm=sqrt(query_norm);
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
        denominator[i]=sqrt(denominator[i])*query_norm;
    }
    vector<pair<double,int> >temp;
    double angle=0.0;
    for(int i=0;i<doc_num;++i)
    {
        if(denominator[i]==0)
            angle=0.0;
        else
            angle=numerator[i]/denominator[i];
        temp.push_back(pair<double,int>((angle),i));
    }
    sort(temp.rbegin(),temp.rend());
    return temp;
}
bool double_int_comp(pair<double,int> i,pair<double,int> j)
{
    return i.first<j.first;
}

void trash_collector()
{
    int num_of_doc=0;
    idfs.clear();
    doc_id.clear();
    classes.clear();
    classes_list.clear();

    class_file.clear();
    index_word.clear();
    word_memory.clear();
}
void process_query_bayes()
{
    map<int,int> word_count_class;
    map<int,double> class_prob;
    map<int,map<int,int> > word_occ_per_class;
    vector<int> word_list;
    map<int,map<int,double> > word_prob;
    FILE* file;
    string file_index="D:\\Academic Files\\CS\\CS419\\Assignment 5\\File_Index.txt";
    file=fopen(file_index.c_str(),"rb");
    for(int i=0;i<classes.size();++i)
    {
        if (i<classes.size()-1)
        {
            class_prob[i]=double(class_start[i+1]-class_start[i])/double(train_set_size);
        }
        else
            class_prob[i]=(train_set_size-class_start[i])/train_set_size;
    }
    for(Word_Doc i;fread(&i,sizeof(Word_Doc),1,file);)
    {
        if(find(word_list.begin(),word_list.end(),i.term_id)==word_list.end())
        {
            word_list.push_back(i.term_id);
        }
        int frequ;
        int class_id=-1;
        fread(&frequ,sizeof(int),1,file);
        for(int j=0;j<class_start.size();++j)
        {
            if(i.doc_id<class_start[j])
                class_id=j;
        }
        if(word_occ_per_class.count(i.term_id)==0)
        {
            for(int j=0;j<classes.size();++j)
            {
                word_occ_per_class[i.term_id][j]=0;
            }
        }
        if(word_count_class.count(class_id)==0)
        {
            word_count_class[class_id]=0;
        }
        word_occ_per_class[i.term_id][class_id]+=frequ;
        word_count_class[class_id]+=frequ;
    }
    for(int i=0;i<word_occ_per_class.size();++i)
    {
        for(int j=0;j<classes.size();++j)
        {
            double nume=(word_occ_per_class[word_list[i]][j]+1);
            double deno=(word_count_class[j]+2000);
            word_prob[word_list[i]][j]=nume/deno;
        }
    }
    int true_guess=0;
    for(int i=0;i<test_set.size();++i)
    {
        fstream fin;
        vector<double> prob_be_class(classes.size(),0.0);
        for(int k=0;k<classes.size();++k)
        {
            prob_be_class[k]+=log(class_prob[k]);
        }
        fin.open(test_set[i].c_str());
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
                        if(index_word2000.count(index_word[word])!=0)
                            for(int k=0;k<classes.size();++k)
                            {
                                prob_be_class[k]+=log(word_prob[index_word[word]][k]);
                            }
                    }
                }
            }
            word.clear();
        }
        fin.close();
        int max_count=prob_be_class[0];
        int guess_class=0;
        for(int k=0;k<classes.size();++k)
        {
            if(max_count<prob_be_class[k])
            {
                max_count=prob_be_class[k];
                guess_class=k;
            }
        }

        if(guess_class==real_class_test[i])
            ++true_guess;
    }
    double precision=double(true_guess)/5000.0;
    cout<<precision<<endl;
    string path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\Precision_Bayes.txt";
    ofstream fout;
    fout.open(path.c_str(),ios::app);
    fout<<precision<<" "<<train_set_size<<endl;
    fout.close();
}
void process_query()
{
    int query_num=test_set.size();
    int true_guess=0;
    int k_nearest_class [classes.size()];
    for(int i=0;i<test_set.size();++i)
    {
        vector<pair<double,int> > rel=query(test_set[i]);
        cout<<"Process query "<<i<<endl;
        for (int j=0;j<classes.size();++j)
        {
            k_nearest_class[j]=0;
        }
        for(int j=0;j<49;++j)//K-nearest neighbors. K is 7
        {
            for(int k=0;k<classes_list.size();++k)
            {
                if(find(classes_list[k].begin(),classes_list[k].end(),rel[j].second)!=classes_list[k].end())
                {
                    k_nearest_class[k]++;
                }
            }
        }
        int max_class=0;
        int guess_class=-1;
        for (int j=0;j<classes.size();++j)
        {
            if(max_class<k_nearest_class[j])
            {
                max_class=k_nearest_class[j];
                guess_class=j;
            }
        }
        cout<<guess_class<<" "<<real_class_test[i]<<" "<<max_class<<endl;
        if(guess_class==real_class_test[i])
        {
            ++true_guess;
        }
    }
    float precision=float(true_guess)/float(test_set.size());
    cout<<precision<<" "<<true_guess<<" "<<test_set.size()<<endl;
    string path="D:\\Academic Files\\CS\\CS419\\Assignment 5\\Precision.txt";
    ofstream fout;
    fout.open(path.c_str(),ios::app);
    fout<<precision<<" "<<train_set_size<<endl;
    fout.close();
}

