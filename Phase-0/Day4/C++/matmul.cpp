// matmul.cpp — big matrix multiply, three variants, templated
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <vector>

constexpr int N = 1024;
constexpr int BLOCK = 64;

template <typename T>
class Matrix {
public:
    Matrix() : data_(static_cast<size_t>(N) * N) {}

    T&       operator()(int r, int c)       { return data_[static_cast<size_t>(r) * N + c]; }
    const T& operator()(int r, int c) const { return data_[static_cast<size_t>(r) * N + c]; }

    T* raw() { return data_.data(); }
    const T* raw() const { return data_.data(); }

    void fill_pattern() {
        for (int i = 0; i < N * N; i++) data_[i] = static_cast<T>((i % 7) + 1);
    }

    void zero() {
        std::fill(data_.begin(), data_.end(), T(0));
    }

    T checksum() const {
        T s = T(0);
        for (int i = 0; i < N * N; i++) s += data_[i];
        return s;
    }

private:
    std::vector<T> data_;
};

template <typename T>
void mul_naive(const Matrix<T>& A, const Matrix<T>& B, Matrix<T>& C) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            T sum = T(0);
            for (int k = 0; k < N; k++)
                sum += A(i, k) * B(k, j);
            C(i, j) = sum;
        }
}

template <typename T>
void mul_reordered(const Matrix<T>& A, const Matrix<T>& B, Matrix<T>& C) {
    C.zero();
    for (int i = 0; i < N; i++)
        for (int k = 0; k < N; k++) {
            T a = A(i, k);
            for (int j = 0; j < N; j++)
                C(i, j) += a * B(k, j);
        }
}

template <typename T>
void mul_tiled(const Matrix<T>& A, const Matrix<T>& B, Matrix<T>& C) {
    C.zero();
    for (int ii = 0; ii < N; ii += BLOCK)
        for (int kk = 0; kk < N; kk += BLOCK)
            for (int jj = 0; jj < N; jj += BLOCK)
                for (int i = ii; i < ii + BLOCK; i++)
                    for (int k = kk; k < kk + BLOCK; k++) {
                        T a = A(i, k);
                        for (int j = jj; j < jj + BLOCK; j++)
                            C(i, j) += a * B(k, j);
                    }
}

int main(int argc, char** argv) {
    const char* mode = argc > 1 ? argv[1] : "naive";

    Matrix<double> A, B, C;
    A.fill_pattern();
    B.fill_pattern();

    auto t0 = std::chrono::steady_clock::now();

    if (std::strcmp(mode, "naive") == 0) mul_naive(A, B, C);
    else if (std::strcmp(mode, "reordered") == 0) mul_reordered(A, B, C);
    else if (std::strcmp(mode, "tiled") == 0) mul_tiled(A, B, C);
    else { std::fprintf(stderr, "unknown mode: %s\n", mode); return 1; }

    auto t1 = std::chrono::steady_clock::now();
    double secs = std::chrono::duration<double>(t1 - t0).count();

    std::fprintf(stderr, "mode=%s time=%.4fs checksum=%.1f\n", mode, secs, C.checksum());
    return 0;
}