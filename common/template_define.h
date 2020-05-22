#pragma once

#include <sstream>

template <class T, int N>
AeArray<T, N>::AeArray() {
    std::memset(elements, 0, sizeof elements);
}

template <class T, int N>
AeArray<T, N>::AeArray(std::initializer_list<T> l) {
    int index = 0;
    for (T v : l) {
        elements[index] = v;
        ++index;
    }
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N>::AeArray(const AeArray<T2, N2> &other) {
    *this = other;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N>::AeArray(const AeArray<T2, N2> &other, T value) {
    *this = other;
    if (N > N2) {
        for (int i = N2; i < N; ++i) {
            elements[i] = value;
        }
    }
}

template <class T, int N>
template <class T2, int N2>
bool AeArray<T, N>::operator==(const AeArray<T2, N2> &other) const {
    for (int i = 0; i < N && i < N2; ++i) {
        if (elements[i] != other.elements[i]) {
            return false;
        }
    }
    return true;
}

template <class T, int N>
template <class T2, int N2>
bool AeArray<T, N>::operator!=(const AeArray<T2, N2> &other) const {
    for (int i = 0; i < N && i < N2; ++i) {
        if (elements[i] == other.elements[i]) {
            return false;
        }
    }
    return true;
}

template <class T, int N>
T &AeArray<T, N>::operator[](int index) {
    return elements[index];
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> &AeArray<T, N>::operator=(const AeArray<T2, N2> &other) {
    for (int i = 0; i < N && i < N2; ++i) {
        elements[i] = other.elements[i];
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> &AeArray<T, N>::operator+=(const AeArray<T2, N2> &other) {
    for (int i = 0; i < N && i < N2; ++i) {
        elements[i] += other.elements[i];
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> &AeArray<T, N>::operator-=(const AeArray<T2, N2> &other) {
    for (int i = 0; i < N && i < N2; ++i) {
        elements[i] -= other.elements[i];
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> &AeArray<T, N>::operator*=(const AeArray<T2, N2> &other) {
    for (int i = 0; i < N && i < N2; ++i) {
        elements[i] *= other.elements[i];
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> &AeArray<T, N>::operator/=(const AeArray<T2, N2> &other) {
    for (int i = 0; i < N && i < N2; ++i) {
        elements[i] /= other.elements[i];
    }
    return *this;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> AeArray<T, N>::operator+(const AeArray<T2, N2> &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N && i < N2; ++i) {
        new_.elements[i] = elements[i] + other.elements[i];
    }
    return new_;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> AeArray<T, N>::operator-(const AeArray<T2, N2> &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N && i < N2; ++i) {
        new_.elements[i] = elements[i] - other.elements[i];
    }
    return new_;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> AeArray<T, N>::operator*(const AeArray<T2, N2> &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N && i < N2; ++i) {
        new_.elements[i] = elements[i] * other.elements[i];
    }
    return new_;
}

template <class T, int N>
template <class T2, int N2>
AeArray<T, N> AeArray<T, N>::operator/(const AeArray<T2, N2> &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N && i < N2; ++i) {
        new_.elements[i] = elements[i] / other.elements[i];
    }
    return new_;
}

template <class T, int N>
template <class T2>
AeArray<T, N> &AeArray<T, N>::operator=(const T2 &other) {
    for (int i = 0; i < N; ++i) {
        elements[i] = other;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeArray<T, N> &AeArray<T, N>::operator+=(const T2 &other) {
    for (int i = 0; i < N; ++i) {
        elements[i] += other;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeArray<T, N> &AeArray<T, N>::operator-=(const T2 &other) {
    for (int i = 0; i < N; ++i) {
        elements[i] -= other;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeArray<T, N> &AeArray<T, N>::operator*=(const T2 &other) {
    for (int i = 0; i < N; ++i) {
        elements[i] *= other;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeArray<T, N> &AeArray<T, N>::operator/=(const T2 &other) {
    for (int i = 0; i < N; ++i) {
        elements[i] /= other;
    }
    return *this;
}

template <class T, int N>
template <class T2>
AeArray<T, N> AeArray<T, N>::operator+(const T2 &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N; ++i) {
        new_.elements[i] = elements[i] + other;
    }
    return new_;
}

template <class T, int N>
template <class T2>
AeArray<T, N> AeArray<T, N>::operator-(const T2 &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N; ++i) {
        new_.elements[i] = elements[i] - other;
    }
    return new_;
}

template <class T, int N>
template <class T2>
AeArray<T, N> AeArray<T, N>::operator*(const T2 &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N; ++i) {
        new_.elements[i] = elements[i] * other;
    }
    return new_;
}

template <class T, int N>
template <class T2>
AeArray<T, N> AeArray<T, N>::operator/(const T2 &other) {
    AeArray<T, N> new_;
    for (int i = 0; i < N; ++i) {
        new_.elements[i] = elements[i] / other;
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

template <class T>
T QeEncode::ConvertTo(const std::string &str) {
    if (str.empty()) return 0;
    std::stringstream ss(str);
    T num;
    ss >> num;
    return num;
}

template <>
const char *QeEncode::ConvertTo<const char *>(const std::string &str) {
    return str.c_str();
}

template <>
std::string QeEncode::ConvertTo<std::string>(const std::string &str) {
    return str;
}

template <class T>
std::vector<T> QeEncode::split(std::string s, std::string delim) {
    std::vector<T> tokens;
    /*char dup[4096];
    strncpy_s(dup, s, 4096);
    char *context = NULL;
    char *token = strtok_s(dup, delim, &context);
    while (token != NULL) {
        tokens.push_back(std::string(token));
        token = strtok_s(NULL, delim, &context);
    }*/

    size_t pos = 0;
    while ((pos = s.find(delim)) != std::string::npos) {
        tokens.push_back(ConvertTo<T>(s.substr(0, pos)));
        s.erase(0, pos + delim.length());
    }
    tokens.push_back(ConvertTo<T>(s));
    return tokens;
}

template <class T>
std::string QeEncode::combine(std::vector<T>& ss, std::string delim) {
    if (ss.empty()) return std::string();
    std::string ret = ss[0];
    for (size_t i=1; i< ss.size() ;++i) {
        ret += (delim + ss[i]);
    }
    return ret;
}

template <class T>
T AeXMLNode::getXMLValue(const char *key) {
    T ret;
    getXMLValue<T>(ret, key);
    return ret;
}

template <class T>
AeXMLNode *AeXMLNode::getXMLValue(T &value, const char *key) {
    std::memset((void *)&value, 0, sizeof value);
    auto keys = ENCODE.split<std::string>(key, ".");
    std::vector<std::string> keys1 = keys;
    keys1.pop_back();
    AeXMLNode *current = getXMLNode(keys1);
    if (!current) return nullptr;

    std::string final_key = keys.back();

    for (const auto &node : current->data->elements) {
        if (final_key.compare(node.key) == 0) {
            value = ENCODE.ConvertTo<T>(node.value);
            return current;
        }
    }

    current = getXMLNode(final_key.c_str());
    if (!current) return nullptr;
    if (final_key.compare(current->data->key.c_str()) == 0) {
        value = ENCODE.ConvertTo<T>(current->data->value);
        return current;
    }
    return nullptr;
}

template <class T, int N>
AeArray<T, N> AeXMLNode::getXMLValues(const char *key) {
    AeArray<T, N> ret;
    getXMLValues<T, N>(ret, key);
    return ret;
}

template <class T, int N>
AeXMLNode *AeXMLNode::getXMLValues(AeArray<T, N> &value, const char *key) {
    std::string str_values;
    AeXMLNode *ret = getXMLValue<std::string>(str_values, key);
    auto values = ENCODE.split<T>(str_values, " ");
    for (int i = 0; i < N && i < values.size(); ++i) {
        value.elements[i] = values[i];
    }
    return ret;
}
/*
template <>
int AeMath::random<int>(int start, int range) {
    if (!range) return start;
    std::random_device rd;
    std::default_random_engine gen = std::default_random_engine(rd());
    std::uniform_real_distribution<int> dis(start, start + range);
    return dis(gen);
}
template <>
float AeMath::random<float>(float start, float range) {
    if (!range) return start;
    std::random_device rd;
    std::default_random_engine gen = std::default_random_engine(rd());
    std::uniform_real_distribution<float> dis(start, start + range);
    return dis(gen);
}
*/

template <class T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type AeMath::random(T start, T range) {
    if (!range) return start;
    std::random_device rd;
    std::default_random_engine gen = std::default_random_engine(rd());
    std::uniform_real_distribution<T> dis(start, start + range);
    return dis(gen);
}

template <class T>
typename std::enable_if<std::is_integral<T>::value, T>::type AeMath::random(T start, T range) {
    if (!range) return start;
    std::random_device rd;
    std::default_random_engine gen = std::default_random_engine(rd());
    std::uniform_int_distribution<T> dis(start, start + range);
    return dis(gen);
}

template <class T, int N>
AeArray<T, N> AeMath::randoms(T start, T range) {
    AeArray<T, N> ret;
    if (!range) {
        ret = start;
        return ret;
    }
    std::random_device rd;
    std::default_random_engine gen = std::default_random_engine(rd());
    //std::uniform_int_distribution<int> dis(start, start + range);
    std::uniform_real_distribution<T> dis(start, start + range);

    for (int i = 0; i < size; ++i) ret[i] = dis(gen);
    return ret;
}

template <int N>
float AeMath::dot(AeArray<float, N> &vec1, AeArray<float, N> &vec2) {
    float ret = 0.f;
    for (int i = 0; i < N; ++i) {
        ret += (vec1.elements[i] * vec2.elements[i]);
    }
    return ret;
}

template <int N>
float AeMath::length(AeArray<float, N> &vec) {
    return fastSqrt(dot<N>(vec, vec));
}

template <int N>
AeArray<float, N> AeMath::normalize(AeArray<float, N> &_vec) {
    return _vec / length<N>(_vec);
}
