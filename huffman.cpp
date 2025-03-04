#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <bitset>
using namespace std;

#define tobstr(s) bitset<8>(s).to_string()
#define bit 0b00000001

#define pnode(n) cout << n->getSymbol() << ": " << n->getFrequence()

// classe que modela os nós da árvore
class Node {
private:
    Node* left = nullptr;
    Node* right = nullptr;
    char symbol;
    int frequence;

public:
    Node();
    Node(int frequence): frequence{frequence} {}
    Node(char symbol, int frequence): symbol{symbol}, frequence{frequence} {}

    Node* getLeft() { return this->left; }
    Node* getRight() { return this->right; }
    char getSymbol() { return this->symbol; }
    int getFrequence() { return this->frequence; }

    void setLeft(Node* node) { this->left = node; }
    void setRight(Node* node) { this->right = node; }

    bool isLeaf() { return (this->left == nullptr && this->right == nullptr) ? true : false; }
    void increment() { this->frequence++; }
};

// função para ordenar os nós da lista de frequência do menos frequente ao mais frequente
void sortByFrequence(vector<Node*>* frequence) {
    sort(frequence->begin(), frequence->end(), [](Node* a, Node* b){ return a->getFrequence() < b->getFrequence(); });
}

// verificar se a lista contém um nó com o símbolo fornecido
bool existsSymbolInList(vector<Node*> l, char c) {
    for (Node* n : l) {
        if (n->getSymbol() == c)
            return true;
    }

    return false;
}

// conta quantas vezes um símbolo aparece no texto e retorna uma lista com os nós de cada símbolo com a quantidade de suas ocorrências
vector<Node*> countFrequence(string text) {
    vector<Node*> nodes;
    map<char, int> chars_position;
    int aux = 0;
    for (char c : text) {
        if (!existsSymbolInList(nodes, c)) {
            nodes.insert(nodes.end(), new Node(c, 0));
            chars_position.insert({c, aux++});
        }

        nodes[chars_position[c]]->increment();
    }

    sortByFrequence(&nodes);
    return nodes;
}

// constroi a árvore
Node* buildHuffmanTree(vector<Node*> nodes) {
    // enquanto tiver pelos menos dois nós para serem juntados na árvore
    while (nodes.size() > 1) {
        // cria um novo nó e referencia como filhos os dois nós de menor frequêcia, e após, remove-os da lista
        Node* aux = new Node(nodes[0]->getFrequence()+nodes[1]->getFrequence());
        aux->setLeft(nodes[0]);
        aux->setRight(nodes[1]);

        nodes.insert(nodes.end(), aux);
        nodes.erase(nodes.begin());
        nodes.erase(nodes.begin());
        sortByFrequence(&nodes);
    }

    // o último elemento que sobra na lista é a raíz da árvore
    return nodes[0];
}

// cria o dicionario de codificação apartir da árvore
void encodingDictionary(Node* root, map<char, string>* dictionary, string path="") {
    if (!root->isLeaf()) {
        encodingDictionary(root->getLeft(), dictionary, (path + "0"));
        encodingDictionary(root->getRight(), dictionary, (path + "1"));
    } else {
        dictionary->insert({root->getSymbol(), path});
    }
}

// transforma o dicionário em bites para poder ser armazenado físicamento no arquivo final
// formato: char(1 byte), tamanho do código(1 byte), código(quantos bytes forem necessários)
vector<char> encodingDictToBytes(map<char, string> dictionary) {
    vector<char> buffer;
    for (auto p : dictionary) {
        buffer.push_back(p.first);
        buffer.push_back(p.second.size());

        // preencher os bits que faltam para completar os bytes para armazenar o código
        char aux = 0b00000000;
        int num_bits = (p.second.size() == 8) ? 8 : (p.second.size()/8 + 1)*8;
        int c = 0;

        for (int i = 0; i < num_bits; i++) {
            if (c/8 == 1) {
                c = 0;
                buffer.push_back(aux);
                aux = 0b00000000;
            }

            if (num_bits - i - 1 < p.second.size()) {
                aux = aux << 1;
                aux |= (p.second[p.second.size() - (num_bits - i)] - '0') & bit;
            }
            c++;
        }

        buffer.push_back(aux);
    }

    buffer.push_back(0b00000000);
    return buffer;
}

map<char, string> restoreDictFromBytes(vector<char> bytes) {
    map<char, string> dict;
    // esta variável diz se deve converter o byte para char
    bool to_char = true;
    // esta variável diz se deve converter os bytes para um caminho
    bool to_path = false;

    char current_char;
    int path_size, aux;
    string path = "";
    while (bytes.size() > 0) {
        if (to_char) {
            current_char = bytes[0];
            bytes.erase(bytes.begin());
            to_char = false;
        } else if (to_path) {
            if (aux/8 + 1 > 0 && aux > 0) {
                path += tobstr(bytes[0]);
                bytes.erase(bytes.begin());
                aux -= 8;
            } else {
                dict.insert({current_char, path.substr(path.size() - path_size, path_size)});
                path = "";
                to_char = true;
                to_path = false;
            }
        } else {
            path_size = bytes[0];
            aux = path_size;
            bytes.erase(bytes.begin());
            to_path = true;
        }
    }

    return dict;
}

vector<char> encodeText(string text, map<char, string> dict) {
    // codifica usando o dicionário
    string encoded;
    for (char c : text)
        encoded += dict[c];

    // adiciona os bits que faltarem para completar o último byte
    string aux;
    for (int i = 0; i < encoded.size()%8; i++)
        aux += "0";

    encoded += aux;
    encoded += char(encoded.size()%8);

    vector<char> buffer;
    for (int i = 0; i < encoded.size()/8; i++) {
        buffer.insert(buffer.end(), stoi(encoded.substr(i*8, 8), nullptr, 2));
    }

    return buffer;
}

void writeEncodedFile(vector<char> bytes_dict_buffer, vector<char> encoded_text_buffer) {
    string write;
    for (char c : bytes_dict_buffer) {
        write += c;
    }
    write += 0xAAAAAAAA;
    for (char c : encoded_text_buffer) {
        write += c;
    }

    ofstream out("test.huff");
    out << write;
    out.close();

    cout << write << endl;
}

int main(int argc, char *argv[]) {
    ifstream stream(argv[1]);
    stringstream buffer;
    string text;
    buffer << stream.rdbuf();
    text = buffer.str();
    stream.close();

    vector<Node*> f = countFrequence(text);
    Node* tree = buildHuffmanTree(f);
    map<char, string> encoder;
    encodingDictionary(tree, &encoder);
    vector<char> bytes_dict_buffer = encodingDictToBytes(encoder);
    vector<char> encoded_text_buffer = encodeText(text, encoder);

    writeEncodedFile(bytes_dict_buffer, encoded_text_buffer);

    /**
    cout << "Encoding dictionary:" << endl;
    for (auto i : encoder) {
        cout << i.first << ": " << i.second << endl;
    }

    cout << endl << "Encoding dictionary to store in file:" << endl;
    for (auto i : bytes_dict) {
        cout << tobstr(i) << endl;
    }
    cout << endl;

    map<char, string> recovered = restoreDictFromBytes(bytes_dict);
    cout << "Recovered dictionary:" << endl;
    for (auto i : recovered) {
        cout << i.first << ": " << i.second << endl;
    }
    **/

    return 0;
}
