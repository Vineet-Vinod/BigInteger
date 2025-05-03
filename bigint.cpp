#include "bigint.h"


bigint::bigint(): bignum(1, 0) {}


bigint::bigint(u_int32_t num): bignum(2, 0)
{
    bignum[0] = num % BASE;
    bignum[1] = num / BASE;
    if (bignum[1] == 0)
        bignum.pop_back();
}


bigint::bigint(const std::string& num): bignum((num.size() - 1) / 9 + 1, 0)
{
    int i, j = 0;
    for (i = num.size(); i >= 9; i -= 9, j++)
        bignum[j] = atoi(num.substr(i-9, 9).c_str());
    bignum[j] = atoi(num.substr(0, i).c_str());
}


bigint::bigint(const std::vector<u_int32_t>& num): bignum(num) {}


bigint::bigint(const bigint &num): bignum(num.bignum) {}


bigint::bigint(bigint &&num): bignum(std::move(num.bignum)) {}


bigint& bigint::operator=(const bigint &num)
{
    if (this != &num)
        this->bignum = num.bignum;
    return *this;
}


bigint& bigint::operator=(bigint &&num)
{
    if (this != &num)
        this->bignum = std::move(num.bignum);
    return *this;
}


bigint::~bigint() { }


inline bigint bigint::operator+(const bigint &num) const
{
    bigint ret(*this);
    ret += num;
    return ret;
}


inline bigint bigint::operator-(const bigint &num) const
{
    bigint ret(*this);
    ret -= num;
    return ret;
}


inline bigint bigint::operator*(const bigint &num) const
{
    bigint ret(*this);
    ret *= num;
    return ret;
}


inline bigint bigint::operator/(const bigint &num) const
{
    bigint ret(*this);
    ret /= num;
    return ret;
}


inline bigint bigint::operator%(const bigint &num) const
{
    bigint ret(*this);
    ret %= num;
    return ret;
}


bool bigint::operator==(const bigint &num) const
{
    if (num.bignum.size() != this->bignum.size())
        return false;

    for (size_t l1 = 0, l2 = 0; l1 < num.bignum.size() && l2 < this->bignum.size(); l1++, l2++)
    {
        if (num.bignum[l1] != this->bignum[l2])
            return false;
    }

    return true;
}


inline bool bigint::operator!=(const bigint &num) const
{
    return !(*this == num);
}


inline bool bigint::operator>=(const bigint &num) const
{
    return !(*this < num);
}


inline bool bigint::operator<=(const bigint &num) const
{
    return !(*this > num);
}


bool bigint::operator>(const bigint &num) const
{
    if (num.bignum.size() > this->bignum.size())
        return false;

    if (num.bignum.size() < this->bignum.size())
        return true;

    for (size_t l = num.bignum.size() - 1; l >= 0; l--)
    {
        if (num.bignum[l] > this->bignum[l])
            return false;
        
        if (num.bignum[l] < this->bignum[l])
            return true;
    }

    return false;
}


bool bigint::operator<(const bigint &num) const
{
    if (num.bignum.size() < this->bignum.size())
        return false;

    if (num.bignum.size() > this->bignum.size())
        return true;

    for (size_t l = num.bignum.size() - 1; l >= 0; l--)
    {
        if (num.bignum[l] < this->bignum[l])
            return false;
        
        if (num.bignum[l] > this->bignum[l])
            return true;
    }

    return false;
}


bigint& bigint::operator+=(const bigint &num)
{
    size_t l1, l2, carry;
    for (l1 = 0, l2 = 0, carry = 0; l1 < num.bignum.size() && l2 < this->bignum.size(); l1++, l2++)
    {
        this->bignum[l2] += num.bignum[l1] + carry;
        carry = this->bignum[l2] >= BASE;
        if (carry)
            this->bignum[l2] -= BASE;
    }

    for (; l1 < num.bignum.size(); l1++, l2++)
    {
        this->bignum.push_back(num.bignum[l1] + carry);
        carry = this->bignum[l2] >= BASE;
        if (carry)
            this->bignum[l2] -= BASE;
    }

    for (; l2 < this->bignum.size(); l2++)
    {
        this->bignum[l2] += carry;
        carry = this->bignum[l2] >= BASE;
        if (carry)
            this->bignum[l2] -= BASE;
    }

    if (carry)
        this->bignum.emplace_back(1);

    return *this;
}


bigint& bigint::operator-=(const bigint &num)
{
    assert(*this >= num);
    if (*this == num)
    {
        this->bignum.clear();
        this->bignum.emplace_back(0);
        return *this;
    }

    size_t l, borrow, nborrow;
    for (l = 0, borrow = 0, nborrow = 0; l < num.bignum.size(); l++, borrow = nborrow)
    {
        if (this->bignum[l] < borrow + num.bignum[l])
        {
            this->bignum[l] += BASE;
            nborrow = 1;
        }
        else
            nborrow = 0;
        this->bignum[l] -= borrow + num.bignum[l];
    }

    for (; l < this->bignum.size(); l++, borrow = nborrow)
    {
        if (this->bignum[l] < borrow)
        {
            this->bignum[l] += BASE;
            nborrow = 1;
        }
        else
            nborrow = 0;
        this->bignum[l] -= borrow;
    }

    while (this->bignum.size() > 1 && this->bignum[this->bignum.size() - 1] == 0)
        this->bignum.pop_back();

    return *this;
}


bigint& bigint::operator*=(const bigint &num)
{
    // Grade school algo (might do Karatsuba later...)
    // Need Karatsuba for > 10000 digits (really notice when > 100000 digits)
    u_int64_t shift = 0;
    bigint orig(*this);
    this->bignum.clear();

    for (const u_int32_t &digit: num.bignum)
    {
        bigint cp(orig);
        cp *= shift << 32 | digit;
        *this += cp;
        shift++;
    }

    return *this;
}


bigint& bigint::operator/=(const bigint &num)
{
    return *this;
}


bigint& bigint::operator%=(const bigint &num)
{
    return *this;
}


std::ostream& operator<<(std::ostream &o, const bigint &num)
{
    auto it = num.bignum.rbegin();
    o << *(it++);
    while (it != num.bignum.rend())
        o << std::setfill('0') << std::setw(9) << *(it++);
    return o;
}


bigint& bigint::operator*=(u_int64_t num)
{
    u_int32_t carry = 0, shift = num >> 32;
    std::vector<u_int32_t> product(shift, 0);
    num &= (1LL << 32) - 1;

    for (u_int64_t l = 0, prod = 0; l < this->bignum.size(); l++)
    {
        prod = this->bignum[l] * num + carry;
        product.emplace_back(prod % BASE);
        carry = prod / BASE;
    }

    if (carry)
        product.emplace_back(carry);

    this->bignum = std::move(product);
    return *this;
}


bigint& bigint::operator/=(u_int64_t num)
{
    uint64_t pmod = 0;
    for (auto iter = this->bignum.rbegin(); iter != this->bignum.rend(); iter++)
    {
        pmod *= bigint::BASE;
        pmod += *iter;
        pmod %= num;
    }

    *this = bigint((uint32_t) pmod);
    return *this;
}


bigint& bigint::operator%=(u_int64_t num)
{
    uint64_t pmod = 0;
    for (auto iter = this->bignum.rbegin(); iter != this->bignum.rend(); iter++)
    {
        pmod *= bigint::BASE;
        pmod += *iter;
        pmod %= num;
    }

    *this = bigint((uint32_t) pmod);
    return *this;
}
