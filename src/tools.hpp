#include <utility>
#include <string>

uint64_t valid_int_input();
bool sort_by_cost(const std::pair<uint64_t, uint64_t>& p1, const std::pair<uint64_t, uint64_t>& p2);
bool compare_char(const char& c1, const char& c2);
bool compare_string(const std::string& s1, const std::string& s2);
std::string remove_accent(const std::string& s1);
unsigned int levenshtein_distance(const std::string& s1, const std::string& s2);
std::ifstream read_file(const std::string& _header, const std::string& _type, const std::string& _filename);
void sec_to_hhmmss(const uint64_t temps_sec); 
void clear_cin();
void quit_program();
