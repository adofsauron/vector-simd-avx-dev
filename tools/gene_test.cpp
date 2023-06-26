#include <iostream>
#include <random>
#include <fstream>
 
using namespace std;
 
void RandCharFile(const uint32_t file_len) {
    default_random_engine e;
    ofstream ofstr("../test_file");
    for (uint32_t i = 0; i < file_len; ++i) {
       ofstr << e() % 128;
    }
    ofstr.close();
}
 
int main() {
    RandCharFile(1024 * 1024 * 1024);
 
    return 0;
}