#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <bitset>
#include <fstream>

using namespace std;

uint32_t rotr(uint32_t x, uint32_t n)
{
    return (x >> n) | (x << (32 - n));
}

uint32_t Ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

uint32_t Maj(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t Sig0(uint32_t x) { return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22); }
uint32_t Sig1(uint32_t x) { return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25); }
uint32_t sig0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3); }
uint32_t sig1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10); }

const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void preprocess(string &message, vector<uint8_t> &padded_message)
{
    uint64_t bitlen = message.size() * 8;

    for (char c : message)
    {
        padded_message.push_back(c);
    }

    padded_message.push_back(0x80);

    int padding_zeros = 448 - (bitlen + 1) % 512;
    padding_zeros = (padding_zeros < 0) ? padding_zeros + 512 : padding_zeros;
    for (int i = 0; i < padding_zeros / 8; i++)
    {
        padded_message.push_back(0x00);
    }

    for (int i = 0; i < 8; ++i)
    {
        padded_message.push_back((bitlen >> (56 - i * 8)) & 0xFF);
    }
}

int main()
{
    std::ifstream inFile("input.txt");
    if (!inFile)
    {
        std::cerr << "Unable to open file input.txt";
        return 1;
    }

    std::string text_in;
    std::string line;
    while (std::getline(inFile, line))
    {
        text_in += line + "\n";
    }
    inFile.close();

    vector<uint8_t> padded_message;
    preprocess(text_in, padded_message);

    uint32_t H[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    for (int i = 0; i * 64 < padded_message.size(); i++)
    {
        uint32_t W[64];
        for (int t = 0; t < 16; t++)
        {
            W[t] = (padded_message[i * 64 + t * 4] << 24) |
                   (padded_message[i * 64 + t * 4 + 1] << 16) |
                   (padded_message[i * 64 + t * 4 + 2] << 8) |
                   (padded_message[i * 64 + t * 4 + 3]);
        }
        for (int t = 16; t < 64; t++)
        {
            W[t] = sig1(W[t - 2]) + W[t - 7] + sig0(W[t - 15]) + W[t - 16];
        }

        uint32_t a = H[0], b = H[1], c = H[2], d = H[3];
        uint32_t e = H[4], f = H[5], g = H[6], h = H[7];

        for (int t = 0; t < 64; t++)
        {
            uint32_t T1 = h + Sig1(e) + Ch(e, f, g) + K[t] + W[t];
            uint32_t T2 = Sig0(a) + Maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;
    }

    for (int i = 0; i < 8; i++)
    {
        cout << hex << setfill('0') << setw(8) << H[i];
    }
    cout << endl;

    return 0;
}