#include <iostream>
#include <unordered_map>
#include <vector>
#include <cassert>

using namespace std;

// структуру ListNode модифицировать нельзя 
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

    ListNode* GetHead() {
        return head;
    }
    ListNode* GetTail() {
        return tail;
    }
    int GetSize() {
        return count;
    }

    // сохранение в файл (файл открыт с помощью fopen(path, "wb")) 
    void Serialize(FILE* file) {
        fwrite(&count, sizeof(int), 1, file);

        if (head == NULL) return;

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
            int buf_size = (it->data).size();
            char* buf = _strdup((it->data).c_str());
            int rand;
            if (it->rand == NULL)
                rand = -1;
            else
                rand = node_to_index[it->rand];
            fwrite(&buf_size, sizeof(int), 1, file);
            for(int i = 0; i < buf_size; ++i)
                fwrite(&buf[i], sizeof(char), 1, file);
            fwrite(&rand, sizeof(int), 1, file);

            it = it->next;
        }
    }

    // загрузка из файла (файл открыт с помощью fopen(path, "rb")) 
    void Deserialize(FILE* file) {
        fread(&count, sizeof(int), 1, file);

        if (count == 0) return;

        unordered_map<int, ListNode*> index_to_node;
        unordered_map<int, int> index_to_rand;

        ListNode* prev_node = NULL;
        int index = 0;
        
        int buf_size;
        string data;
        int rand;
                
        fread(&buf_size, sizeof(int), 1, file);
        for (int i = 0; i < buf_size; ++i) {
            char symbol;
            fread(&symbol, sizeof(char), 1, file);
            data.push_back(symbol);
        }
        fread(&rand, sizeof(int), 1, file);

        head = new ListNode{ NULL, NULL, NULL, data };
        index_to_node[index] = head;
        index_to_rand[index] = rand;
        prev_node = head;
        ++index;
        while (index < count) {
            data = "";
            fread(&buf_size, sizeof(int), 1, file);
            for (int i = 0; i < buf_size; ++i) {
                char symbol;
                fread(&symbol, sizeof(char), 1, file);
                data.push_back(symbol);
            }
            fread(&rand, sizeof(int), 1, file);

            prev_node->next = new ListNode{ prev_node, NULL, NULL, data };
            index_to_node[index] = prev_node->next;
            index_to_rand[index] = rand;
            prev_node = prev_node->next;
            ++index;
        }
        prev_node->next = NULL;
        tail= prev_node;

        ListNode* it = tail;
        while (it != NULL) {
            --index;
            if(index_to_rand[index] != -1)
                it->rand = index_to_node[index_to_rand[index]];
            it = it->prev;
        }
    }

private:
    ListNode* head;
    ListNode* tail;
    int count = 0;
};

void TestNormalList() {
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
    fclose(file);

    file = fopen("out.dat", "rb");
    List list2;
    list2.Deserialize(file);
    fclose(file);

    assert(list1.GetSize() == list2.GetSize());
    assert((list1.GetHead())->data == (list2.GetHead())->data);
    assert((list1.GetTail())->data == (list2.GetTail())->data);

}
void TestEmptyList() {
    List list1;

    FILE* file;

    file = fopen("out.dat", "wb");
    list1.Serialize(file);
    fclose(file);

    file = fopen("out.dat", "rb");
    List list2;
    list2.Deserialize(file);
    fclose(file);

    assert(list1.GetSize() == list2.GetSize());
    assert(list1.GetHead() == list2.GetHead());
    assert(list1.GetTail() == list2.GetTail());

}
void TestOneNodeList() {
    ListNode node1;
    
    node1.data = "1";
    node1.next = NULL;
    node1.prev = NULL;
    node1.rand = &node1;

    List list1(&node1, &node1, 1);

    FILE* file;

    file = fopen("out.dat", "wb");
    list1.Serialize(file);
    fclose(file);

    file = fopen("out.dat", "rb");
    List list2;
    list2.Deserialize(file);
    fclose(file);

    assert(list1.GetSize() == list2.GetSize());
    assert((list1.GetHead())->data == (list2.GetHead())->data);
    assert((list1.GetTail())->data == (list2.GetTail())->data);

}

int main() {
    TestNormalList();
    TestEmptyList();
    TestOneNodeList();

    return 0;
}
