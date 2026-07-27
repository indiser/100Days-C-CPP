#include<iostream>
#include<vector>
#include<stdexcept>
using namespace std;

struct Node
{
    int val;
    Node *next;
};

class FreeList
{
    private:
        Node *head;
        int freeCount;
    public:
        FreeList()
        {
            head = nullptr;
            freeCount = 0;
        }

        ~FreeList()
        {
            while(head)
            {
                Node *temp = head;
                head = head->next;
                delete temp;
            }
        }

        Node *allocate(int val)
        {
            Node *node;
            if(head)
            {
                node = head;
                head = head->next;
                freeCount--;
            }
            else node = new Node();
            node->val = val;
            node->next = nullptr;
            return node;
        }

        void deallocate(Node *node)
        {
            if(!node) return;
            node->next = head;
            head = node;
            freeCount++;
        }

        size_t getCount() { return freeCount; }

};

int main()
{
    try {
        FreeList pool;

        // Allocate nodes
        Node* n1 = pool.allocate(10);
        Node* n2 = pool.allocate(20);

        std::cout << "Allocated: " << n1->val << ", " << n2->val << "\n";

        // Free nodes
        pool.deallocate(n1);
        pool.deallocate(n2);

        std::cout << "Free nodes available: " << pool.getCount() << "\n";

        // Allocate again (should reuse freed nodes)
        Node* n3 = pool.allocate(30);
        std::cout << "Reused node with val: " << n3->val << "\n";

        pool.deallocate(n3);

    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << "\n";
    }

    return 0;
}