#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>

#include <sstream>
#include <typeinfo>

#include <vector>
#include <utility>
#include <functional>
#include <algorithm>
#include <bitset>
#include <time.h>

#define SAMPLEDSIZE 9
using namespace std;

struct CSVDATA
{
    string Name;
    string Country;
    string Gender;
};

struct EXTRACTED_DATA
{
    vector<string> Names;
    vector<string> Countries;
    vector<string> Genders;
};

vector<CSVDATA> csvData_original, csvData_compare;
vector<CSVDATA> csvData_original_hash, csvData_compare_hash;
EXTRACTED_DATA HLL_IND_candidate_compare, HLL_IND_candidate_original;

/*读取csv，原值和hash存入不同容器*/
/*original 原；original hash；compare 原；compare hash；*/
pair<vector<CSVDATA>, vector<CSVDATA> > read_csv(string filepath,
                                                 vector<CSVDATA> csvData,
                                                 vector<CSVDATA> csvData_hash)
{
    pair<vector<CSVDATA>, vector<CSVDATA> > p1;
    hash<string> hash_obj;
    ifstream inFile(filepath, ios::in);
    string lineStr;
    getline(inFile, lineStr);
    while (getline(inFile, lineStr))
    {
        stringstream ss(lineStr);
        string str;
        CSVDATA csvdata;
        CSVDATA csvdata_hash;
        getline(ss, str, ',');
        csvdata.Name = str;
        stringstream name;
        name << hash_obj(str);
        // cout << typeid(hash_obj(str)).name() << endl;
        csvdata_hash.Name = name.str();
        getline(ss, str, ',');
        csvdata.Country = str;
        stringstream country;
        country << hash_obj(str);
        csvdata_hash.Country = country.str();
        getline(ss, str, ',');
        csvdata.Gender = str;
        stringstream gender;
        gender << hash_obj(str);
        csvdata_hash.Gender = gender.str();
        csvData.push_back(csvdata);
        csvData_hash.push_back(csvdata_hash);
    }
    return make_pair(csvData, csvData_hash);
}

/*打印 read_csv()*/
void print_csv(vector<struct CSVDATA> csvData)
{
    for (int i = 0; i < csvData.size(); i++)
    {
        cout << csvData[i].Name << "," << csvData[i].Country << "," << csvData[i].Gender << endl;
    }
    cout << endl;
}

string StrToBitStr(string str)
{
    str = str.substr(0, 19);
    // cout << str << endl;
    long binary;
    binary = atol(str.c_str());

    // cout << binary << endl;

    string r;
    while (binary != 0)
    {
        r += (binary % 2 == 0 ? "0" : "1");
        binary /= 2;
    }
    reverse(begin(r), end(r));
    // cout << r << endl;
    string s = r.substr(0, 4);
    // cout << s << endl;
    //  cout << count << endl;
    //  string hll = s.substr(0, 1) + to_string(count);
    //  cout << hll << endl;
    //  cout << endl;
    return s;
}

/*三个单词的binary各取前四位连起来得到每个tuple的binary string
对比compare和original，找出相同tuple*/
pair<EXTRACTED_DATA, EXTRACTED_DATA> HLL(vector<CSVDATA> csvData_compare_hash, vector<CSVDATA> csvData_original_hash, vector<CSVDATA> csvData_compare, vector<CSVDATA> csvData_original)
{
    EXTRACTED_DATA csvData_compare_hash_HLL;
    EXTRACTED_DATA csvData_original_hash_HLL;
    vector<string> hll_compare;
    vector<string> hll_original;
    vector<int> compare_record_num;
    vector<int> original_record_num;

    for (int i = 0; i < csvData_compare_hash.size(); i++)
    {
        csvData_compare_hash_HLL.Names.push_back(StrToBitStr(csvData_compare_hash[i].Name));
        csvData_compare_hash_HLL.Countries.push_back(StrToBitStr(csvData_compare_hash[i].Country));
        csvData_compare_hash_HLL.Genders.push_back(StrToBitStr(csvData_compare_hash[i].Gender));
    }
    for (int i = 0; i < csvData_original_hash.size(); i++)
    {
        csvData_original_hash_HLL.Names.push_back(StrToBitStr(csvData_original_hash[i].Name));
        csvData_original_hash_HLL.Countries.push_back(StrToBitStr(csvData_original_hash[i].Country));
        csvData_original_hash_HLL.Genders.push_back(StrToBitStr(csvData_original_hash[i].Gender));
    }
    for (int i = 0; i < csvData_compare_hash_HLL.Names.size(); i++)
    {
        hll_compare.push_back(csvData_compare_hash_HLL.Names[i] + csvData_compare_hash_HLL.Countries[i] + csvData_compare_hash_HLL.Genders[i]);
        // cout << csvData_compare_hash_HLL.Names[i] << "," << csvData_compare_hash_HLL.Countries[i] << "," << csvData_compare_hash_HLL.Genders[i] << endl;
        // cout << hll_compare[i] << endl;
    }
    cout << endl;
    for (int j = 0; j < csvData_original_hash_HLL.Names.size(); j++)
    {
        hll_original.push_back(csvData_original_hash_HLL.Names[j] + csvData_original_hash_HLL.Countries[j] + csvData_original_hash_HLL.Genders[j]);
        // cout << csvData_compare_hash_HLL.Names[i] << "," << csvData_compare_hash_HLL.Countries[i] << "," << csvData_compare_hash_HLL.Genders[i] << endl;
        // cout << hll_original[j] << endl;
    }
    cout << endl;
    int i;
    for (int i = 0; i < hll_compare.size(); i++)
    {
        for (int j = 0; j < hll_original.size(); j++)
        {
            if (hll_compare[i] == hll_original[j])
            {
                // cout << hll_compare[i] << endl;
                // cout << i << endl;
                if (find(compare_record_num.begin(), compare_record_num.end(), i) == compare_record_num.end())
                {
                    compare_record_num.push_back(i);
                }
                if (find(original_record_num.begin(), original_record_num.end(), j) == original_record_num.end())
                {
                    original_record_num.push_back(j);
                }
            }
        }
    }
    for (int i = 0; i < compare_record_num.size(); i++)
    {
        // cout << compare_record_num[i] << endl;
        //  cout << csvData_compare[compare_record_num[i]].Name << csvData_compare[compare_record_num[i]].Country << csvData_compare[compare_record_num[i]].Gender << endl;
        HLL_IND_candidate_compare.Names.push_back(csvData_compare[compare_record_num[i]].Name);
        HLL_IND_candidate_compare.Countries.push_back(csvData_compare[compare_record_num[i]].Country);
        HLL_IND_candidate_compare.Genders.push_back(csvData_compare[compare_record_num[i]].Gender);
    }
    // cout << HLL_IND_candidate_compare.Names[i] << "," << HLL_IND_candidate_compare.Countries[i] << "," << HLL_IND_candidate_compare.Genders[i] << endl;

    for (int i = 0; i < original_record_num.size(); i++)
    {
        // cout << original_record_num[i] << endl;
        //  cout << csvData_compare[compare_record_num[i]].Name << csvData_compare[compare_record_num[i]].Country << csvData_compare[compare_record_num[i]].Gender << endl;
        HLL_IND_candidate_original.Names.push_back(csvData_original[original_record_num[i]].Name);
        HLL_IND_candidate_original.Countries.push_back(csvData_original[original_record_num[i]].Country);
        HLL_IND_candidate_original.Genders.push_back(csvData_original[original_record_num[i]].Gender);
    }
    // for (int i = 0; i < HLL_IND_candidate_original.Names.size(); i++)
    // {
    //     cout << HLL_IND_candidate_original.Names[i] << HLL_IND_candidate_original.Countries[i] << HLL_IND_candidate_original.Genders[i] << endl;
    // }

    return make_pair(HLL_IND_candidate_compare, HLL_IND_candidate_original);
}

/*打印 IND and candidate*/
void print_IND(EXTRACTED_DATA IND)
{
    for (int i = 0; i < IND.Names.size(); i++)
    {
        cout << IND.Names[i] << "," << IND.Countries[i] << "," << IND.Genders[i] << endl;
    }
    cout << endl;
}

int main(void)
{
    string filepath1 = "csv/originaltable1.csv";
    string filepath2 = "csv/compare1.csv";

    csvData_original = read_csv(filepath1, csvData_original, csvData_original_hash).first;
    csvData_original_hash = read_csv(filepath1, csvData_original, csvData_original_hash).second;
    csvData_compare = read_csv(filepath2, csvData_compare, csvData_compare_hash).first;
    csvData_compare_hash = read_csv(filepath2, csvData_compare, csvData_compare_hash).second;

    print_csv(csvData_compare);
    print_csv(csvData_compare_hash);

    HLL_IND_candidate_compare = HLL(csvData_compare_hash, csvData_original_hash, csvData_compare, csvData_original).first;
    HLL_IND_candidate_original = HLL(csvData_compare_hash, csvData_original_hash, csvData_compare, csvData_original).second;
    print_IND(HLL_IND_candidate_compare);
    cout << endl;
    print_IND(HLL_IND_candidate_original);
}