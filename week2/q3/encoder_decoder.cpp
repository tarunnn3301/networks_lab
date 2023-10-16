#include <iostream>
#include <string>
#include <vector>

using namespace std;

const string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

string base64_encode(const string &input) {
    string encoded;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 63]);
            valb -= 6;
        }
    }
    if (valb > -6)
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 63]);
    while (encoded.size() % 4 != 0)
        encoded.push_back('=');
    return encoded;
}

string base64_decode(const string &input) {
    string decoded;
    vector<int> vec(256, -1);
    for (int i = 0; i < 64; i++)
        vec[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : input) {
        if (vec[c] == -1)
            break;
        val = (val << 6) + vec[c];
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(static_cast<char>((val >> valb) & 255));
            valb -= 8;
        }
    }
    return decoded;
}
