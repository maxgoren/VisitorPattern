#ifndef object_hpp
#define object_hpp
#include <iostream>
#include <vector>
using namespace std;

enum ObjectType {
    NUMBER, STRING, BOOL, FUNCTION, VECTOR, NIL
};

struct Function;

struct Object {
    ObjectType type;
    union {
        double numval;
        bool boolval;
        std::string* stringval;
        Function* func;
        vector<Object>* vec;
    };
    Object(double v) : type(NUMBER) { numval = v; }
    Object(bool v) : type(BOOL) { boolval = v; }
    Object(string v) : type(STRING) { stringval = new string(v); }
    Object(Function* fn) : type(FUNCTION), func(fn) { }
    Object(vector<Object>* obj) : type(VECTOR), vec(obj) { }
    Object() : type(NIL), numval(0.0) { }
    Object(const Object& ob) { 
        type = ob.type; 
        switch (type) {
            case NUMBER: numval = ob.numval; break;
            case BOOL: boolval = ob.boolval; break;
            case STRING: stringval = ob.stringval; break;
            case FUNCTION: func = ob.func; break;
            case VECTOR: vec = ob.vec; break;
            default: break;
        }
    }
    ~Object() { }
    Object& operator=(const Object& ob) {
        if (this != &ob) {
            type = ob.type; 
            switch (type) {
                case NUMBER: numval = ob.numval; break;
                case BOOL: boolval = ob.boolval; break;
                case STRING: stringval = ob.stringval; break;
                case FUNCTION: func = ob.func; break;
                case VECTOR: vec = ob.vec; break;
                default: break;
            }
        }
        return *this;
    }
};


Object add(Object lhs, Object rhs) {
    double l = lhs.numval;
    double r = rhs.numval;
    return Object(l+r);
}

Object sub(Object lhs, Object rhs) {
    double l = lhs.numval;
    double r = rhs.numval;
    return Object(l-r);
}

Object mul(Object lhs, Object rhs) {
    double l = lhs.numval;
    double r = rhs.numval;
    return Object(l*r);
}

Object div(Object lhs, Object rhs) {
    double l = lhs.numval;
    double r = rhs.numval;
    return Object(l/r);
}

Object eq(Object lhs, Object rhs) {
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval == rhs.numval);
        case STRING: return Object(*lhs.stringval == *rhs.stringval);
        case BOOL:   return Object(lhs.boolval == rhs.boolval);
        case NIL:   return Object(lhs.type == rhs.type);
    }
    return Object(false);
}

Object neq(Object lhs, Object rhs) {
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval != rhs.numval);
        case STRING: return Object(*lhs.stringval != *rhs.stringval);
        case BOOL:   return Object(lhs.boolval != rhs.boolval);
        case NIL:   return Object(lhs.type != rhs.type);
    }
    return Object(false);
}

Object lt(Object lhs, Object rhs) {
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval < rhs.numval);
        case STRING: return Object(*lhs.stringval < *rhs.stringval);
        case BOOL:   return Object(lhs.boolval < rhs.boolval);
        case NIL:   return Object(lhs.type < rhs.type);
    }
    return Object(false);
}

Object gt(Object lhs, Object rhs) {
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval > rhs.numval);
        case STRING: return Object(*lhs.stringval > *rhs.stringval);
        case BOOL:   return Object(lhs.boolval > rhs.boolval);
        case NIL:   return Object(lhs.type > rhs.type);
    }
    return Object(false);
}

Object lte(Object lhs, Object rhs) {
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval <= rhs.numval);
        case STRING: return Object(*lhs.stringval <= *rhs.stringval);
        case BOOL:   return Object(lhs.boolval <= rhs.boolval);
        case NIL:   return Object(lhs.type <= rhs.type);
    }
    return Object(false);
}

Object gte(Object lhs, Object rhs) {
    switch (lhs.type) {
        case NUMBER: return Object(lhs.numval >= rhs.numval);
        case STRING: return Object(*lhs.stringval >= *rhs.stringval);
        case BOOL:   return Object(lhs.boolval >= rhs.boolval);
        case NIL:   return Object(lhs.type >= rhs.type);
    }
    return Object(false);
}

std::ostream& operator<<(ostream& os, const Object& ob) {
    switch (ob.type) {
        case NUMBER: os<<ob.numval; break;
        case BOOL: os<<(ob.boolval ? "true":"false"); break;
        case STRING: os<<*ob.stringval; break;
        case FUNCTION: os<<"(func)"; break;
        case NIL: os<<"nil"; break;
        case VECTOR: {
            os<<"vector, size="<<ob.vec->size()<<", { ";
            for (auto m : *ob.vec) {
                os<<m<<" ";
            } 
            cout<<"}";
        } break;
        default: break;
    }
    return os;
}

Object operator+(const Object& lhs, const Object& rhs) {
    return add(lhs, rhs);
}

#endif