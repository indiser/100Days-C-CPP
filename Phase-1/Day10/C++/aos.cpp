#include<iostream>
#include<vector>
#include<cstdint>
#include<chrono>

constexpr int N = 1 << 20;

struct Particle
{
    float x, y, z;
    uint64_t color;
};

int main()
{
    std::vector<Particle> p(N);

    for (int i = 0; i < N; i++)
    {
        p[i].x = i * 1.0f;
        p[i].y = i * 2.0f;
        p[i].z = i * 3.0f;
        p[i].color = i;
    }

    auto start = std::chrono::steady_clock::now();

    float sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += p[i].x + p[i].y + p[i].z;
    }

    auto end = std::chrono::steady_clock::now();
    double ns = std::chrono::duration<double, std::nano>(end - start).count();

    std::cout << "sum: " << sum << "\n";
    std::cout << "time: " << ns << " ns (" << ns / N << " ns/particle)\n";

    return 0;
}