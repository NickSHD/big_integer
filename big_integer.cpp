#include "big_integer.h"
#include <algorithm>

const ui big_integer::kLogBase = 32;
const ui big_integer::maxNumber = (ui) 1e9;
const ull big_integer::kBase = (ull) 1 << kLogBase;

bool big_integer::getSign() const {
    return !data.empty() && (data.back() & ((ui) 1 << (kLogBase - 1)));
}

bool big_integer::getSignWithoutBack() const {
    return data.size() > 1 && (data[data.size() - 2] & ((ui) 1 << (kLogBase - 1)));
}

ui big_integer::getEmptyCell() const {
    if (getSign()) {
        return (ui) (kBase - 1);
    }
    return 0;
}

void big_integer::relax() {
    while (!data.empty() && data.back() == getEmptyCell() && getSign() == getSignWithoutBack()) {
        data.pop_back();
    }
}

big_integer::big_integer() {
    data.clear();
}

big_integer::big_integer(big_integer const &other) {
    data = other.data;
}

big_integer::big_integer(int a) {
    data.clear();
    if (a) {
        data.push_back((ui) a);
    }
}

big_integer::big_integer(ui a) {
    data.clear();
    if (a) {
        data.push_back((ui) a);
    }
    if (getSign()) {
        data.push_back(0);
    }
}

big_integer::big_integer(std::string const &str) {
    data.clear();
    bool sign = str[0] == '-';
    ull cur = 0;
    ui pow10 = 1;
    for (char c : str) {
        if (c != '-') {
            if (cur * 10 + (c - '0') < maxNumber && pow10 < maxNumber) {
                cur = cur * 10 + (c - '0');
                pow10 *= 10;
            } else {
                *this = (*this * pow10) + (ui) cur;
                cur = (ull) (c - '0');
                pow10 = 10;
            }
        }
    }
    *this = (*this * pow10) + (ui) cur;
    if (getSign()) {
        data.push_back(0);
    }
    if (sign) {
        *this = -*this;
    }
}

big_integer::~big_integer() {
    data.clear();
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    size_t len = std::max(data.size(), rhs.data.size()) + 1;
    ui emptyCell = getEmptyCell();
    size_t oldSize = data.size();
    data.resize(len);
    while (oldSize < len) {
        data[oldSize++] = emptyCell;
    }
    bool c = false;
    ui a = 0, b = 0;
    for (size_t i = 0; i < len; ++i) {
        bool oldC = c;
        c = (ull) (a = i < data.size() ? data[i] : this->getEmptyCell()) +
            (b = i < rhs.data.size() ? rhs.data[i] : rhs.getEmptyCell()) + c >= kBase;
        data[i] = a + b + oldC;
    }
    relax();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this += -rhs;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer res;
    big_integer a = *this, b = rhs;
    bool firstSign = getSign(), secondSign = rhs.getSign();
    if (firstSign) {
        a = -a;
    }
    if (secondSign) {
        b = -b;
    }
    for (size_t i = 0; i < b.data.size(); ++i) {
        big_integer aux = a * b.data[i] << i * kLogBase;
        res += aux;
    }
    if (firstSign ^ secondSign) {
        res = -res;
    }
    res.relax();
    return *this = res;
}

ui getNumber(ui_vector const &a, size_t const &ind) {
    return ind < a.size() ? a[ind] : (ui) 0;
}

ui getTrial(ui const &a, ui const &b, ui const &c) {
    ull res = (((ull) a << big_integer::kLogBase) + b) / c;
    if (res > big_integer::kBase - 1) {
        res = big_integer::kBase - 1;
    }
    return (ui) res;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    big_integer res;
    big_integer a = *this, b = rhs;
    bool firstSign = getSign(), secondSign(rhs.getSign());
    if (firstSign) {
        a = -a;
    }
    if (secondSign) {
        b = -b;
    }
    if (a < b) {
        return *this = 0;
    }
    if (b.data.size() == 1) {
        res = myDiv(a, b.data[0]).first;
    } else {
        size_t m = b.data.size();
        while (b.data[m - 1] == 0) {
            --m;
        }
        ui J = b.data[m - 1];
        auto f = (ui) (big_integer::kBase / (J + 1));
        a *= f, b *= f;
        a.relax();
        b.relax();
        size_t n = a.data.size();
        m = b.data.size();

        while (b.data[m - 1] == 0) {
            --m;
        }
        ui divisor = b.data[m - 1];

        big_integer cur = a >> kLogBase * (n - m + 1), aux;
        res.data.resize(n - m + 1);

        for (size_t i = n - m + 1; i > 0; --i) {
            cur = (cur << kLogBase) + a.data[i - 1];
            ui tq = getTrial(getNumber(cur.data, m), getNumber(cur.data, m - 1), divisor);
            aux = b * tq;
            while (tq > 0 && cur < aux) {
                aux = b * (--tq);
            }
            cur -= aux;
            res.data[i - 1] = tq;
        }
    }

    res.relax();
    if (firstSign ^ secondSign) {
        res = -res;
    }
    return *this = res;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return *this -= *this / rhs * rhs;
}

big_integer &big_integer::apply_bit_operation(big_integer const &rhs, const std::function<ui(ui, ui)> func) {
    size_t len = std::max(data.size(), rhs.data.size());
    ui emptyCell = getEmptyCell();
    size_t oldSize = data.size();
    data.resize(len);
    while (oldSize < len) {
        data[oldSize++] = emptyCell;
    }
    for (size_t i = 0; i < len; ++i) {
        data[i] = func(i < data.size() ? data[i] : this->getEmptyCell(),
                       i < rhs.data.size() ? rhs.data[i] : rhs.getEmptyCell());
    }
    relax();
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return apply_bit_operation(rhs, [](ui a, ui b) { return a & b; });
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return apply_bit_operation(rhs, [](ui a, ui b) { return a | b; });
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return apply_bit_operation(rhs, [](ui a, ui b) { return a ^ b; });
}

void big_integer::shiftCells(int rhs) {
    if (rhs > 0) {
        size_t oldSize = data.size();
        data.resize(data.size() + rhs);
        for (size_t i = oldSize; i > 0; --i) {
            data[i + rhs - 1] = data[i - 1];
        }
        for (auto i = (size_t) rhs; i > 0; --i) {
            data[i - 1] = 0;
        }
    } else {
        for (auto i = (size_t) -rhs; i < data.size(); ++i) {
            data[i + rhs] = data[i];
        }
        for (size_t i = data.size(); i > data.size() + rhs; --i) {
            data[i - 1] = getEmptyCell();
        }
        relax();
    }
}

big_integer &big_integer::operator<<=(int rhs) {
    if (rhs < 0) {
        return *this >>= -rhs;
    }
    int auxShift = rhs / kLogBase;
    if (auxShift) {
        shiftCells(auxShift);
    }
    auxShift = rhs - auxShift * kLogBase;
    if (auxShift) {
        data.push_back(getEmptyCell());
        for (size_t i = data.size(); i > 0; --i) {
            if (i != data.size()) {
                data[i] += data[i - 1] >> (kLogBase - auxShift);
            }
            data[i - 1] <<= auxShift;
        }
    }
    relax();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs < 0) {
        return *this <<= -rhs;
    }
    int auxShift = rhs / kLogBase;
    if (auxShift) {
        shiftCells(-auxShift);
    }
    auxShift = rhs - auxShift * kLogBase;
    if (auxShift) {
        ui cur = getEmptyCell();
        for (size_t i = 0; i < data.size(); ++i) {
            if (i != 0) {
                data[i - 1] += data[i] << (kLogBase - auxShift);
            }
            data[i] >>= auxShift;
        }
        data.back() += cur << (kLogBase - auxShift);
    }
    relax();
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    return ~*this + 1;
}

big_integer big_integer::operator~() const {
    big_integer r = *this;
    if (data.empty()) {
        r.data.push_back(0);
    }
    for (size_t i = 0; i < r.data.size(); ++i) {
        r.data[i] = ~r.data[i];
    }
    r.relax();
    return r;
}

big_integer &big_integer::operator++() {
    return *this += 1;
}

const big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer &big_integer::operator--() {
    return *this -= 1;
}

const big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return a.data == b.data;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a.data == b.data);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.getSign() != b.getSign()) {
        return a.getSign();
    }
    if (a.data.size() != b.data.size()) {
        return a.data.size() < b.data.size();
    }
    for (size_t i = a.data.size(); i > 0; --i) {
        if (a.data[i - 1] != b.data[i - 1]) {
            return a.data[i - 1] < b.data[i - 1];
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(b < a);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

std::pair<big_integer, ui> myDiv(big_integer const &a, ui const &b) {
    big_integer res = a;
    ui c = 0;
    for (size_t i = res.data.size(); i > 0; --i) {
        ull cur = big_integer::kBase * c + res.data[i - 1];
        res.data[i - 1] = (ui) (cur / b);
        c = (ui) (cur % b);
    }
    res.relax();
    return {res, c};
}

big_integer operator*(big_integer const &a, ui const &b) {
    big_integer res = a;
    bool sign = res.getSign();
    if (sign) {
        res = -res;
    }
    res.data.push_back(0);
    ui c = 0;
    for (size_t i = 0; i < res.data.size(); ++i) {
        ull cur = (ull) res.data[i] * b + c;
        res.data[i] = (ui) (cur & (big_integer::kBase - 1));
        c = (ui) (cur >> big_integer::kLogBase);
    }
    if (sign) {
        res = -res;
    }
    res.relax();
    return res;
}

std::string to_string(big_integer const &a) {
    std::string res;
    big_integer b = a;
    if (b.getSign()) {
        res += '-';
        b = -b;
    }
    std::vector<std::string> aux(b.data.size() * 2);
    size_t iA = 0;
    while (b > 0) {
        auto Y = myDiv(b, big_integer::maxNumber);
        aux[iA++] = std::to_string(Y.second);
        b = Y.first;
    }
    for (size_t i = iA; i > 0; --i) {
        if (i != iA) {
            res += std::string(9 - aux[i - 1].size(), '0');
        }
        res += aux[i - 1];
    }
    if (res.empty()) {
        res = "0";
    }
    return res;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}

std::istream &operator>>(std::istream &s, big_integer &a) {
    std::string aux;
    s >> aux;
    a = big_integer(aux);
    return s;
}