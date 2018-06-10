#include "my_vector.h"
#include <cassert>

typedef unsigned int ui;
using std::shared_ptr;

bool ui_vector::is_big() const {
    return _size > 1;
}

ui_vector::ui_vector() {
    small = 0;
    _size = 0;
}

ui_vector::~ui_vector() {
    if (is_big()) {
        big.reset();
    }
}

size_t ui_vector::size() const {
    return _size;
}

bool ui_vector::empty() const {
    return _size == 0;
}

ui ui_vector::back() const {
    assert(_size > 0);
    if (is_big()) {
        return big->at(_size - 1);
    }
    return small;
}

ui &ui_vector::back() {
    assert(_size > 0);
    if (is_big()) {
        check_count();
        return big->at(_size - 1);
    }
    return small;
}

void ui_vector::push_back(ui val) {
    if (is_big()) {
        check_count();
        big->push_back(val);
    } else if (_size) {
        new(&big) shared_ptr<std::vector<ui>>(new std::vector<ui>(1, small));
        big->push_back(val);
    } else {
        small = val;
    }
    ++_size;
}

void ui_vector::pop_back() {
    assert(_size > 0);
    if (!is_big()) {
        small = 0;
    } else if (_size > 2) {
        check_count();
        big->pop_back();
    } else {
        ui tmp = big->at(0);
        big.reset();
        small = tmp;
    }
    --_size;
}

void ui_vector::clear() {
    resize(0);
}

void ui_vector::resize(size_t len) {
    if (is_big()) {
        if (len < 2) {
            ui tmp = operator[](0);
            big.reset();
            small = tmp;
        } else {
            check_count();
            big->resize(len);
        }
    } else if (len > 1) {
        new(&big) shared_ptr<std::vector<ui>>(new std::vector<ui>(len, small));
    }
    _size = len;
}

ui ui_vector::operator[](size_t ind) const {
    assert(ind < _size);
    if (is_big()) {
        return big->at(ind);
    }
    return small;
}

ui &ui_vector::operator[](size_t ind) {
    assert(ind < _size);
    if (is_big()) {
        check_count();
        return big->at(ind);
    }
    return small;
}

ui_vector &ui_vector::operator=(ui_vector const &other) {
    if (is_big()) {
        big.reset();
    }
    if (other.is_big()) {
        new(&big) shared_ptr<std::vector<ui>>(other.big);
    } else {
        small = other.small;
    }
    _size = other._size;
    return *this;
}

bool operator==(ui_vector const &a, ui_vector const &b) {
    if (a.is_big() ^ b.is_big()) {
        return false;
    } else if (a.is_big()) {
        return *a.big == *b.big;
    }
    return a.small == b.small;
}

void ui_vector::check_count() {
    if (!big.unique()) {
        big = std::make_shared<std::vector<ui>>(*big);
    }
}