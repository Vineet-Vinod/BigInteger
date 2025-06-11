#ifndef __BIGINT_H__
#define __BIGINT_H__


#include "vector"
#include "string"
#include "iostream"
#include "fstream"
#include "iomanip"
#include "cassert"
#include "cstring"


// Deals with only unsigned ints
class bigint
{
private:
    std::vector<u_int32_t> bignum;
    static const int BASE = 1'000'000'000;
    bigint& operator*=(u_int64_t num);
    bigint& operator/=(u_int64_t num);
    bigint& operator%=(u_int64_t num);
    bigint& regular_multiplication(const bigint &num);
    static bigint karatsuba(bigint mul1, bigint mul2);

public:
    bigint();                                 // Default constructor
    bigint(u_int32_t num);                    // int constructor
    bigint(const std::string& num);           // string constructor
    bigint(const std::vector<u_int32_t>& num);// vector constructor
    bigint(const bigint &num);                // copy constructor
    bigint(bigint &&num) noexcept;            // move constructor
    bigint& operator=(const bigint &num);     // copy assignment
    bigint& operator=(bigint &&num) noexcept; // move assignment
    ~bigint();                                // Destructor

    bigint operator+(const bigint &num) const;
    bigint operator-(const bigint &num) const;
    bigint operator*(const bigint &num) const;
    bigint operator/(const bigint &num) const;
    bigint operator%(const bigint &num) const;

    bool operator==(const bigint &num) const;
    bool operator!=(const bigint &num) const;
    bool operator>=(const bigint &num) const;
    bool operator<=(const bigint &num) const;
    bool operator>(const bigint &num) const;
    bool operator<(const bigint &num) const;

    bigint& operator+=(const bigint &num);
    bigint& operator-=(const bigint &num);
    bigint& operator*=(const bigint &num);
    bigint& operator/=(const bigint &num);
    bigint& operator%=(const bigint &num);

    u_int64_t size() const;
    friend std::ostream& operator<<(std::ostream &o, const bigint &num);
};


#endif
