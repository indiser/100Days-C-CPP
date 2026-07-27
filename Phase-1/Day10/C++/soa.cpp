#include<iostream>
#include<vector>
#include<cstdint>
#include<chrono>

constexpr int N = 1 << 20;

int main()
{
    std::vector<float> x(N), y(N), z(N);
    std::vector<uint64_t> color(N);

    for (int i = 0; i < N; i++)
    {
        x[i] = i * 1.0f;
        y[i] = i * 2.0f;
        z[i] = i * 3.0f;
        color[i] = i;
    }

    auto start = std::chrono::steady_clock::now();

    float sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += x[i] + y[i] + z[i];
    }

    auto end = std::chrono::steady_clock::now();
    double ns = std::chrono::duration<double, std::nano>(end - start).count();

    std::cout << "sum: " << sum << "\n";
    std::cout << "time: " << ns << " ns (" << ns / N << " ns/particle)\n";

    return 0;
}