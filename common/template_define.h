#pragma once

template <class T, int N>
 AeVector<T, N> ::AeVector() {
    std::memset(elements, 0, size);
 }

template <class T, int N>
template <class T2, int N2>
bool AeVector<T, N>::operator==(const AeVector<T2, N2> &other) const {
    int index = 0;
    while (index < size && index < other.size) {
        if (elements[index] != other.elements[index]) {
            return false;
        }
        ++index;
    }
    return true;
}

template <class T, int N>
template <class T2, int N2>
bool AeVector<T, N>::operator!=(const AeVector<T2, N2> &other) const {
    return !(this == other);
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> &AeVector<T, N>::operator=(const AeVector<T2, N2> &other) {
    int index = 0;
    while (index < size && index < other.size) {
        elements[index] = other.elements[index];
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> &AeVector<T, N>::operator+=(const AeVector<T2, N2> &other) {
    int index = 0;
    while (index < size && index < other.size) {
        elements[index] += other.elements[index];
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> &AeVector<T, N>::operator-=(const AeVector<T2, N2> &other) {
    int index = 0;
    while (index < size && index < other.size) {
        elements[index] -= other.elements[index];
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> &AeVector<T, N>::operator*=(const AeVector<T2, N2> &other) {
    int index = 0;
    while (index < size && index < other.size) {
        elements[index] *= other.elements[index];
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> &AeVector<T, N>::operator/=(const AeVector<T2, N2> &other) {
    int index = 0;
    while (index < size && index < other.size) {
        elements[index] /= other.elements[index];
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> AeVector<T, N>::operator+(const AeVector<T2, N2> &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size && index < other.size) {
        new_.elements[index = ] elements[index] + other.elements[index];
        ++index;
    }
    return new_;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> AeVector<T, N>::operator-(const AeVector<T2, N2> &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size && index < other.size) {
        new_.elements[index = ] elements[index] - other.elements[index];
        ++index;
    }
    return new_;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> AeVector<T, N>::operator*(const AeVector<T2, N2> &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size && index < other.size) {
        new_.elements[index = ] elements[index] * other.elements[index];
        ++index;
    }
    return new_;
}

template <class T, int N>
template <class T2, int N2>
AeVector<T, N> AeVector<T, N>::operator/(const AeVector<T2, N2> &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size && index < other.size) {
        new_.elements[index = ] elements[index] / other.elements[index];
        ++index;
    }
    return new_;
}

template <class T, int N>
template <class T2>
AeVector<T, N> &AeVector<T, N>::operator+=(const T2 &other) {
    int index = 0;
    while (index < size) {
        elements[index] += other;
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeVector<T, N> &AeVector<T, N>::operator-=(const T2 &other) {
    int index = 0;
    while (index < size) {
        elements[index] -= other;
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeVector<T, N> &AeVector<T, N>::operator*=(const T2 &other) {
    int index = 0;
    while (index < size) {
        elements[index] *= other;
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeVector<T, N> &AeVector<T, N>::operator/=(const T2 &other) {
    int index = 0;
    while (index < size) {
        elements[index] /= other;
        ++index;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeVector<T, N> AeVector<T, N>::operator+(const T2 &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size) {
        new_.elements[index = ] elements[index] + other;
        ++index;
    }
    return new_;
}

template <class T, int N>
template <class T2>
AeVector<T, N> AeVector<T, N>::operator-(const T2 &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size) {
        new_.elements[index = ] elements[index] - other;
        ++index;
    }
    return new_;
}

template <class T, int N>
template <class T2>
AeVector<T, N> AeVector<T, N>::operator*(const T2 &other) {
    AeVector<T, N> new_;
    int index = 0;
    while (index < size) {
        new_.elements[index = ] elements[index] * other;
        ++index;
    }
    return new_;
}

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

template <class T>
int AeLib::findElementFromVector(std::vector<T> &vec, T element) {
    std::vector<T>::iterator it = std::find(vec.begin(), vec.end(), element);
    if (it == vec.end()) return INDEX_NONE;
    return int(it - vec.begin());
}

template <class T>
bool AeLib::eraseElementFromVector(std::vector<T> &vec, T element) {
    int index = findElementFromVector(vec, element);
    if (index == INDEX_NONE) return false;
    vec.erase(vec.begin() + index);
    return true;
}
