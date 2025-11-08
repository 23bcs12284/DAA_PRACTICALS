// huffman.cpp
// Compile: g++ -std=c++17 -O2 huffman.cpp -o huffman
// Usage:
//   ./huffman c input.bin output.huff   # compress
//   ./huffman d input.huff output.bin   # decompress

#include <bits/stdc++.h>
using namespace std;

struct Node
{
  unsigned char ch;
  uint64_t freq;
  Node *left, *right;
  Node(unsigned char c, uint64_t f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
  Node(Node *l, Node *r) : ch(0), freq(l->freq + r->freq), left(l), right(r) {}
};

// comparator for priority queue (min-heap)
struct Cmp
{
  bool operator()(const Node *a, const Node *b) const
  {
    return a->freq > b->freq;
  }
};

void build_codes(Node *root, vector<string> &codes, string cur)
{
  if (!root)
    return;
  if (!root->left && !root->right)
  {
    codes[root->ch] = cur.empty() ? "0" : cur; // handle single-symbol file
  }
  else
  {
    build_codes(root->left, codes, cur + "0");
    build_codes(root->right, codes, cur + "1");
  }
}

void free_tree(Node *root)
{
  if (!root)
    return;
  free_tree(root->left);
  free_tree(root->right);
  delete root;
}

// write bits in buffer to ostream (as bytes). Keep track of leftover bits.
void write_bits(ofstream &ofs, const string &bits, uint8_t &bit_buffer, int &bit_count)
{
  for (char b : bits)
  {
    bit_buffer = (bit_buffer << 1) | (b - '0');
    bit_count++;
    if (bit_count == 8)
    {
      ofs.put((char)bit_buffer);
      bit_buffer = 0;
      bit_count = 0;
    }
  }
}

void compress_file(const string &infile, const string &outfile)
{
  // count frequencies
  array<uint64_t, 256> freq{};
  freq.fill(0);
  ifstream ifs(infile, ios::binary);
  if (!ifs)
  {
    cerr << "Failed to open " << infile << "\n";
    exit(1);
  }
  char c;
  while (ifs.get(c))
    freq[(unsigned char)c]++;
  ifs.clear();
  ifs.seekg(0);

  // build Huffman tree
  priority_queue<Node *, vector<Node *>, Cmp> pq;
  for (int i = 0; i < 256; ++i)
    if (freq[i])
      pq.push(new Node((unsigned char)i, freq[i]));
  if (pq.empty())
  { // empty file: write empty header and exit
    ofstream ofs(outfile, ios::binary);
    // write header: all zeros (256 uint64_t)
    for (int i = 0; i < 256; ++i)
    {
      uint64_t z = 0;
      ofs.write(reinterpret_cast<const char *>(&z), sizeof(z));
    }
    return;
  }
  // special case: one symbol only
  while (pq.size() > 1)
  {
    Node *a = pq.top();
    pq.pop();
    Node *b = pq.top();
    pq.pop();
    pq.push(new Node(a, b));
  }
  Node *root = pq.top();

  vector<string> codes(256);
  build_codes(root, codes, "");

  // open output and write header: 256 uint64_t frequencies (so decompressor can rebuild)
  ofstream ofs(outfile, ios::binary);
  for (int i = 0; i < 256; ++i)
    ofs.write(reinterpret_cast<const char *>(&freq[i]), sizeof(freq[i]));

  // encode the data stream
  uint8_t bit_buffer = 0;
  int bit_count = 0;
  while (ifs.get(c))
  {
    write_bits(ofs, codes[(unsigned char)c], bit_buffer, bit_count);
  }
  // write remaining bits (pad with zeros on the right)
  if (bit_count > 0)
  {
    bit_buffer <<= (8 - bit_count);
    ofs.put((char)bit_buffer);
    // write a final byte telling how many bits in last byte were used (0..8)
    // We'll append one extra byte indicating valid bits in final byte (bit_count).
    // But since we wrote freq header of fixed size, append this trailing info now:
    ofs.put((char)bit_count); // last byte: number of valid bits in final data byte
  }
  else
  {
    // if bit_count==0, last data byte fully used; indicate 8 bits used
    ofs.put((char)8);
  }

  ofs.close();
  ifs.close();
  free_tree(root);
}

Node *build_tree_from_freq(const array<uint64_t, 256> &freq)
{
  priority_queue<Node *, vector<Node *>, Cmp> pq;
  for (int i = 0; i < 256; ++i)
    if (freq[i])
      pq.push(new Node((unsigned char)i, freq[i]));
  if (pq.empty())
    return nullptr;
  while (pq.size() > 1)
  {
    Node *a = pq.top();
    pq.pop();
    Node *b = pq.top();
    pq.pop();
    pq.push(new Node(a, b));
  }
  return pq.top();
}

void decompress_file(const string &infile, const string &outfile)
{
  ifstream ifs(infile, ios::binary);
  if (!ifs)
  {
    cerr << "Failed to open " << infile << "\n";
    exit(1);
  }
  // read header
  array<uint64_t, 256> freq;
  for (int i = 0; i < 256; ++i)
    ifs.read(reinterpret_cast<char *>(&freq[i]), sizeof(freq[i]));

  // if header all zero -> empty file
  bool allzero = true;
  for (int i = 0; i < 256; ++i)
    if (freq[i])
    {
      allzero = false;
      break;
    }
  if (allzero)
  {
    ofstream ofs(outfile, ios::binary);
    return;
  }

  // build tree
  Node *root = build_tree_from_freq(freq);

  // read entire remaining file into buffer
  vector<unsigned char> rest((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
  ifs.close();
  if (rest.empty())
  {
    cerr << "Corrupt file: no bitstream\n";
    exit(1);
  }

  // last byte indicates number of valid bits in final data byte
  unsigned char last_valid_bits = rest.back();
  rest.pop_back();
  if (rest.empty())
  {
    cerr << "Corrupt: no encoded bytes\n";
    exit(1);
  }

  // convert bytes to stream of bits, but only up to total valid bits
  // total bits = 8*(rest.size()-1) + last_valid_bits but easier to process until we hit EOF marker by traversing tree
  ofstream ofs(outfile, ios::binary);

  Node *cur = root;
  size_t total_bytes = rest.size();
  for (size_t i = 0; i < total_bytes; ++i)
  {
    unsigned char byte = rest[i];
    int bits_to_read = 8;
    if (i == total_bytes - 1)
      bits_to_read = (int)last_valid_bits;
    for (int b = 7; b >= 8 - bits_to_read; --b)
    {
      int bit = (byte >> b) & 1;
      if (bit == 0)
        cur = cur->left;
      else
        cur = cur->right;
      if (!cur->left && !cur->right)
      {
        ofs.put((char)cur->ch);
        cur = root;
      }
    }
  }
  ofs.close();
  free_tree(root);
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
    compress_file(argv[2], argv[3]);
  else if (mode == "d")
    decompress_file(argv[2], argv[3]);
  else
  {
    cerr << "Unknown mode: use 'c' to compress or 'd' to decompress\n";
    return 1;
  }
  return 0;
}
