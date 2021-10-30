#pragma once

#define HOVERABLE_NULL 0

class Hoverable {
public:
    Hoverable(unsigned int id) : id(id) {}
    virtual ~Hoverable() = default;

    static unsigned int generate_id() {
        static unsigned int id = 0;
        id++;
        return id;
    }
protected:
    unsigned int id;
};
