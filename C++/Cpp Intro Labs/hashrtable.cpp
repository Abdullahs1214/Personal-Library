#include <iostream>
#include <list>
#include <string>
using namespace std;

class HashTable {
    private:
    static const int hashGroups = 10;
    list<pair<int,string>>table[hashGroups];
    public:
    bool isEmpty();
    int hashFunction(int key);
    void insertItem(int key, string value);
    void removeItem(int key);
    string searchTable (int key);
    void printTable();
};
bool HashTable::isEmpty(){
    int sum{};
    for (int i{};i<hashGroups;i++){
        sum += table[i].size();
    }
    if (!sum){
        return true;
    }
    return false;
}
int HashTable::hashFunction(int key){
    return key % hashGroups;
}

void HashTable::insertItem(int key, string value){
    int hashValue = hashFunction(key);
    auto& cell = table[hashValue];
    auto itr = begin(cell);
    bool keyExists = false;
    for (; itr != end(cell); itr++){
        if (itr->first == key){
            keyExists = true;
            itr ->second = value;
            cout << "value replaced" << endl;
            break;
        }
    }
    if (!keyExists){
        cell.emplace_back(key,value);
        cout<< "pair inserted" << endl;
    }
    return;{

    }
}
void HashTable::removeItem(int key){
    int hashValue = hashFunction(key);
    auto& cell = table[hashValue];
    auto itr = begin(cell);
    bool keyExists = false;
    for (; itr != end(cell); itr++){
        if (itr->first == key){
            keyExists = true;
            itr= cell.erase(itr);
            cout << "pair erased" << endl;
            break;
        }
    }
    if (!keyExists){
        cout << "item not found" << endl;
    }
    return;
}
void HashTable::printTable(){
    for (int i{}; i < hashGroups; i++){
        if (table[i].size() == 0) continue;
        auto bitr = table[i].begin();
        for (;bitr != table[i].end(); bitr ++){
            cout << "key: " << bitr ->first << "Value: " << bitr->second << endl;
        }
    }
    return;
}
int main () {
    HashTable ht;
    if (ht.isEmpty()){
        cout << "working"<< endl;
    }else {
        cout << "problem" << endl;
    }
    ht.insertItem(905, "Jim");
    ht.insertItem(907, "Bob");
    ht.insertItem(904, "Rob");
    ht.insertItem(905, "Pop");
    ht.printTable();
    ht.removeItem(907);
    ht.removeItem(100);

    if (ht.isEmpty()){
        cout << "problem"<< endl;
    }else {
        cout << "working" << endl;
    }

    return 0;
}
