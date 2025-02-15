#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<char, int> freq = {
    {L'C', 5722},
    {L'O', 1700},
    {L'N', 2375},
    {L'T', 7607},
    {L'E', 5519},
    {L'S', 6380},
    {L':', 1107},
    {L'\n', 97339},
    {L' ', 884352},
    {L'P', 2324},
    {L'R', 1681},
    {L'U', 452},
    {L'A', 4996},
    {L'I', 19948},
    {L'H', 7368},
    {L'G', 997},
    {L'B', 4544},
    {L'Y', 2320},
    {L'M', 12708},
    {L'F', 2698},
    {L'L', 5823},
    {L'D', 3032},
    {L'K', 671},
    {L'V', 499},
    {L'W', 5040},
    {L'J', 1771},
    {L'b', 59958},
    {L'y', 90353},
    {L'a', 329586},
    {L'n', 297110},
    {L'e', 536125},
    {L'u', 125136},
    {L's', 259036},
    {L't', 360835},
    {L'(', 511},
    {L'1', 149},
    {L'8', 57},
    {L')', 528},
    {L'h', 260185},
    {L'p', 65706},
    {L'r', 262284},
    {L'i', 269673},
    {L'l', 163474},
    {L'o', 323979},
    {L',', 75728},
    {L'f', 96537},
    {L'c', 99392},
    {L'm', 103425},
    {L'w', 92891},
    {L'v', 46910},
    {L'k', 23728},
    {L'g', 79459},
    {L';', 13344},
    {L'd', 176625},
    {L'.', 42184},
    {L'"', 18604},
    {L'7', 47},
    {L'6', 30},
    {L'0', 95},
    {L'5', 56},
    {L'4', 45},
    {L'z', 1799},
    {L'q', 4941},
    {L'9', 36},
    {L'-', 16456},
    {L'2', 82},
    {L'\'', 12183},
    {L'x', 7122},
    {L'j', 4398},
    {L'Z', 9},
    {L'?', 3238},
    {L'!', 4133},
    {L'Q', 53},
    {L'3', 47},
    {L'&', 36},
    {L'*', 109},
    {L'X', 378},
    {L'_', 4018},
    {L'[', 47},
    {L']', 47},
    {L'£', 16},
    {L'ê', 26},
    {L'à', 12},
    {L'é', 10},
};

struct Node {
    char ch;
    int freq;
    Node* left;
    Node* right;

    Node(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr) {}

    bool operator>(const Node& other) const {
        return freq > other.freq;
    }
};

Node* buildHuffmanTree(const std::unordered_map<char, int>& freq) {
    std::priority_queue<Node*, std::vector<Node*>, std::greater<Node*>> minHeap;

    // Add all chars to min heap
    for (const std::pair<char, int>& pair : freq) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    // combine nodes w/ lowest freq
    while (minHeap.size() != 1) {
        Node* left = minHeap.top();
        minHeap.pop();
        Node* right = minHeap.top();
        minHeap.pop();

        Node* sum = new Node('\0', left->freq + right->freq);
        sum->left = left;
        sum->right = right;
        minHeap.push(sum);
    }

    // most freq char is root
    return minHeap.top();
}

void buildHuffmanCode(Node* root, const std::string& str, std::unordered_map<char, std::string>& huffmanCode) {
    if (!root) return;

    // leaf, so assign code
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = str;
    }

    // not leaf, so recursively expand code
    buildHuffmanCode(root->left, str + "0", huffmanCode);
    buildHuffmanCode(root->right, str + "1", huffmanCode);
}

std::string decode(const std::vector<uint8_t>& compressed) {
    // Extract the bits packed in bytes
    std::string decompressed;
    for (uint8_t byte : compressed) {
        for (int i = 7; i >= 0; --i) {
            decompressed.push_back((byte >> i) & 1 ? '1' : '0');
        }
    }
    return decompressed;
}

std::string lzw_compress(const std::string& source) {
    // init dict
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary[std::string(1, i)] = i;
    }

    std::string current;
    std::vector<int> compressed;
    int dictSize = 256;

    for (char c : source) {
        // build up string
        std::string next = current + c;
        // check if in dict
        if (dictionary.count(next)) {
            current = next;
        } else {
            // add to dict if not present
            compressed.push_back(dictionary[current]);
            dictionary[next] = dictSize++;
            current = std::string(1, c);
        }
    }

    if (!current.empty()) {
        compressed.push_back(dictionary[current]);
    }

    // pack compressed code
    std::vector<uint8_t> packed;
    int byte = 0;
    int count = 0;
    for (int code : compressed) {
        for (int i = 15; i >= 0; --i) {
            if ((code >> i) & 1) {
                byte |= (1 << (7 - count));
            }
            ++count;
            if (count == 8) {
                packed.push_back(byte);
                byte = 0;
                count = 0;
            }
        }
    }
    if (count > 0) {
        packed.push_back(byte);
    }

    return std::string(packed.begin(), packed.end());
}

std::string lzw_decompress(const std::string& source) {
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < 256; i++) {
        dictionary[i] = std::string(1, i);
    }

    std::vector<int> compressed;
    int code = 0;
    int count = 0;

    // unpack compressed code
    for (char byte : source) {
        for (int i = 7; i >= 0; --i) {
            code = (code << 1) | ((byte >> i) & 1);
            ++count;

            if (count == 16) {
                compressed.push_back(code);
                code = 0;
                count = 0;
            }
        }
    }

    int dictSize = 256;
    std::string current = dictionary[compressed[0]];
    std::string decompressed = current;
    std::string entry;

    for (size_t i = 1; i < compressed.size(); i++) {
        int code = compressed[i];
        if (dictionary.count(code)) {
            entry = dictionary[code];
        } else if (code == dictSize) {
            entry = current + current[0];
        }

        decompressed += entry;
        dictionary[dictSize++] = current + entry[0];
        current = entry;
    }

    return decompressed;
}

std::string huffman_compress(const std::string& source) {
    // Build the huffman tree from the frequency list
    std::unordered_map<char, std::string> huffmanCode;
    Node* root = buildHuffmanTree(freq);
    buildHuffmanCode(root, "", huffmanCode);

    // Encode the source string using the huffman code
    std::vector<uint8_t> packed;
    uint8_t byte = 0;
    int count = 0;

    for (wchar_t c : source) {
        std::string code = huffmanCode[c];
        if (code.empty()) {
            return source;
        }

        // pack compressed code
        for (char bit : code) {
            byte = byte << 1 | (bit - '0');
            ++count;
            if (count == 8) {
                packed.push_back(byte);
                byte = 0;
                count = 0;
            }
        }
    }

    // account for remaining bits
    if (count > 0) {
        byte = byte << (8 - count);
        packed.push_back(byte);
    }

    return std::string(packed.begin(), packed.end());
}

std::string huffman_decompress(const std::string& source) {
    // build huffman tree from freq list
    Node* root = buildHuffmanTree(freq);
    std::string decompressed;
    Node* current = root;

    // unpack the compressed string into huffman bits
    std::string bits = decode(std::vector<uint8_t>(source.begin(), source.end()));

    // decompress the bits using the huffman tree
    for (char bit : bits) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        if (!current->left && !current->right) {
            decompressed += current->ch;
            current = root;
        }
    }

    return decompressed;
}

std::string compress(const std::string& source) {
    std::string compressed_huffman = huffman_compress(source);
    std::string compressed_lzw = lzw_compress(source);
    // use the smallest compressed string w/ an identifier
    if (compressed_huffman.size() < compressed_lzw.size() && compressed_huffman.size() < source.size()) {
        return "H" + compressed_huffman;
    } else if (compressed_lzw.size() < source.size()) {
        return "L" + compressed_lzw;
    } else {
        return "S" + source;
    }
}

std::string decompress(const std::string& source) {
    char method = source[0];
    std::string compressed = source.substr(1);

    // decompress based on identifier
    if (method == 'H') {
        return huffman_decompress(compressed);
    } else if (method == 'L') {
        return lzw_decompress(compressed);
    }
    return compressed;
}

int main() {
    std::string source = "This friend, and Sir Walter, did not marry, whatever might have been\nanticipated on that head by their acquaintance.  Thirteen years had\npassed away since Lady Elliot's death, and they were still near\nneighbours and intimate friends, and one remained a widower, the other\na widow.";
    std::string compressed = compress(source);
    std::string decompressed = decompress(compressed);
    std::cout << "Compressed: " << compressed << std::endl;
    std::cout << "Decompressed: " << decompressed << std::endl;
    std::cout << "Original size: " << source.size() << " bytes" << std::endl;
    std::cout << "Compressed size: " << compressed.size() << " bytes" << std::endl;
    std::cout << (source == decompressed) << std::endl;
    return 0;
}