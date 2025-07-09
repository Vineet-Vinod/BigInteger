#ifndef __BIGINT_H__
#define __BIGINT_H__


#include "vector"
#include "string"
#include "deque"
#include "iostream"
#include "fstream"
#include "iomanip"
#include "cassert"
#include "cstring"

#define all(v) v.begin(), v.end()

class bigint
{
private:
    bool neg;
    std::vector<u_int32_t> bignum;
    static const int BASE = 1'000'000'000;

    bool abs_greater_than(const bigint &num) const;
    bool abs_lesser_than(const bigint &num) const;

    static void add_with_shift(bigint &a, const bigint& b, u_int64_t sb);
    static bigint _add_split(const bigint &a, u_int32_t st, u_int32_t end, u_int32_t split);
    static void _sub(std::vector<u_int32_t> &a, const std::vector<u_int32_t> &b);
    static void sub(bigint &a, const bigint &b);
    static bigint multiply(const bigint &mul1, const bigint &mul2, u_int32_t m1_st, u_int32_t m1_end, u_int32_t m2_st, u_int32_t m2_end);
    static void regular_multiplication(bigint &mul, const bigint &num);
    bigint& operator*=(u_int32_t num);
    bigint div_mod(const bigint &dividend, const bigint &divisor, bool div);
    static u_int32_t div(bigint &dividend, bigint &div_cpy, const bigint &divisor, bool shift);
    static void mul_dig_in_place(bigint &prod, const bigint &orig, u_int32_t digit);

    void pop_leading_zeros();
    void reverse_num();

public:
    bigint();
    bigint(int64_t num);
    bigint(const std::string& num);
    bigint(const bigint &num);
    bigint(const bigint &num, u_int32_t st, u_int32_t end);
    bigint(const std::vector<u_int32_t> bnum, bool sign = false);
    bigint(bigint &&num) noexcept;
    bigint& operator=(const bigint &num);
    bigint& operator=(bigint &&num) noexcept;
    ~bigint();
    
    u_int64_t num_digits() const;
    friend std::ostream& operator<<(std::ostream &o, const bigint &num);

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
};


#endif
