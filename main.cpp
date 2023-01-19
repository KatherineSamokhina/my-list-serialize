#include <iostream> 
#include <string> 
#include <unordered_map> 
#include <vector> 
#include <cassert> 
 
using namespace std; 
 
// структуру ListNode модифицровать нельзя 
struct ListNode { 
    ListNode* prev; 
    ListNode* next; 
    ListNode* rand; // указатель на произвольный элемент данного списка, либо NULL 
    std::string data; 
}; 
 
class List { 
public: 
    List() 
        : head(NULL) 
        , tail(NULL) 
        , count(0) 
    {} 
 
    List(ListNode* _head, ListNode* _tail, int _count) 
        : head(_head) 
        , tail(_tail) 
        , count(_count) 
    {} 
 
    ~List() 
    { 
        while(head != NULL) 
        { 
            tail = head->next; 
            delete head; 
            head = tail; 
        } 
    } 
 
    ListNode* GetHead() { 
        return head; 
    } 
    ListNode* GetTail() { 
        return tail; 
    } 
    int GetSize() { 
        return count; 
    } 
 
    struct NodeInfo { 
        string data; 
        int rand; 
    }; 
 
    // сохранение в файл (файл открыт с помощью fopen(path, "wb")) 
    void Serialize(FILE* file) { 
         
        int index = 0; 
        unordered_map<ListNode*, int> node_to_index; 
        ListNode* it = head; 
        while (it != NULL) { 
            node_to_index[it] = index; 
            it = it->next; 
            ++index; 
        } 
 
        it = head; 
        while (it != NULL) { 
            NodeInfo node_information; 
            node_information.data = it->data; 
            if (it->rand == NULL) 
                node_information.rand = -1; 
            else 
                node_information.rand = node_to_index[it->rand]; 
            fwrite(&node_information, sizeof(NodeInfo), 1, file); 
            it = it->next; 
        } 
        fclose(file); 
    } 
 
    // загрузка из файла (файл открыт с помощью fopen(path, "rb")) 
    void Deserialize(FILE* file) { 
         
        unordered_map<int,ListNode*> index_to_node; 
        NodeInfo node_information; 
        ListNode* prev_node = NULL; 
        int index = 0; 
        unordered_map<int, int> index_to_rand; 
 
        fread(&node_information, sizeof(NodeInfo), 1, file); 
        head = new ListNode; 
        head->data = node_information.data; 
        head->prev = prev_node; 
        index_to_node[index] = head; 
        index_to_rand[index] = node_information.rand; 
        prev_node = head; 
        ++index; 
        while (fread(&node_information, sizeof(NodeInfo), 1, file)) { 
            ListNode* new_node = new ListNode; 
            new_node->data = node_information.data; 
            new_node->prev = prev_node; 
            index_to_node[index] = new_node; 
            index_to_rand[index] = node_information.rand; 
            prev_node = new_node; 
            ++index; 
        } 
        fclose(file); 
 
        count = index; 
        tail = index_to_node[index - 1]; 
 
        ListNode* it = tail; 
        ListNode* next_node = NULL; 
        while (it != NULL) { 
            --index; 
            it->next = next_node; 
            it->rand = index_to_node[index_to_rand[index]]; 
            next_node = it; 
            it = it->prev; 
        } 
    } 
 
private: 
    ListNode* head; 
    ListNode* tail; 
    int count; 
}; 
 
 
int main() { 
    ListNode node1; 
    ListNode node2; 
    ListNode node3; 
    ListNode node4; 
 
    node1.data = "1"; 
    node1.next = &node2; 
    node1.prev = NULL; 
    node1.rand = &node3; 
 
    node2.data = "2"; 
    node2.next = &node3; 
    node2.prev = &node1; 
    node2.rand = &node2; 
 
    node3.data = "3"; 
    node3.next = &node4; 
    node3.prev = &node2; 
    node3.rand = NULL; 
 
    node4.data = "4"; 
    node4.next = NULL; 
    node4.prev = &node3; 
    node4.rand = &node1; 
 
    List list1(&node1, &node4, 4); 
 
    FILE* file; 
 
    file = fopen("out.dat", "wb"); 
    list1.Serialize(file); 
 
    file = fopen("out.dat", "rb"); 
    List list2; 
    list2.Deserialize(file); 
 
    assert(list1.GetSize() == list2.GetSize()); 
    assert((list1.GetHead())->data == (list2.GetHead())->data); 
    cout << "finish"; 
}
