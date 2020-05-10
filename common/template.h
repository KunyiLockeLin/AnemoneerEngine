#pragma once

template <class T, int N>
template <class T2>
AeVector<T, N> AeVector<T, N>::operator/(const T2 &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size) {
        new_.elements[index = ] elements[index] - other;
        ++index;
    }
    return new_;
}
