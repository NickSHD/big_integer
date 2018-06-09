#ifndef MY_UI_VECTOR
#define MY_UI_VECTOR

#include <memory>
#include <vector>

struct ui_vector {
    typedef unsigned int ui;

public:
    ui_vector();

    ~ui_vector();

    size_t size() const;

    bool empty() const;

    ui back() const;

    ui &back();

    void push_back(ui val);

    void pop_back();

    void clear();

    void resize(size_t len);

    ui operator[](size_t ind) const;

    ui &operator[](size_t ind);

    ui_vector &operator=(ui_vector const &other);

    friend bool operator==(ui_vector const &a, ui_vector const &b);

private:
    union {
        std::shared_ptr<std::vector<ui>> big;
        ui small;
    };

    size_t _size;

    bool is_big() const;

    void check_count();
};

bool operator==(ui_vector const &a, ui_vector const &b);

#endif