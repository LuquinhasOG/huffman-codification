#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

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

void conversionDictionary(Node* root, map<char, string>* dictionary, string path="") {
    if (!root->isLeaf()) {
        convertionDictionary(root->getLeft(), dictionary, (path + "0"));
        convertionDictionary(root->getRight(), dictionary, (path + "1"));
    } else {
        dictionary->insert({root->getSymbol(), path});
    }
}

int main(int argc, char *argv[]) {
    ifstream stream(argv[1]);
    stringstream buffer;
    string text;

    buffer << stream.rdbuf();
    text = buffer.str();

    vector<Node*> f = countFrequence(text);
    Node* tree = buildHuffmanTree(f);
    map<char, string> convertion;
    conversionDictionary(tree, &convertion);

    for (pair<char, string> i : convertion)
        cout << i.first << ": " << i.second << endl;

    return 0;
}
