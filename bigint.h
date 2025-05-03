#ifndef __BIGINT_H__
#define __BIGINT_H__


#include "vector"
#include "string"
#include "iostream"
#include "fstream"
#include "iomanip"
#include "assert.h"
#include "string.h"


// Deals with only unsigned ints
class bigint
{
private:
    std::vector<u_int32_t> bignum;
    static const int BASE = 1000000000;
    bigint& operator*=(u_int64_t num);
    bigint& operator/=(u_int64_t num);
    bigint& operator%=(u_int64_t num);
public:
    bigint();                                 // Default constructor
    bigint(u_int32_t num);                    // int constructor
    bigint(const std::string& num);           // string constructor
    bigint(const std::vector<u_int32_t>& num);// vector constructor
    bigint(const bigint &num);                // copy constructor
    bigint(bigint &&num);                     // move constructor
    bigint& operator=(const bigint &num);     // copy assignment
    bigint& operator=(bigint &&num);          // move assignment
    ~bigint();                                // Destructor

    inline bigint operator+(const bigint &num) const;
    inline bigint operator-(const bigint &num) const;
    inline bigint operator*(const bigint &num) const;
    inline bigint operator/(const bigint &num) const;
    inline bigint operator%(const bigint &num) const;

    bool operator==(const bigint &num) const;
    inline bool operator!=(const bigint &num) const;
    inline bool operator>=(const bigint &num) const;
    inline bool operator<=(const bigint &num) const;
    bool operator>(const bigint &num) const;
    bool operator<(const bigint &num) const;

    bigint& operator+=(const bigint &num);
    bigint& operator-=(const bigint &num);
    bigint& operator*=(const bigint &num);
    bigint& operator/=(const bigint &num);
    bigint& operator%=(const bigint &num);

    friend std::ostream& operator<<(std::ostream &o, const bigint &num);
};


#endif
