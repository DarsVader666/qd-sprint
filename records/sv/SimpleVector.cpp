#include "SimpleVector.h"
#include <cassert>
#include <utility>

SimpleVector::SimpleVector(std::size_t n) : m_capacity(n) {
    if (n) m_data = new int[n];
}

SimpleVector::SimpleVector(const SimpleVector& o)
    : m_data(o.m_size ? new int[o.m_size] : nullptr),
      m_size(o.m_size), m_capacity(o.m_size) {
    for (std::size_t i = 0; i < m_size; ++i) m_data[i] = o.m_data[i];
}

SimpleVector& SimpleVector::operator=(const SimpleVector& o) {
    if (this == &o) return *this;
    SimpleVector tmp(o);              // copy-and-swap
    std::swap(m_data, tmp.m_data);
    std::swap(m_size, tmp.m_size);
    std::swap(m_capacity, tmp.m_capacity);
    return *this;
}

SimpleVector::~SimpleVector() { delete[] m_data; }

std::size_t SimpleVector::size() const { return m_size; }
std::size_t SimpleVector::capacity() const { return m_capacity; }
bool SimpleVector::empty() const { return m_size == 0; }

void SimpleVector::reserve(std::size_t n) {
    if (n <= m_capacity) return;
    int* p = new int[n];
    for (std::size_t i = 0; i < m_size; ++i) p[i] = m_data[i];
    delete[] m_data;
    m_data = p;
    m_capacity = n;
}

void SimpleVector::push_back(int v) {
    if (m_size == m_capacity) reserve(m_capacity ? m_capacity * 2 : 1);
    m_data[m_size++] = v;
}

void SimpleVector::pop_back() {
    assert(m_size > 0);
    --m_size;
}

void SimpleVector::clear() { m_size = 0; }

int& SimpleVector::operator[](std::size_t i) {
    assert(i < m_size);
    return m_data[i];
}

const int& SimpleVector::operator[](std::size_t i) const {
    assert(i < m_size);
    return m_data[i];
}

int* SimpleVector::data() { return m_data; }
const int* SimpleVector::data() const { return m_data; }
