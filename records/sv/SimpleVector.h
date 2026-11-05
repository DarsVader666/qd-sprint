#ifndef SIMPLE_VECTOR_H
#define SIMPLE_VECTOR_H
#include <cstddef>

class SimpleVector {
public:
    SimpleVector() = default;
    explicit SimpleVector(std::size_t n);
    SimpleVector(const SimpleVector&);
    SimpleVector& operator=(const SimpleVector&);
    ~SimpleVector();

    std::size_t size() const;
    std::size_t capacity() const;
    bool empty() const;
    void reserve(std::size_t n);
    void push_back(int v);
    void pop_back();
    void clear();

    int& operator[](std::size_t i);
    const int& operator[](std::size_t i) const;
    int* data();
    const int* data() const;

private:
    int* m_data = nullptr;
    std::size_t m_size = 0;
    std::size_t m_capacity = 0;
};
#endif
