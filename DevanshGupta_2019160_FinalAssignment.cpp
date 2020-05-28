/* Author: Devansh Gupta
   Roll No.:2019160
   CO Group: 1
*/
#include<iostream>
#include<vector>
#include<unordered_map>
#include<hash_map>
#include<map>
#include<stack>
#include<queue>
#include<string>
#include<math.h>
#include<iomanip>
#include<cmath>
#include<cctype>

using namespace std; 

#define ll long long
#define pb push_back
#define fastcin ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL)
#define fin freopen("Inputs/input.txt","r",stdin)
#define fcache freopen("Outputs/cache.txt","w",stdout)
#define frep freopen("Outputs/report.txt","w",stdout)
#define fdebug freopen("debug.txt","w",stdout)
#define forn(n) for(ll int i=0;i<n;i++)
#define uniformize(s) transform(s.begin(), s.end(), s.begin(), ::tolower)
#define determine(s) s?"HIT":"MISS"
#define printrow_status(i,v) cout<<"|"<<setw(7)<<i<<"|"<<setw(32)<<v[0]<<"|"<<setw(14)<<v[1]<<"|"<<setw(15)<<v[2]<<"|"<<endl
#define partition_status cout<<"+"<<"-------"<<"+"<<"--------------------------------"<<"+"<<"--------------"<<"+"<<"---------------"<<"+"<<endl
#define partition_cache_tag cout<<"+"<<"--------------------"<<"+"<<endl
#define print_tag(s) cout<<"|"<<setw(20)<<s<<"|"<<endl
#define print_offset(v) cout<<"|"<<setw(27)<<v[0]<<"|"<<setw(24)<<v[1]<<"|"<<endl
#define partition_block cout<<"+"<<"==========================="<<"+"<<"========================"<<"+"<<endl
#define empty " "


unordered_map<string,unordered_map<string,ll int>>main_memory;
string hex_converter[16];
vector<vector<string>> output_table;
unordered_map<char,string> bin_converter;

void printTag(vector<pair<string,ll int>> tag_array){
    forn(tag_array.size()){
        cout<<tag_array[i].first<<" ";
    }
    cout<<endl;
}
string fillstr(string s){
    
    while(s.length()!=32){
        s="0"+s;
    }
    return s;
}
string fillstr_for_hex(string s){
    while(s.length()!=4){
        s="0"+s;
    }
    return s;
}
ll int convert_bin_to_int(string s){
    ll int sum=0;
    reverse(s.begin(),s.end());
    forn(s.length()){
        if(s[i]=='1'){
            sum+=pow(2,i);
        }
    }
    return sum;
}
string convert_int_to_bin(ll int num){
    string s="";
    while(num!=0){
        s+=to_string(num%2);
        num=num/2;
    }
    reverse(s.begin(),s.end());
    return s;
}
string convert_bin_to_hex(string binary){
    string hex="";
    ll int i=0;
    while(i<binary.length()){
        ll int num=convert_bin_to_int(binary.substr(i,4));
        hex+=hex_converter[num];
        i+=4;
    }
    return "0x"+hex;
}
string convert_hex_to_bin(string hex_string){
    string bin = "";
    forn(hex_string.length()){
        if(i<2){
            continue;
        }
        bin+=bin_converter[toupper(hex_string[i])];
    }
    return bin;
}
void initialize(){
    forn(16){
        if(i<10){
            hex_converter[i]=char(i+48);
            bin_converter[char(i+48)]=fillstr_for_hex(convert_int_to_bin(i));
        }
        else{
            hex_converter[i]=char(55+i);
            bin_converter[char(55+i)]=fillstr_for_hex(convert_int_to_bin(i));
        }
    }
}
bool check_pow_two(ll int a){
    string s = convert_int_to_bin(a);
    int sum = 0;
    forn(s.length()){
        if(s[i]=='1'){
            sum+=1;
        }
    }
    return sum==1;
}

bool check_params(ll int a, ll int b, ll int c, ll int d){
    if(a==1 || a==2 || a==3){
        if(b>=c && c>=d && d>=4)
        {
            if(check_pow_two(b) && check_pow_two(c) && check_pow_two(d)){
                return true;
            }
        }
        return false;
    }
    return false;
}

class Cache{
    public:
    ll int cache_size, block_size,no_cache_line, current,hits,misses,n;
    vector<unordered_map<string,ll int>> data_array;     //String is the "4 byte" offset
    vector<pair<string,ll int>> tag_array;               //string is the tag and int is the LRU enabler
    vector<ll int> set_tracker;
    Cache(){
        this->hits=0;
        this->misses=0;
        this->current=0;
    }
    void initialize_set_tracker(){
        forn(no_cache_line/n){
            set_tracker.pb(0);
        }
    }
    vector<string> parse_addresss_associative(string address){
        vector<string> parsed;
        parsed.pb(address.substr(0,32-log2(block_size)));
        parsed.pb(address.substr(32-log2(block_size),log2(block_size)-2));
        return parsed;
    }
    vector<string> parse_address_direct(string address){
        vector<string> parsed;
        parsed.pb(address.substr(0,32-log2(block_size)-log2(no_cache_line)));
        parsed.pb(address.substr(32-log2(block_size)-log2(no_cache_line),log2(no_cache_line)));
        parsed.pb(address.substr(32-log2(block_size),log2(block_size)-2));
        return parsed;
    }
    vector<string> parse_address_n_way(string address){
        vector<string> parsed;
        parsed.pb(address.substr(0,32-log2(block_size)-log2(no_cache_line/n)));
        parsed.pb(address.substr(32-log2(block_size)-log2(no_cache_line/n),log2(no_cache_line/n)));
        parsed.pb(address.substr(32-log2(block_size),log2(block_size)-2));
        return parsed;
    }
    pair<ll int,bool> associative_mapping_cache_read(string address);
    bool associative_mapping_cache_write(string address,ll int data_Insert);
    void initialize_data_and_tag_array();       //For direct mapping and n-way mapping
    pair<ll int,bool> direct_mapping_cache_read(string address);
    bool direct_mapping_cache_write(string address,ll int data_Insert);
    pair<ll int,bool> n_way_set_associative_cache_read(string address);
    bool n_way_set_associative_cache_write(string address,ll int data_Insert);
};

pair<ll int,bool> Cache::n_way_set_associative_cache_read(string address){
    vector<string> parse = parse_address_n_way(address);
    ll int index=convert_bin_to_int(parse[1]);
    ll int encoding=0;
    bool hit=false;
    ll int ret=0;
    forn(set_tracker[index]){
        if(tag_array[i+n*index].first==parse[0]){
            hit=true;
            ret = data_array[encoding+n*index][parse[2]];
            tag_array[i+n*index].second=7;
        }
        else{
            if(--tag_array[i+n*index].second<0){
                tag_array[i+n*index].second++;
            }
        }
        encoding++;
    }
    if(hit){
        this->hits++;
        return pair<ll int,bool>(ret,true);
    }
    this->misses++;
    if(set_tracker[index]<n){
        tag_array[index*n+encoding]=pair<string,ll int>(parse[0],7);
        data_array[encoding+n*index]=main_memory[parse[0]+parse[1]];
        set_tracker[index]++;
        return pair<ll int,bool>(data_array[index*n+encoding][parse[2]],false);
    }
    //Evict and Replace
    ll int min_num=INT32_MAX;
    forn(n){
        min_num=min(min_num,tag_array[n*index+i].second);
    }
    encoding=0;
    forn(n){
        if(tag_array[i+n*index].second==min_num){
            tag_array[i+n*index]=pair<string,ll int>(parse[0],7);
            data_array[encoding+n*index]=main_memory[parse[0]+parse[1]];
            return pair<ll int,bool>(data_array[encoding+n*index][parse[2]],false);
        }
        encoding++;
    }
    return pair<ll int,bool>(0,false);
}

bool Cache::n_way_set_associative_cache_write(string address,ll int data_Insert){
    vector<string> parse = parse_address_n_way(address);
    main_memory[parse[0]+parse[1]][parse[2]]=data_Insert;
    ll int index=convert_bin_to_int(parse[1]);
    ll int encoding=0;
    bool hit=false;
    forn(set_tracker[index]){
        if(tag_array[i+n*index].first==parse[0]){
            hit=true;
            data_array[encoding+n*index][parse[2]]=data_Insert;
            tag_array[i+n*index].second=7;
        }
        else{
            if(--tag_array[i+n*index].second<0){
                tag_array[i+n*index].second++;
            }
        }
        encoding++;
    }
    if(hit){
        this->hits++;
        return true;
    }
    this->misses++;
    if(set_tracker[index]<n){
        tag_array[index*n+encoding]=pair<string,ll int>(parse[0],7);
        data_array[encoding+n*index]=main_memory[parse[0]+parse[1]];
        set_tracker[index]++;
        return false;
    }
    //Evict and Replace
    ll int min_num=INT32_MAX;
    forn(n){
        min_num=min(min_num,tag_array[n*index+i].second);
    }
    encoding=0;
    forn(n){
        if(tag_array[i+n*index].second==min_num){
            tag_array[i+n*index]=pair<string,ll int>(parse[0],7);
            data_array[encoding+n*index]=main_memory[parse[0]+parse[1]];
            return false;
        }
        encoding++;
    }
    return false;
}

void Cache::initialize_data_and_tag_array(){
    forn(no_cache_line){
        unordered_map<string,ll int> empty_data;
        data_array.pb(empty_data);
        pair<string,ll int> empty_tag;
        tag_array.pb(empty_tag);
    }
}

pair<ll int,bool> Cache::direct_mapping_cache_read(string address){
    vector<string> parse=parse_address_direct(address);
    ll int index=convert_bin_to_int(parse[1]);
    if(tag_array[index].first==parse[0]){
        this->hits++;
        return pair<ll int,bool>(data_array[index][parse[2]],true);
    }
    this->misses++;
    //Evict and replace
    tag_array[index].first=parse[0];
    data_array[index]=main_memory[parse[0]+parse[1]];
    return pair<ll int,bool>(data_array[index][parse[2]],false);
}

bool Cache::direct_mapping_cache_write(string address, ll int data_Insert){
    vector<string> parse=parse_address_direct(address);
    main_memory[parse[0]+parse[1]][parse[2]]=data_Insert;
    ll int index=convert_bin_to_int(parse[1]);
    if(tag_array[index].first==parse[0]){
        this->hits++;
        data_array[index][parse[2]]=data_Insert;
        return true;
    }
    this->misses++;
    //Evict and replace
    tag_array[index].first=parse[0];
    data_array[index]=main_memory[parse[0]+parse[1]];
    return false;
}

pair<ll int,bool> Cache::associative_mapping_cache_read(string address){
    vector<string> parse=parse_addresss_associative(address);
    ll int encoding=0;
    ll int ret=0;
    bool hit=false;
    forn(tag_array.size()){
        if(tag_array[i].first==parse[0]){
            tag_array[i].second=7;
            ret=data_array[encoding][parse[1]];
            hit=true;
        }
        else{
            if(--tag_array[i].second<0){
                tag_array[i].second++;
            }
        }
        encoding++;
    }
    if(hit){
        this->hits++;
        return pair<ll int,bool>(ret,true);
    }
    this->misses++;
    if(this->current<no_cache_line){
        tag_array.pb(pair<string,ll int>(parse[0],7));
        data_array.pb(main_memory[parse[0]]);
        this->current++;
        return pair<ll int,bool>(data_array[tag_array.size()-1][parse[1]],false);
    }
    //Evict block and replace it with the current tag and data if the cache is filled
    ll int min_num=INT32_MAX;
    forn(tag_array.size()){
        min_num=min(min_num,tag_array[i].second);
    }
    encoding=0;
    forn(tag_array.size()){
        if(tag_array[i].second==min_num){
            tag_array[i]=pair<string,ll int>(parse[0],7);
            data_array[encoding]=main_memory[parse[0]];
            return pair<ll int,bool>(data_array[encoding][parse[1]],false);
        }
        encoding++;
    }
    return pair<ll int,bool>(0,false);
}

bool Cache::associative_mapping_cache_write(string address,ll int data_Insert){
    vector<string> parse=parse_addresss_associative(address);
    main_memory[parse[0]][parse[1]]=data_Insert;
    ll int encoding=0;
    bool hit = false;
    forn(tag_array.size()){
        if(tag_array[i].first==parse[0]){
            tag_array[i].second=7;
            data_array[encoding][parse[1]]=data_Insert;
            hit=true;
        }
        else{
            if(--tag_array[i].second<0){
                tag_array[i].second++;
            }
        }
        encoding++;
    }
    if(hit){
        this->hits++;
        return true;
    }
    this->misses++;
    if(this->current<no_cache_line){
        tag_array.pb(pair<string,ll int>(parse[0],7));
        data_array.pb(main_memory[parse[0]]);
        this->current++;
        return false;
    }
    //Evict block and replace it with the current tag and data if the cache is filled
    ll int min_num=INT32_MAX;
    forn(tag_array.size()){
        min_num=min(min_num,tag_array[i].second);
    }
    encoding=0;
    forn(tag_array.size()){
        if(tag_array[i].second==min_num){
            tag_array[i]=pair<string,ll int>(parse[0],7);
            data_array[encoding]=main_memory[parse[0]];
            data_array[encoding][parse[1]]=data_Insert;
            return false;
        }
        encoding++;
    }
    return false;
}


int main(){
    fastcin;
    fin;
    Cache main_cache= Cache();
    ll int type_of_cache,memory_size;
    cin>>type_of_cache>>memory_size>>main_cache.cache_size>>main_cache.block_size;
    if(!check_params(type_of_cache,memory_size,main_cache.cache_size,main_cache.block_size)){
        cout<<"ERROR at Line 1:Invalid Parameters>>Aborting::Note:Further instructions will not be executed"<<endl;
        return 0;
    }
    initialize();
    if(type_of_cache==1){
        main_cache.no_cache_line=main_cache.cache_size/main_cache.block_size;
        main_cache.n=1;
    }
    if(type_of_cache==2){
        //Direct Mapping Cache
        main_cache.no_cache_line=main_cache.cache_size/main_cache.block_size;
        main_cache.n=1;
        main_cache.initialize_data_and_tag_array();
    }
    if(type_of_cache==3){
        main_cache.no_cache_line=main_cache.cache_size/main_cache.block_size;
        cin>>main_cache.n;
        if(main_cache.n>main_cache.no_cache_line || main_cache.n<1){
            cout<<"ERROR at Line 1:Invalid Parameters>>Aborting::Note:Further instructions will not be executed"<<endl;
            return 0;
        }
        if(!check_pow_two(main_cache.n)){
            cout<<"ERROR at Line 1:Invalid Parameters>>Aborting::Note:Further instructions will not be executed"<<endl;
            return 0;
        }
        main_cache.initialize_set_tracker();
        main_cache.initialize_data_and_tag_array();
    }
    //Initializing memory and certifying type of cache
    ll int line = 1;
    string query;
    do{
        line++;
        string instruction = "";
        vector<string> row;
        cin>>query;
        uniformize(query);
        instruction+=query+" ";
        if(query=="read"){
            string address;
            cin>>address;
            instruction+=address+" ";
            row.pb(instruction);
            pair<ll int,bool> read_result;
            if(convert_bin_to_int(convert_hex_to_bin(address))<memory_size){
                //Read protocol will be implemented
                switch (type_of_cache)
                {
                case 1:
                    read_result=main_cache.associative_mapping_cache_read(fillstr(convert_hex_to_bin(address)));
                    break;

                case 2:
                    read_result=main_cache.direct_mapping_cache_read(fillstr(convert_hex_to_bin(address)));
                    break;
                
                case 3:
                    read_result=main_cache.n_way_set_associative_cache_read(fillstr(convert_hex_to_bin(address)));
                    break;
                
                default:
                    break;
                }
                row.pb(determine(read_result.second));
                row.pb(to_string(read_result.first));
            }
            else{
                cout<<"ERROR in Line "<<line<<" :Invalid Address>>Aborting::Note:Further instructions will not be executed"<<endl;
                query="abort";
                return 0;
            }
        }
        else if(query=="write"){
            ll int data;
            string address;
            cin>>address;
            instruction+=address+" ";
            if(convert_bin_to_int(convert_hex_to_bin(address))<memory_size){
                cin>>data;
                instruction+=to_string(data);
                row.pb(instruction);
                bool write_result;
                //Write protocol will be implemented- Write through
                switch (type_of_cache)
                {
                case 1:
                    write_result=main_cache.associative_mapping_cache_write(fillstr(convert_hex_to_bin(address)),data);
                    break;

                case 2:
                    write_result=main_cache.direct_mapping_cache_write(fillstr(convert_hex_to_bin(address)),data);
                    break;
                
                case 3:
                    write_result=main_cache.n_way_set_associative_cache_write(fillstr(convert_hex_to_bin(address)),data);
                    break;
                
                default:
                    break;
                }
                row.pb(determine(write_result));
                row.pb("NO OUTPUT");
            }
            else{
                cout<<"ERROR in Line "<<line<<" :Invalid Address>>Aborting::Note:Further instructions will not be executed"<<endl;
                query="abort";
                return 0;
            }
        }
        else if(query!="abort"){
            cout<<"ERROR in Line "<<line<<" :Invalid Instruction>>Aborting::Note:Further instructions will not be executed"<<endl;
            query = "abort";
            return 0;
        }
        output_table.pb(row);
    }while(query!="abort");
    vector<string> start;
    start.pb("Instruction");start.pb("Cache Status");start.pb("Output");
    frep;
    cout<<"TYPE OF CACHE: ";
    switch (type_of_cache)
    {
    case 1:
        cout<<"Associative"<<endl;
        break;
    case 2:
        cout<<"Direct Mapping"<<endl;
        break;
    case 3:
        cout<<main_cache.n<<" Way Set Associative"<<endl;
        break;
    default:
        break;
    }
    partition_status;
    printrow_status("S.No.",start);
    partition_status;
    forn(output_table.size()-1){
        printrow_status(i+1,output_table[i]);
        partition_status;
    }
    cout<<"TOTAL HITS: "<<main_cache.hits<<endl;
    cout<<"TOTAL MISSES: "<<main_cache.misses<<endl;
    fcache;
    partition_cache_tag;
    print_tag("      Tag Array     ");
    partition_cache_tag;
    for(ll int i=0;i<main_cache.tag_array.size();i++){
        print_tag(convert_bin_to_hex(main_cache.tag_array[i].first));
        partition_cache_tag;
    }
    cout<<endl;
    cout<<"Data Array"<<endl;
    start.clear();
    start.pb("Offset");
    start.pb("Data");
    partition_block;
    print_offset(start);
    partition_block;
    forn(main_cache.data_array.size()){
        for(auto& j: main_cache.data_array[i]){
            vector<string> row;
            row.pb(j.first);
            row.pb(to_string(j.second));
            print_offset(row);
        }
        partition_block;
    }
    return 0;
}