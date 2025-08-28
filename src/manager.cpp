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
    if(!fs::is_directory(".abs")){
        fs::create_directory(".abs");
        std::fstream tracked(".abs/tracked.txt", std::ios::out);
        tracked << "SALAM";
    }
    else{
        std::cout << "SALAM2";
    }
}
