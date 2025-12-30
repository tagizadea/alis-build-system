#include <manager.hpp>

uint32_t crc32_table[256];

void generate_crc32_table(){
    uint32_t polynomial = 0xedb88320;
    for(uint32_t i = 0; i < 256; ++i){
        uint32_t crc = i;
        for(uint32_t j = 8; j > 0; --j){
            if(crc & 1) crc = (crc >> 1) ^ polynomial;
            else crc >>= 1;
        }
        crc32_table[i] = crc;
    }
}


uint32_t compute_crc32(std::ifstream &file){
    uint32_t crc = 0xffffffff;

    char byte;
    while(file.get(byte)){
        unsigned char byte_value = static_cast<unsigned char>(byte);
        crc = (crc >> 8) ^ crc32_table[(crc ^ byte_value) & 0xff];
    }

    return ~crc;
}

void init_manager(){
    generate_crc32_table();
    if(!fs::is_directory(".abs")) fs::create_directory(".abs");

    fstream tracked;
    if(!fs::exists(".abs/tracked.txt")) tracked.open(".abs/tracked.txt", ios::in | ios::out | ios::app);
    else tracked.open(".abs/tracked.txt", ios::in | ios::out);
    if(!tracked.is_open()){
        cout << "Manager error: The file \"tracked\" could not opened!";
        exit(0); // !!! Debug systemi ile deyis
    }

    fstream about(".abs/about.txt", ios::out);
    if(!about.is_open()){
        cout << "Manager error: The file \"about\" could not opened!";
        exit(0); // !!! Debug systemi ile deyis
    }
    about << "Ali's Build System for C/C++.\nVersion number: 0.8ALPHA";
    about.close();

    string line;
    int line_cnt = 0;
    while(getline(tracked, line)){
        if(line.size() <= 2) continue;
        string name = "", checksum = "";
        bool flag = false;
        for(int i=0;i<line.size();++i){

            if(line[i] == ' '){
                flag = true;
                continue;
            }

            if(flag) checksum += line[i];
            else name += line[i];
        }
        try{
            Manager::getInstance().tracked_files[name] = {++line_cnt, static_cast<uint32_t>(stoul(checksum))};
        }
        catch(const std::exception& e){
            cout << "Manager error: \"tracked\" file corrupted! " << e.what() << '\n';
            exit(0); // !!! Debug systemi ile deyis
        }
        
    }
    tracked.close();
    // for(auto i : Manager::getInstance().tracked_files) cout << i.first << ' ' << i.second.first << ' ' << i.second.second << '\n'; //

}

vector<string> track(){
    vector <string> ans;

    for(string s : Manager::getInstance().sources){
        if(fs::exists(s)){
            ifstream file(s);
            uint32_t checksum = compute_crc32(file);
            uint32_t saved_checksum = 0;
            bool first_time = false;
            if(Manager::getInstance().tracked_files.count(s))
                saved_checksum = Manager::getInstance().tracked_files[s].second;
            else first_time = true;
            
            if(checksum != saved_checksum){
                ans.push_back(s);
                Manager::getInstance().tracked_files[s].second = checksum;
                if(first_time){
                    fstream tracked(".abs/tracked.txt", ios::app);
                    tracked << s << ' ' << checksum << '\n';
                    tracked.close();
                }
                else{
                    int line = Manager::getInstance().tracked_files[s].first;
                    fstream tracked(".abs/tracked.txt", ios::in | ios::out);
                    string l;
                    int c_l = 0;
                    vector <string> temp;
                    while(getline(tracked, l)){
                        ++c_l;
                        if(c_l == line) temp.push_back( s + " " + to_string(checksum));
                        else temp.push_back(l);
                    }
                    tracked.clear();
                    tracked.seekp(0, ios::beg);
                    for(string si : temp){
                        tracked << si << '\n';
                    }
                    tracked.close();
                }
            }

            file.close();
        }
    }

    return ans;
}
