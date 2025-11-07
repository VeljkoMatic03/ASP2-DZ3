#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <fstream>

using namespace std;

int ceil(int k){
    if(k % 2) return k/2+1;
    else return k/2;
}

struct Record{
    long long ca_id;
    long long ca_b_id;
    long long ca_c_id;
    string ca_name;
    int ca_tax_st;
    float ca_bal;
    Record(long long ca, long long cb, long long cc, string name, int tax, float bal):
    ca_id(ca), ca_b_id(cb), ca_c_id(cc), ca_tax_st(tax), ca_bal(bal){
        ca_name = name;
    }
    Record() = default;
    int operator==(const Record b) const{
        int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0, flag5 = 0, flag6 = 0;
        if(ca_id == b.ca_id)flag1 = 1;
        if(ca_c_id == b.ca_c_id)flag2 = 1;
        if(ca_b_id == b.ca_b_id) flag3 =1;
        if(ca_name == b.ca_name)flag4 = 1;
        if(ca_tax_st == b.ca_tax_st) flag5 = 1;
        if(ca_bal == b.ca_bal) flag6 = 1;
        if(flag1 and flag2 and flag3 and flag4 and flag5 and flag6) return 1;
        else return 0;
    }
};

struct RecordFile{
    Record** arr;
    int n;
    RecordFile(): n(0){
        arr = new Record*[10];
    }
};

class Node{
private:
    long long* keys;
    RecordFile** accounts;
    int n;
    Node** child;
    Node* parent;
    bool leaf;
public:
    Node(int m, bool leaf): n(0), parent(nullptr), leaf(leaf){
        keys = new long long[m - 1];
        accounts = new RecordFile*[m-1];
        child = new Node*[m+1];
        for(int i = 0; i < m+1; i++){
            child[i] = nullptr;
        }
    }
    ~Node(){
        delete[] keys;
        delete[] child;
        //delete[] accounts;
    }
    RecordFile* getFile(int index){return accounts[index];}
    void insertAcc(long long ind, long long key, Record* r){
        int pos = search(ind);
        RecordFile* rf = accounts[pos];
        int low = 0, high = rf->n - 1;
        if(key < rf->arr[low]->ca_id) {
            pos = low;
        }
        else if(key > rf->arr[high]->ca_id) {
            pos = high + 1;
        }
        else {
            while (low < high) {
                int mid = (low + high) / 2;
                if (key > rf->arr[mid]->ca_id and key < rf->arr[mid + 1]->ca_id) {
                    pos = mid + 1;
                    break;
                } else if (key > rf->arr[mid]->ca_id and key > rf->arr[mid + 1]->ca_id) {
                    low = mid + 1;
                } else high = mid;
            }
        }
        //rf->n++;
        for(int i = rf->n; i > pos; i--){
            rf->arr[i] = rf->arr[i-1];
        }
        rf->n++;
        rf->arr[pos] = r;
    }
    int search(long long key, int* num = nullptr){
        int low = 0, high = n-1;
        while(low <= high){
            if(num) (*num)++;
            int mid = (low + high) / 2;
            if(key == keys[mid]) return mid;
            else if(key > keys[mid]){
                low = mid + 1;
            }
            else high = mid - 1;
        }
        return -1;
    }
    int position(long long key){
        int low = 0, high = n-1;
        if(key < keys[low]) return low;
        if(key > keys[high]) return high + 1;
        while(low < high){
            int mid = (low + high) / 2;
            if(key > keys[mid] and key < keys[mid+1]) return mid+1;
            else if(key > keys[mid] and key > keys[mid + 1]) low = mid+1;
            else high = mid;
        }
    }
    long long getKey(int index) const{
        return this->keys[index];
    }
    Node* getChild(int index) const{
        return this->child[index];
    }
    int getN() const{ return this->n;}
    Node* getP() const{ return this->parent;}
    friend class Tree;
};

class Tree{
private:
    Node* root;
    int m;
public:
    Tree(int m): root(nullptr), m(m){}
    Tree(): root(nullptr), m(0){}
    Tree(const Tree& t){
        root = t.root;
        m = t.m;
    }
    Node* getroot(){
        return root;
    }
    Node* searchTree(long long key, int* num = nullptr){
        Node* curr = root;
        while(curr){
            int index = curr->search(key, num);
            if(index != -1) break;
            if(num) (*num)++;
            curr = curr->getChild(curr->position(key));
        }
        return curr;
    }
    Node* predecessor(Node* current, long long key, int* num = nullptr){
        int pos = current->search(key);
        Node* curr = current->child[pos];
        while(curr->child[curr->n]) curr = curr->child[curr->n], (*num)++;
        return curr;
    }
    Node* successor(Node* current, long long key, int* num = nullptr){
        int pos = current->search(key);
        Node *curr = current->child[pos+1];
        while(curr->child[0]) curr = curr->child[0], (*num)++;
        return curr;
    }

    void rightMerge(Node* lbro, Node* rbro, Node*& parent, int posnode){
        long long arr[2 * ceil(m) - 2];
        RecordFile* arr1[2 * ceil(m) - 2];
        for(int i = 0; i < ceil(m) - 2; i++){
            arr[i] = lbro->keys[i];
            arr1[i] = lbro->accounts[i];
        }
        arr[ceil(m) - 2] = parent->keys[posnode];
        arr1[ceil(m) - 2] = parent->accounts[posnode];
        for(int i = ceil(m) - 1, j = 0; i < 2*ceil(m) - 2; i++, j++){
            arr[i] = rbro->keys[j];
            arr1[i] = rbro->accounts[j];
        }
        Node* newnode = new Node(m, lbro->leaf);
        newnode->n = 2 * ceil(m) - 2;
        for(int i = 0; i < newnode->n; i++){
            newnode->keys[i] = arr[i];
            newnode->accounts[i] = arr1[i];
        }
        newnode->parent = parent;
        for(int i = 0; i < lbro->n + 1; i++){
            newnode->child[i] = lbro->child[i];
            if(newnode->child[i]) newnode->child[i]->parent = newnode;
        }
        for(int i = lbro->n + 1, j = 0; j < rbro->n + 1; j++, i++){
            newnode->child[i] = rbro->child[j];
            if(newnode->child[i]) newnode->child[i]->parent = newnode;
        }
        for(int i = posnode; i < parent->n - 1; i++){
            parent->keys[i] = parent->keys[i+1];
            parent->accounts[i] = parent->accounts[i+1];
            parent->child[i+1] = parent->child[i+2];
        }
        parent->child[posnode] = newnode;
        parent->n--;
    }

    void leftMerge(Node* lbro, Node* rbro, Node*& parent, int posnode){
        long long arr[2 * ceil(m) - 1];
        RecordFile* arr1[2 * ceil(m) - 1];
        for(int i = 0; i < ceil(m) - 1; i++){
            arr[i] = lbro->keys[i];
            arr1[i] = lbro->accounts[i];
        }
        arr[ceil(m) - 1] = parent->keys[posnode - 1];
        arr1[ceil(m) - 1] = parent->accounts[posnode - 1];
        for(int i = ceil(m), j = 0; i < 2*ceil(m) - 2; i++, j++){
            arr[i] = rbro->keys[j];
            arr1[i] = rbro->accounts[j];
        }
        Node* newnode = new Node(m, lbro->leaf);
        newnode->n = 2 * ceil(m) - 2;
        for(int i = 0; i < newnode->n; i++){
            newnode->keys[i] = arr[i];
            newnode->accounts[i] = arr1[i];
        }
        newnode->parent = parent;
        for(int i = 0; i < lbro->n + 1; i++){
            newnode->child[i] = lbro->child[i];
            if(newnode->child[i]) newnode->child[i]->parent = newnode;
        }
        for(int i = lbro->n + 1, j = 0; j < rbro->n + 1; j++, i++){
            newnode->child[i] = rbro->child[j];
            if(newnode->child[i]) newnode->child[i]->parent = newnode;
        }
        for(int i = posnode-1; i < parent->n - 1; i++){
            parent->keys[i] = parent->keys[i+1];
            parent->accounts[i] = parent->accounts[i+1];
            parent->child[i+1] = parent->child[i+2];
        }
        parent->child[posnode-1] = newnode;
        parent->n--;
    }

    void insert(long long k, RecordFile *f){
        if(searchTree(k)) return;
        if(root == nullptr){
            root = new Node(m, true);
            root->keys[0] = k;
            root->accounts[0] = f;
            root->n = 1;
        }
        else{
            Node* curr = root;
            while(!curr->leaf){
                int pos = curr->position(k);
                curr = curr->child[pos];
            }
            int pos = curr->position(k);
            if(curr->n < m-1){
                for(int i = curr->n; i > pos; i--){
                    curr->keys[i] = curr->keys[i-1];
                    curr->accounts[i] = curr->accounts[i-1];
                }
                curr->keys[pos] = k;
                curr->accounts[pos] = f;
                curr->n++;
            }
            else{
                long long arr[m];
                RecordFile* arr1[m];
                for(int i = 0; i < pos; i++) arr[i] = curr->keys[i], arr1[i] = curr->accounts[i];
                arr[pos] = k;
                arr1[pos] = f;
                for(int i = pos; i < curr->n; i++) arr[i+1] = curr->keys[i], arr1[i+1] = curr->accounts[i];
                Node* n1 = new Node(m, curr->leaf);
                for(int i = 0; i < ceil(m) - 1; i++){
                    n1->keys[i] = arr[i];
                    n1->accounts[i] = arr1[i];
                    n1->n++;
                }
                Node* n2 = new Node(m, curr->leaf);
                for(int i = ceil(m), j = 0; i < m; i++, j++){
                    n2->keys[j] = arr[i];
                    n2->accounts[j] = arr1[i];
                    n2->n++;
                }
                n2->parent = n1->parent = curr->parent;
                stack<Node*> s1;
                s1.push(n2);
                s1.push(n1);
                if(curr != root){
                    Node* p = curr->parent;
                    int i = 0;
                    while(p->child[i] != curr) i++;
                    for(int j = p->n; j > i; j--){
                        p->child[j+1] = p->child[j];
                    }
                    p->child[i] = n1;
                    p->child[i + 1] = n2;
                }
                long long k1 = arr[ceil(m) - 1];
                RecordFile* f1 = arr1[ceil(m) - 1];
                while(true){
                    if(curr == root){
                        root = new Node(m, false);
                        root->keys[0] = k1;
                        root->accounts[0] = f1;
                        root->n++;
                        for(int i = 0; i < 2; i++){
                            root->child[i] = s1.top();
                            s1.top()->parent = root;
                            s1.pop();
                        }
                        delete curr;
                        break;
                    }
                    else{
                        Node* p = curr->parent;
                        if(p->n < m - 1){
                            int pos = p->position(k1);
                            for(int i = p->n; i > pos; i--){
                                p->keys[i] = p->keys[i-1];
                                p->accounts[i] = p->accounts[i-1];
                            }
                            p->keys[pos] = k1;
                            p->accounts[pos] = f1;
                            p->n++;
                            while(!s1.empty()) s1.pop();
                            delete curr;
                            break;
                        }
                        else{
                            delete curr;
                            curr = p;
                            long long arr[m];
                            RecordFile* arr1[m];
                            int pos = curr->position(k1);
                            for(int i = 0; i < pos; i++) arr[i] = curr->keys[i], arr1[i] = curr->accounts[i];
                            arr[pos] = k1;
                            arr1[pos] = f1;
                            for(int i = pos; i < curr->n; i++) arr[i+1] = curr->keys[i], arr1[i+1] = curr->accounts[i];
                            Node* n1 = new Node(m, curr->leaf);
                            for(int i = 0; i < ceil(m) - 1; i++){
                                n1->keys[i] = arr[i];
                                n1->accounts[i] = arr1[i];
                                n1->n++;
                            }
                            Node* n2 = new Node(m, curr->leaf);
                            for(int i = ceil(m), j = 0; i < m; i++, j++){
                                n2->keys[j] = arr[i];
                                n2->accounts[j] = arr1[i];
                                n2->n++;
                            }
                            n2->parent = n1->parent = curr->parent;
                            for(int i = 0; i < (m+1) / 2; i++){
                                n1->child[i] = curr->child[i];
                                n1->child[i]->parent = n1;
                            }
                            for(int i = (m+1) / 2, j = 0; i < m+1; i++, j++){
                                n2->child[j] = curr->child[i];
                                n2->child[j]->parent = n2;
                            }
                            while(!s1.empty())s1.pop();
                            s1.push(n2);
                            s1.push(n1);
                            if(curr != root){
                                Node* p = curr->parent;
                                int i = 0;
                                while(p->child[i] != curr) i++;
                                for(int j = p->n; j > i; j--){
                                    p->child[j+1] = p->child[j];
                                }
                                p->child[i] = n1;
                                p->child[i + 1] = n2;
                            }
                            k1 = arr[ceil(m)  -1];
                            f1 = arr1[ceil(m) - 1];
                        }
                    }
                }
            }
        }
    }
    void remove(long long k, int *num = nullptr){
        Node* curr = searchTree(k, num);
        long long key = k;
        if(!curr) return;
        if(!curr->leaf){
            if(curr == root){
                int pos = curr->search(key);
                if((!curr->child[pos+1]) and (!curr->child[pos])){
                    for(int i = pos; i < curr->n - 1; i++){
                        curr->keys[i] = curr->keys[i+1];
                        curr->accounts[i] = curr->accounts[i+1];
                    }
                    curr->n--;
                    return;
                }
            }
            Node* succ = successor(curr, key, num), *pred = predecessor(curr, key, num);
            if(succ->n > (ceil(m) - 1)){
                curr->keys[curr->search(key)] = succ->keys[0];
                curr->accounts[curr->search(key)] = succ->accounts[0];
                curr = succ;
                key = succ->keys[0];
                int pos = curr->search(key);
                if((!curr->child[pos+1]) and (!curr->child[pos])) {
                    for (int i = pos; i < curr->n - 1; i++) {
                        curr->keys[i] = curr->keys[i + 1];
                        curr->accounts[i] = curr->accounts[i+1];
                    }
                    curr->n--;
                    return;
                }
                (*num) ++;
            }
            else if(pred->n > (ceil(m) - 1)){
                curr->keys[curr->search(key)] = pred->keys[pred->n - 1];
                curr->accounts[curr->search(key)] = pred->accounts[pred->n - 1];
                curr = pred;
                key = pred->keys[pred->n-1];
                int pos = curr->search(key);
                if((!curr->child[pos+1]) and (!curr->child[pos])) {
                    for (int i = pos; i < curr->n - 1; i++) {
                        curr->keys[i] = curr->keys[i + 1];
                        curr->accounts[i] = curr->accounts[i+1];
                    }
                    curr->n--;
                    return;
                }
                (*num)++;
            }
            else{
                //uzima se sledbenik, pa se ponavlja proces ispitivanja nad listom
                curr->keys[curr->search(key)] = succ->keys[0];
                curr->accounts[curr->search(key)] = succ->accounts[0];
                curr = succ;
                key = succ->keys[0];
                if(curr->n > (ceil(m) - 1)){
                    int pos = curr->search(key);
                    for (int i = pos; i < curr->n - 1; i++) {
                        curr->keys[i] = curr->keys[i + 1];
                        curr->accounts[i] = curr->accounts[i + 1];
                    }
                    curr->n--;
                    (*num)++;
                    return;
                }
                else{
                    int pos = curr->search(key);
                    for (int i = pos; i < curr->n - 1; i++) {
                        curr->keys[i] = curr->keys[i + 1];
                        curr->accounts[i] = curr->accounts[i + 1];
                    }
                    curr->n--;
                    Node* p = curr->parent;
                    int posnode = 0;
                    while(p->child[posnode] != curr) posnode++;
                    if(posnode == p->n){
                        if(p->child[posnode - 1]->n > (ceil(m) - 1)){
                            Node* lbro = p->child[posnode - 1];
                            int pos = curr->position(p->keys[posnode - 1]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode - 1];
                            curr->accounts[pos] = p->accounts[posnode - 1];
                            curr->n++;
                            for(int i = curr->n; i > 0; i--) curr->child[i] = curr->child[i-1];
                            curr->child[0] = lbro->child[lbro->n];
                            p->keys[posnode - 1] = lbro->keys[lbro->n - 1];
                            p->accounts[posnode - 1] = lbro->accounts[lbro->n - 1];
                            lbro->n--;
                            (*num)++;
                            return;
                        }
                        else{
                            Node* lbro = p->child[posnode - 1];
                            leftMerge(lbro, curr, p, posnode);
                            (*num)++;
                            delete curr, delete lbro;
                            curr = p;
                        }
                    }
                    else if(posnode == 0){
                        if(p->child[posnode + 1]->n > (ceil(m) - 1)){
                            Node *rbro = p->child[posnode + 1];
                            int pos = curr->position(p->keys[posnode]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode];
                            curr->accounts[pos] = p->accounts[posnode];
                            curr->n++;
                            curr->child[curr->n] = rbro->child[0];
                            p->keys[posnode] = rbro->keys[0];
                            p->accounts[posnode] = rbro->accounts[0];
                            for(int i = 0; i < rbro->n - 1; i++){
                                rbro->keys[i] = rbro->keys[i+1];
                                rbro->accounts[i] = rbro->accounts[i+1];
                            }
                            for(int i = 0; i < rbro->n; i++) rbro->child[i] = rbro->child[i+1];
                            rbro->n--;
                            (*num)++;
                            return;
                        }
                        else{
                            Node* rbro = p->child[posnode + 1];
                            rightMerge(curr, rbro, p, posnode);
                            (*num)++;
                            delete curr, delete rbro;
                            curr = p;
                        }
                    }
                    else{
                        Node *rbro = p->child[posnode + 1], *lbro = p->child[posnode - 1];
                        if(rbro->n > (ceil(m) - 1)){
                            int pos = curr->search(p->keys[posnode]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode];
                            curr->accounts[pos] = p->accounts[posnode];
                            curr->n++;
                            curr->child[curr->n] = rbro->child[0];
                            p->keys[posnode] = rbro->keys[0];
                            p->accounts[posnode] = rbro->accounts[0];
                            for(int i = 0; i < rbro->n - 1; i++){
                                rbro->keys[i] = rbro->keys[i+1];
                                rbro->accounts[i] = rbro->accounts[i+1];
                            }
                            for(int i = 0; i < rbro->n; i++) rbro->child[i] = rbro->child[i+1];
                            rbro->n--;
                            (*num)++;
                            return;
                        }
                        else if(lbro->n > (ceil(m) - 1)){
                            int pos = curr->position(p->keys[posnode - 1]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode - 1];
                            curr->accounts[pos] = p->accounts[posnode - 1];
                            curr->n++;
                            for(int i = curr->n; i > 0; i--) curr->child[i] = curr->child[i-1];
                            curr->child[0] = lbro->child[lbro->n];
                            p->keys[posnode - 1] = lbro->keys[lbro->n - 1];
                            p->accounts[posnode - 1] = lbro->accounts[lbro->n - 1];
                            lbro->n--;
                            (*num)++;
                            return;
                        }
                        else{
                            rightMerge(curr, rbro, p, posnode);
                            (*num)++;
                            delete curr, delete rbro;
                            curr = p;
                        }
                    }
                }
            }
        }
        else{
            if(curr == root){
                int pos = curr->search(key);
                if((!curr->child[pos+1]) and (!curr->child[pos])){
                    for(int i = pos; i < curr->n - 1; i++){
                        curr->keys[i] = curr->keys[i+1];
                        curr->accounts[i] = curr->accounts[i+1];
                    }
                    curr->n--;
                    (*num)++;
                    return;
                }
            }
            if(curr->n > (ceil(m) - 1)){
                int pos = curr->search(key);
                if((!curr->child[pos+1]) and (!curr->child[pos])) {
                    for (int i = pos; i < curr->n - 1; i++) {
                        curr->keys[i] = curr->keys[i + 1];
                        curr->accounts[i] = curr->accounts[i+1];
                    }
                    curr->n--;
                    (*num)++;
                    return;
                }
            }
            else{
                int pos = curr->search(key);
                for (int i = pos; i < curr->n - 1; i++) {
                    curr->keys[i] = curr->keys[i + 1];
                    curr->accounts[i] = curr->accounts[i+1];
                }
                curr->n--;
                Node* p = curr->parent;
                int posnode = 0;
                while(p->child[posnode] != curr) posnode++;
                if(posnode == p->n){
                    if(p->child[posnode - 1]->n > (ceil(m) - 1)){
                        Node* lbro = p->child[posnode - 1];
                        int pos = curr->position(p->keys[posnode - 1]);
                        for(int i = curr->n; i > pos; i--){
                            curr->keys[i] = curr->keys[i-1];
                            curr->accounts[i] = curr->accounts[i-1];
                        }
                        curr->keys[pos] = p->keys[posnode - 1];
                        curr->accounts[pos] = p->accounts[posnode - 1];
                        curr->n++;
                        for(int i = curr->n; i > 0; i--) curr->child[i] = curr->child[i-1];
                        curr->child[0] = lbro->child[lbro->n];
                        p->keys[posnode - 1] = lbro->keys[lbro->n - 1];
                        p->accounts[posnode - 1] = lbro->accounts[lbro->n - 1];
                        lbro->n--;
                        (*num)++;
                        return;
                    }
                    else{
                        Node* lbro = p->child[posnode - 1];
                        leftMerge(lbro, curr, p, posnode);
                        (*num)++;
                        delete curr, delete lbro;
                        curr = p;
                    }
                }
                else if(posnode == 0){
                    if(p->child[posnode + 1]->n > (ceil(m) - 1)){
                        Node *rbro = p->child[posnode + 1];
                        int pos = curr->position(p->keys[posnode]);
                        for(int i = curr->n; i > pos; i--){
                            curr->keys[i] = curr->keys[i-1];
                            curr->accounts[i] = curr->accounts[i-1];
                        }
                        curr->keys[pos] = p->keys[posnode];
                        curr->accounts[pos] = p->accounts[posnode];
                        curr->n++;
                        curr->child[curr->n] = rbro->child[0];
                        p->keys[posnode] = rbro->keys[0];
                        p->accounts[posnode] = rbro->accounts[0];
                        for(int i = 0; i < rbro->n - 1; i++){
                            rbro->keys[i] = rbro->keys[i+1];
                            rbro->accounts[i] = rbro->accounts[i+1];
                        }
                        for(int i = 0; i < rbro->n; i++) rbro->child[i] = rbro->child[i+1];
                        rbro->n--;
                        (*num)++;
                        return;
                    }
                    else{
                        Node* rbro = p->child[posnode + 1];
                        rightMerge(curr, rbro, p, posnode);
                        (*num)++;
                        delete curr, delete rbro;
                        curr = p;
                    }
                }
                else{
                    Node *rbro = p->child[posnode + 1], *lbro = p->child[posnode - 1];
                    if(rbro->n > (ceil(m) - 1)){
                        int pos = curr->position(p->keys[posnode]);
                        for(int i = curr->n; i > pos; i--){
                            curr->keys[i] = curr->keys[i-1];
                            curr->accounts[i] = curr->accounts[i-1];
                        }
                        curr->keys[pos] = p->keys[posnode];
                        curr->accounts[pos] = p->accounts[posnode];
                        curr->n++;
                        curr->child[curr->n] = rbro->child[0];
                        p->keys[posnode] = rbro->keys[0];
                        p->accounts[posnode] = rbro->accounts[0];
                        for(int i = 0; i < rbro->n - 1; i++){
                            rbro->keys[i] = rbro->keys[i+1];
                            rbro->accounts[i] = rbro->accounts[i+1];
                        }
                        for(int i = 0; i < rbro->n; i++) rbro->child[i] = rbro->child[i+1];
                        rbro->n--;
                        (*num)++;
                        return;
                    }
                    else if(lbro->n > (ceil(m) - 1)){
                        int pos = curr->position(p->keys[posnode - 1]);
                        for(int i = curr->n; i > pos; i--){
                            curr->keys[i] = curr->keys[i-1];
                            curr->accounts[i] = curr->accounts[i-1];
                        }
                        curr->keys[pos] = p->keys[posnode - 1];
                        curr->accounts[pos] = p->accounts[posnode - 1];
                        curr->n++;
                        for(int i = curr->n; i > 0; i--) curr->child[i] = curr->child[i-1];
                        curr->child[0] = lbro->child[lbro->n];
                        p->keys[posnode - 1] = lbro->keys[lbro->n - 1];
                        p->accounts[posnode - 1] = lbro->accounts[lbro->n - 1];
                        lbro->n--;
                        (*num)++;
                        return;
                    }
                    else{
                        rightMerge(curr, rbro, p, posnode);
                        (*num)++;
                        delete curr, rbro;
                        curr = p;
                    }
                }
            }
        }
        while(true){
            if(curr == root){
                if(curr->n > 0) return;
                else{
                    root = curr->child[0];
                    (*num)++;
                    delete curr;
                    return;
                }
            }
            else{
                //probamo od desnog brata, pa od levog, ako ne moze spajamo cvorove i stavljamo curr na oca i ponavljamo do kraja algoritma
                if(curr->n >= (ceil(m) - 1)) return;
                else{
                    Node* p = curr->parent;
                    int posnode = 0;
                    while(p->child[posnode] != curr) posnode++;
                    if(posnode == p->n){
                        if(p->child[posnode - 1]->n > (ceil(m) - 1)){
                            Node* lbro = p->child[posnode - 1];
                            int pos = curr->position(p->keys[posnode - 1]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode - 1];
                            curr->accounts[pos] = p->accounts[posnode - 1];
                            curr->n++;
                            for(int i = curr->n; i > 0; i--) curr->child[i] = curr->child[i-1];
                            curr->child[0] = lbro->child[lbro->n];
                            p->keys[posnode - 1] = lbro->keys[lbro->n - 1];
                            p->accounts[posnode - 1] = lbro->accounts[lbro->n - 1];
                            lbro->n--;
                            (*num)++;
                            return;
                        }
                        else{
                            Node* lbro = p->child[posnode - 1];
                            leftMerge(lbro, curr, p, posnode);
                            (*num)++;
                            delete curr, delete lbro;
                            curr = p;
                        }
                    }
                    else if(posnode == 0){
                        if(p->child[posnode + 1]->n > (ceil(m) - 1)){
                            Node *rbro = p->child[posnode + 1];
                            int pos = curr->position(p->keys[posnode]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode];
                            curr->accounts[pos] = p->accounts[posnode];
                            curr->n++;
                            curr->child[curr->n] = rbro->child[0];
                            p->keys[posnode] = rbro->keys[0];
                            p->accounts[posnode] = rbro->accounts[0];
                            for(int i = 0; i < rbro->n - 1; i++){
                                rbro->keys[i] = rbro->keys[i+1];
                                rbro->accounts[i] = rbro->accounts[i+1];
                            }
                            for(int i = 0; i < rbro->n; i++) rbro->child[i] = rbro->child[i+1];
                            rbro->n--;
                            (*num)++;
                            return;
                        }
                        else{
                            Node* rbro = p->child[posnode + 1];
                            rightMerge(curr, rbro, p, posnode);
                            (*num)++;
                            delete curr, delete rbro;
                            curr = p;
                        }
                    }
                    else{
                        Node *rbro = p->child[posnode + 1], *lbro = p->child[posnode - 1];
                        if(rbro->n > (ceil(m) - 1)){
                            int pos = curr->position(p->keys[posnode]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode];
                            curr->accounts[pos] = p->accounts[posnode];
                            curr->n++;
                            curr->child[curr->n] = rbro->child[0];
                            p->keys[posnode] = rbro->keys[0];
                            p->accounts[posnode] = rbro->accounts[0];
                            for(int i = 0; i < rbro->n - 1; i++){
                                rbro->keys[i] = rbro->keys[i+1];
                                rbro->accounts[i] = rbro->accounts[i+1];
                            }
                            for(int i = 0; i < rbro->n; i++) rbro->child[i] = rbro->child[i+1];
                            rbro->n--;
                            (*num)++;
                            return;
                        }
                        else if(lbro->n > (ceil(m) - 1)){
                            int pos = curr->position(p->keys[posnode - 1]);
                            for(int i = curr->n; i > pos; i--){
                                curr->keys[i] = curr->keys[i-1];
                                curr->accounts[i] = curr->accounts[i-1];
                            }
                            curr->keys[pos] = p->keys[posnode - 1];
                            curr->accounts[pos] = p->accounts[posnode - 1];
                            curr->n++;
                            for(int i = curr->n; i > 0; i--) curr->child[i] = curr->child[i-1];
                            curr->child[0] = lbro->child[lbro->n];
                            p->keys[posnode - 1] = lbro->keys[lbro->n - 1];
                            p->accounts[posnode - 1] = lbro->accounts[lbro->n - 1];
                            lbro->n--;
                            (*num)++;
                            return;
                        }
                        else{
                            rightMerge(curr, rbro, p, posnode);
                            (*num)++;
                            delete curr, rbro;
                            curr = p;
                        }
                    }
                }
            }
        }
    }
    void printTree(){
        if(!root){
            cout<<"NEMA";
            return;
        }
        queue<Node*> q1;
        Node* buffer;
        q1.push(root);
        q1.push(buffer);
        while(!q1.empty()){
            Node* curr = q1.front();
            q1.pop();
            if(curr == buffer and q1.empty()) break;
            if(curr == buffer and !q1.empty()){
                q1.push(buffer);
                cout<<endl;
                continue;
            }
            for(int i = 0; i < curr->n + 1; i++){
                if(curr->getChild(i) != nullptr) {
                    q1.push(curr->getChild(i));
                }
            }
            for(int i = 0; i < curr->n; i++){
                cout<<"|"<<curr->keys[i];
            }
            cout<<" ; ";
        }
    }
    int fillTree(string f){
        ifstream file(f);
        if(!file.is_open()) return 0;
        string str;
        while(getline(file, str)){
            string temp = "";
            long long ca, cb, cc;
            string name;
            int tax;
            float bal;
            int i = 0, cnt = 0;
            while(str[i] != '\0'){
                if(str[i] == '|'){
                    switch(cnt){
                        case(0) : ca = stoll(temp);
                            temp = "";
                            break;
                        case(1): cb = stoll(temp);temp = ""; break;
                        case(2): cc = stoll(temp); temp = ""; break;
                        case(3): name = temp; temp = ""; break;
                        case(4): tax = stoi(temp); temp = ""; break;
                        //case(5): bal = stof(temp); temp = ""; break;
                    }
                    cnt++;
                    i++;
                }
                else{
                    temp += str[i++];
                }
            }
            bal = stof(temp);
            Record* re = new Record(ca, cb, cc, name, tax, bal);
            RecordFile* recf = new RecordFile();
            recf->arr[0] = re;
            recf->n ++;
            Node* curr = searchTree(cc);
            if(!curr){
                insert(cc, recf);
            }
            else{
                curr->insertAcc(cc,ca, re);
            }
        }
        return 1;
    }
};

void findAllAcc(Tree t, long long cc){
    ofstream output("Izlaz.txt");
    ifstream input("C:\\Users\\HP\\Desktop\\Customer.txt");
    string temp = "", str;
    while(getline(input, str)){
        for(int i = 0; str[i] != '|'; i++) {
            temp += str[i];
        }
        long long x = stoll(temp);
        if(x == cc) break;
        temp = "";
    }
    temp = "";
    int i = 0, j = 0;
    while(i < 4){
        if(str[j] == '|') i++;
        temp += str[j++];
    }
    int num = 0;
    Node* curr = t.searchTree(cc, &num);
    if(curr){
        int pos = curr->search(cc, &num);
        num += curr->getFile(pos)->n;
        int broj = curr->getFile(pos)->n;
        cout<<endl<<"Pronadjeno je "<<broj<<" zapisa za datog korisnika.\nUkupan broj koraka pri pretrazi: "<<num<<endl;
        RecordFile* f = curr->getFile(pos);
        for(int i = 0; i < broj; i++){
            output<<temp<<f->arr[i]->ca_id<<'|'<<f->arr[i]->ca_c_id<<'|'<<f->arr[i]->ca_tax_st<<'|'<<fixed<<f->arr[i]->ca_bal<<endl;
        }
    }
    else{
        cout<<endl<<"Nije pronadjen nijedan zapis za datog korisnika.\nUkupan broj koraka pri pretrazi: "<<num<<endl;
        temp+= "null|null|null|null";
        output<<temp;
    }
    output.close();
}

void insertRecord(Tree &t, Record* r){
    long long cc = r->ca_c_id, ca = r->ca_id;
    stack<Node*> s1;
    s1.push(t.getroot());
    while(!s1.empty()){
        Node* curr = s1.top();
        s1.pop();
        for(int i = 0; i < curr->getN() + 1; i++){
            if(curr->getChild(i)) s1.push(curr->getChild(i));
        }
        for(int i = 0; i < curr->getN(); i++){
            RecordFile* rf = curr->getFile(i);
            for(int j = 0; j < rf->n; j++){
                if(rf->arr[j]->ca_id == ca){
                    cout<<endl<<"\nZapis sa ovim primarnim kljucem vec postoji u indeksu."<<endl;
                    return;
                }
            }
        }
    }
    Node* curr = t.searchTree(cc);
    curr->insertAcc(cc, ca, r);
}

void deleteAllRecords(Tree& t, long long cc){
    int num = 0;
    Node* curr = t.searchTree(cc, &num);
    if(!curr){
        cout<<"\nTrazeni cvor nije moguce obrisati - ne postoji.\n";
        cout<<"Broj koraka brisanja: "<<num<<endl;
    }
    else{
        num = 0;
        cout<<"\nObrisani zapisi:\n";
        RecordFile* rf = curr->getFile(curr->search(cc));
        num+=rf->n;
        for(int i = 0; i < rf->n; i++){
            cout<<rf->arr[i]->ca_id<<'|'<<rf->arr[i]->ca_b_id<<'|'<<rf->arr[i]->ca_c_id<<'|'<<rf->arr[i]->ca_name<<'|';
            cout<<rf->arr[i]->ca_tax_st<<'|'<<rf->arr[i]->ca_bal<<endl;
        }
        t.remove(cc, &num);
        cout<<"\nBroj koraka potrebnih za brisanje: "<<num<<endl;
    }
}

void deleteRecord(Tree &t, Record r){
    int num = 0;
    long long cc = r.ca_c_id, ca = r.ca_id;
    Node* curr = t.searchTree(cc, &num);
    if(!curr){
        cout<<"\nNijedan zapis nije obrisan.\n"<<"Broj koraka za koje je izvrsena operacija: "<<num<<endl;
        return;
    }
    else{
        RecordFile* rf = curr->getFile(curr->search(cc));
        int i = 0;
        for( i = 0; i < rf->n; i++){
            num++;
            Record* rec = rf->arr[i];
            if(*rec == r) break;
        }
        if(i == rf->n){
            cout<<"\nNijedan zapis nije obrisan.\n"<<"Broj koraka za koje je izvrsena operacija: "<<num<<endl;
            return;
        }
        delete rf->arr[i];
        for(i; i < rf->n - 1; i++){
            rf->arr[i] = rf->arr[i+1];
        }
        rf->n--;
        cout<<"\nUspesno brisanje zapisa.\nBroj koraka za koje je izvrsena operacija: "<<num<<endl;
    }
}

void menu(){
    cout<<"-----MENI-----\n";
    int flag = 0;
    Tree drvo(3);
    while(1){
        int m;
        cout<<"Opcije:\n0. Terminacija programa\n1. Stvaranje indeksa\n2. Ispis indeksa\n3. Pretraga svih racuna jednog korisnika\n";
        cout<<"4. Dodavanje novog zapisa u indeks\n5. Brisanje zapisa iz indeksa\n6. Brisanje svih racuna jednog korisnika\n";
        int choice;
        cout<<"Vas izbor: ";
        cin>>choice;
        while(choice < 0 or choice > 6){
            cout<<"\nIzabrali ste nepostojecu opciju. Molim Vas unesite ispravan broj: ";
            cin>>choice;
        }
        if(choice == 0) {
            cout<<"Kraj programa.";
            return;
        }
        else if(choice == 1){
            cout<<"\nStvaranje indeksa\n";
            cout<<"Unesite red indeksa izmedju 3 i 10: ";
            cin>>m;
            while(m < 3 or m > 10){
                cout<<"\nNevalidan red indeksa, unesite ispravan broj: ";
                cin>>m;
            }
            Tree drvo1(m);
            drvo = drvo1;
            cout<<"\nUnesite ime fajla za koji zelite da formirate indeks: ";
            string name;
            cin>>name;
            int control = drvo.fillTree(name);
            while(!control){
                cout<<"\nUnesite ispravno ime fajla: ";
                cin>>name;
                control = drvo.fillTree(name);
            }
            flag++;
            cout<<"\nUspesno kreiran indeks!\n";
        }
        else if(choice == 2){
            if(!flag){
                cout<<"\nMorate prvo kreirati indeks!\n";
            }
            else{
                cout<<"\nIspis indeksa po parametru CA_C_ID:\n";
                drvo.printTree();
                cout<<endl;
            }
        }
        else if(choice == 3){
            if(!flag) cout<<"\nMorate prvo kreirati indeks!\n";
            else{
                long long cc;
                cout<<"\nUnesite C_ID korisnika cije racune zelite da pretrazite: ";
                cin>>cc;
                findAllAcc(drvo, cc);
            }
        }
        else if(choice == 4){
            if(!flag) cout<<"\nMorate prvo kreirati indeks!\n";
            else{
                long long ca, cb, cc;
                string name;
                int tax;
                float bal;
                cout<<"\nUnesite podatke o novom racunu u sledecem poretku:\n";
                cout<<"CA_ID CA_B_ID CA_C_ID\nCA_NAME\nCA_TAX_ST CA_BAL\n";
                cin>>ca>>cb>>cc;
                cin>>name;
                cin>>tax>>bal;
                Record *r = new Record(ca, cb, cc, name, tax, bal);
                insertRecord(drvo, r);
            }
        }
        else if(choice == 5){
            if(!flag) cout<<"\nMorate prvo kreirati indeks!\n";
            else{
                long long ca, cb, cc;
                string name;
                int tax;
                float bal;
                cout<<"\nUnesite podatke o novom racunu u sledecem poretku:\n";
                cout<<"CA_ID CA_B_ID CA_C_ID CA_NAME\nCA_TAX_ST CA_BAL\n";
                cin>>ca>>cb>>cc>>name;
                cin>>tax>>bal;
                Record r(ca, cb, cc, name, tax, bal);
                deleteRecord(drvo, r);
            }
        }
        else{
            long long cc;
            cout<<"\nUnesite CA_C_ID korisnika cije sve racune zelite da obrisete: ";
            cin>>cc;
            deleteAllRecords(drvo, cc);
        }
    }
};

int main() {
    menu();
    return 0;
}