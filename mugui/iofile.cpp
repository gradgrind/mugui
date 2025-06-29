#include "iofile.h"
#include <fstream>

using namespace std;

// Returns an empty string if read failed
string readfile(
    const string &filepath)
{
    string data;
    ifstream in(filepath.c_str());
    getline(in, data, string::traits_type::to_char_type(string::traits_type::eof()));
    return data;
}

bool writefile(
    const string &data, const string &filepath)
{
    std::ofstream file(filepath);
    if (file) {
        file << data;
        file.close();
        return true;
    }
    return false;
}
