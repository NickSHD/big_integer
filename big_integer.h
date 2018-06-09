#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <functional>
#include <string>
#include "my_vector.h"

typedef unsigned int ui;
typedef unsigned long long ull;

struct big_integer {

private:
    bool getSign() const;

    bool getSignWithoutBack() const;

    ui getEmptyCell() const;

    void relax();

    void shiftCells(int rhs);

public:
    big_integer();

    big_integer(big_integer const &other);

    big_integer(int a);

    big_integer(ui a);

    explicit big_integer(std::string const &str);

    ~big_integer();

    big_integer &operator+=(big_integer const &rhs);

    big_integer &operator-=(big_integer const &rhs);

    big_integer &operator*=(big_integer const &rhs);

    big_integer &operator/=(big_integer const &rhs);

    big_integer &operator%=(big_integer const &rhs);

    big_integer &apply_bit_operation(big_integer const &rhs, const std::function<ui(ui, ui)> func);

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    const big_integer operator++(int);

    big_integer &operator--();

    const big_integer operator--(int);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend std::string to_string(big_integer const &a);

    friend std::pair<big_integer, ui> myDiv(big_integer const &a, ui const &b);

    friend big_integer operator*(big_integer const &a, ui const &b);

    friend ui getTrial(ui const &a, ui const &b, ui const &c);

private:
    ui_vector data;

    static const ui kLogBase;
    static const ull kBase;
    static const ui maxNumber;
};

big_integer operator+(big_integer a, big_integer const &b);

big_integer operator-(big_integer a, big_integer const &b);

big_integer operator*(big_integer a, big_integer const &b);

big_integer operator/(big_integer a, big_integer const &b);

big_integer operator%(big_integer a, big_integer const &b);

big_integer operator&(big_integer a, big_integer const &b);

big_integer operator|(big_integer a, big_integer const &b);

big_integer operator^(big_integer a, big_integer const &b);

big_integer operator<<(big_integer a, int b);

big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const &a, big_integer const &b);

bool operator!=(big_integer const &a, big_integer const &b);

bool operator<(big_integer const &a, big_integer const &b);

bool operator>(big_integer const &a, big_integer const &b);

bool operator<=(big_integer const &a, big_integer const &b);

bool operator>=(big_integer const &a, big_integer const &b);

std::string to_string(big_integer const &a);

std::ostream &operator<<(std::ostream &s, big_integer const &a);

std::istream &operator>>(std::istream &s, big_integer &a);

std::pair<big_integer, ui> myDiv(big_integer const &a, ui const &b);

big_integer operator*(big_integer const &a, ui const &b);

ui getTrial(ui const &a, ui const &b, ui const &c);

#endif // BIG_INTEGER_H