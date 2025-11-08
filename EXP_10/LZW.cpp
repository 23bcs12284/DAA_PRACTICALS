// lzw.cpp
// Compile: g++ -std=c++17 -O2 lzw.cpp -o lzw
// Usage:
//   ./lzw c input.bin output.lzw
//   ./lzw d input.lzw output.bin

#include <bits/stdc++.h>
using namespace std;

// We use 12-bit codes (0..4095)
const int MAX_BITS = 12;
const int MAX_DICT = 1 << MAX_BITS;

void write_code(ofstream &ofs, uint32_t code, vector<uint8_t> &outbuf, int &bit_pos)
{
  // pack 12-bit code into outbuf; buffer bytes will be flushed by caller
  // bit_pos is next free bit position in overall stream (0..7 in last byte).
  for (int i = MAX_BITS - 1; i >= 0; --i)
  {
    int bit = (code >> i) & 1;
    if (outbuf.empty() || bit_pos == 8)
    {
      outbuf.push_back(0);
      bit_pos = 0;
    }
    outbuf.back() = (outbuf.back() << 1) | bit;
    bit_pos++;
  }
}

void flush_outbuf(ofstream &ofs, vector<uint8_t> &outbuf, int bit_pos)
{
  if (outbuf.empty())
    return;
  // If last byte not full (bit_pos < 8), left-shift it to make it a full byte already handled by above writing.
  // But because we always left shift while writing, bytes are already aligned. So simply write them.
  ofs.write(reinterpret_cast<const char *>(outbuf.data()), outbuf.size());
  outbuf.clear();
  bit_pos = 8; // indicates empty next -> handled by write_code logic
}

void compress_lzw(const string &infile, const string &outfile)
{
  ifstream ifs(infile, ios::binary);
  if (!ifs)
  {
    cerr << "Failed to open " << infile << "\n";
    exit(1);
  }
  ofstream ofs(outfile, ios::binary);
  // initialize dictionary with single-byte strings
  map<string, int> dict;
  for (int i = 0; i < 256; ++i)
  {
    string s(1, (char)i);
    dict[s] = i;
  }
  int next_code = 256;
  string w;
  vector<uint8_t> outbuf;
  int bit_pos = 8; // bit_pos tracks bits used in last buffer byte (we'll treat empty as 8 to force new byte)
  int c;
  while ((c = ifs.get()) != EOF)
  {
    char k = (char)c;
    string wk = w + k;
    if (dict.find(wk) != dict.end())
    {
      w = wk;
    }
    else
    {
      // output code for w
      write_code(ofs, dict[w], outbuf, bit_pos);
      if (next_code < MAX_DICT)
      {
        dict[wk] = next_code++;
      }
      w = string(1, k);
    }
  }
  if (!w.empty())
  {
    write_code(ofs, dict[w], outbuf, bit_pos);
  }
  // flush out buffered bytes (if last byte not fully filled, left shift so bits are in high-order bits)
  // Our write_code has been left-shifting into bytes; if last byte has <8 bits used, we need to left-shift remaining
  if (!outbuf.empty())
  {
    // If last byte is partially filled (bit_pos < 8), shift left to make the bits in MSBs
    if (bit_pos > 0 && bit_pos < 8)
    {
      outbuf.back() <<= (8 - bit_pos);
    }
    ofs.write(reinterpret_cast<const char *>(outbuf.data()), outbuf.size());
  }
  ifs.close();
  ofs.close();
}

uint32_t read_code(const vector<uint8_t> &buf, size_t &byte_idx, int &bit_idx)
{
  // read 12 bits from buf starting at (byte_idx, bit_idx) where bit_idx is bit position within current byte (0..7 from MSB)
  uint32_t code = 0;
  for (int i = 0; i < MAX_BITS; ++i)
  {
    if (byte_idx >= buf.size())
      return UINT32_MAX;
    int shift = 7 - bit_idx;
    int bit = (buf[byte_idx] >> shift) & 1;
    code = (code << 1) | bit;
    bit_idx++;
    if (bit_idx == 8)
    {
      bit_idx = 0;
      byte_idx++;
    }
  }
  return code;
}

void decompress_lzw(const string &infile, const string &outfile)
{
  ifstream ifs(infile, ios::binary);
  if (!ifs)
  {
    cerr << "Failed to open " << infile << "\n";
    exit(1);
  }
  vector<uint8_t> inbuf((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
  ifs.close();

  // initialize dictionary
  vector<string> dict;
  dict.reserve(MAX_DICT);
  for (int i = 0; i < 256; ++i)
  {
    dict.push_back(string(1, (char)i));
  }
  int next_code = 256;

  ofstream ofs(outfile, ios::binary);
  size_t byte_idx = 0;
  int bit_idx = 0;
  uint32_t prev_code = read_code(inbuf, byte_idx, bit_idx);
  if (prev_code == UINT32_MAX)
    return;
  string prev_str = dict[prev_code];
  ofs.write(prev_str.data(), prev_str.size());

  while (true)
  {
    uint32_t cur_code = read_code(inbuf, byte_idx, bit_idx);
    if (cur_code == UINT32_MAX)
      break;
    string entry;
    if (cur_code < dict.size())
    {
      entry = dict[cur_code];
    }
    else if (cur_code == next_code)
    {
      // special case: KwKwK
      entry = prev_str + prev_str[0];
    }
    else
    {
      cerr << "Bad compressed code: " << cur_code << "\n";
      exit(1);
    }
    ofs.write(entry.data(), entry.size());
    if (next_code < MAX_DICT)
    {
      dict.push_back(prev_str + entry[0]);
      next_code++;
    }
    prev_str = entry;
  }
  ofs.close();
}

int main(int argc, char **argv)
{
  if (argc != 4)
  {
    cerr << "Usage: " << argv[0] << " [c|d] input output\n";
    return 1;
  }
  string mode = argv[1];
  if (mode == "c")
    compress_lzw(argv[2], argv[3]);
  else if (mode == "d")
    decompress_lzw(argv[2], argv[3]);
  else
  {
    cerr << "Unknown mode\n";
    return 1;
  }
  return 0;
}
