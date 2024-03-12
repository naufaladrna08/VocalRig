#ifndef ARRAY_HPP
#define ARRAY_HPP
#include <cstddef>

template <typename T, std::size_t N>
struct Array {
  T data[N];
  const size_t size() const { return N; }

  const T operator[](size_t index) const { return data[index]; }
  operator T *() {
    T *p = new T[N];
    memcpy(p, data, sizeof(data));
    return p;
  }
};

template <typename T, typename... U>
Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

#endif