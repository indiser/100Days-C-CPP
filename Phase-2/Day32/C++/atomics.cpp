#include<iostream>
#include<vector>
#include<atomic>
#include<thread>
using namespace std;

// atomic<int> counter = 0;


// void count()
// {
//     for (int i = 0; i < 10000; i++)
//     {
//         counter ++;
//     }
// }

// void comp_exchange()
// {
//     atomic<int> x = 10;
//     int expected = 10;
//     bool success = x.compare_exchange_strong(expected, 100);

//     cout<< x <<endl;
// }

// void increment()
// {
//     int old = counter.load();
//     while(!counter.compare_exchange_weak(old, old+1))
//     {

//     }
// }

atomic<int> counter{0};

void increment() {
    counter.fetch_add(1, std::memory_order_relaxed);
}

int get_count() {
    return counter.load(std::memory_order_relaxed);
}

int main()
{
    // thread t1(count);
    // thread t2(count);

    // t1.join();
    // t2.join();

    increment();
    get_count();

    cout<< counter << endl;

    // comp_exchange();
    

    return 0;
}