#include "bigint.h"
#define all(v) v.begin(), v.end()


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
    if (i)
        bignum[j] = atoi(num.substr(0, i).c_str());
}


bigint::bigint(const std::vector<u_int32_t>& num): bignum(num) {}


bigint::bigint(const bigint &num): bignum(num.bignum) {}


bigint::bigint(bigint &&num) noexcept: bignum(std::move(num.bignum)) {}


bigint& bigint::operator=(const bigint &num)
{
    if (this != &num)
        this->bignum = num.bignum;
    return *this;
}


bigint& bigint::operator=(bigint &&num) noexcept
{
    if (this != &num)
        this->bignum = std::move(num.bignum);
    return *this;
}


bigint::~bigint() { }


bigint bigint::operator+(const bigint &num) const
{
    bigint ret(*this);
    ret += num;
    return ret;
}


bigint bigint::operator-(const bigint &num) const
{
    bigint ret(*this);
    ret -= num;
    return ret;
}


bigint bigint::operator*(const bigint &num) const
{
    bigint ret(*this);
    ret *= num;
    return ret;
}


bigint bigint::operator/(const bigint &num) const
{
    bigint ret(*this);
    ret /= num;
    return ret;
}


bigint bigint::operator%(const bigint &num) const
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


bool bigint::operator!=(const bigint &num) const
{
    return !(*this == num);
}


bool bigint::operator>=(const bigint &num) const
{
    return !(*this < num);
}


bool bigint::operator<=(const bigint &num) const
{
    return !(*this > num);
}


bool bigint::operator>(const bigint &num) const
{
    if (num.size() > this->size())
        return false;

    if (num.size() < this->size())
        return true;

    for (size_t l = num.size() - 1; l >= 0; l--)
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
    if (num.size() < this->size())
        return false;

    if (num.size() > this->size())
        return true;

    for (size_t l = num.size() - 1; l >= 0; l--)
    {
        if (num.bignum[l] < this->bignum[l])
            return false;
        
        if (num.bignum[l] > this->bignum[l])
            return true;
    }

    return false;
}


bigint& bigint::operator+=(const bigint &addend)
{
    bigint num(addend);
    while (bignum.size() > 1 && !bignum[bignum.size()-1])
        bignum.pop_back();
    while (num.bignum.size() > 1 && !num.bignum[num.bignum.size()-1])
        num.bignum.pop_back();

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


bigint& bigint::operator-=(const bigint &sub)
{
    assert(*this >= sub);
    if (*this == sub)
    {
        this->bignum.clear();
        this->bignum.emplace_back(0);
        return *this;
    }

    bigint num(sub);
    while (bignum.size() > 1 && !bignum[bignum.size()-1])
        bignum.pop_back();
    while (num.bignum.size() > 1 && !num.bignum[num.bignum.size()-1])
        num.bignum.pop_back();

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
    // Karatsuba for > 100 'digits' (each 'digit' is a 10 digit number)
    if (bignum.size() < 100)
        return regular_multiplication(num);

    *this = std::move(karatsuba(*this, num));
    return *this;
}


bigint bigint::karatsuba(bigint mul1, bigint mul2)
{
    if (mul1.bignum.size() == 0)
        return bigint(0);

    if (mul1.bignum.size() < mul2.bignum.size())
        for (u_int64_t i = 0; i < mul2.bignum.size() - mul1.bignum.size(); i++)
            mul1.bignum.emplace_back(0);

    for (u_int64_t i = 0; i < mul1.bignum.size() - mul2.bignum.size(); i++)
        mul2.bignum.emplace_back(0);

    if (mul1.bignum.size() == 1)
    {
        bigint res(mul1.bignum[0]);
        res *= mul2.bignum[0];
        return res;
    }

    u_int64_t shift = mul1.bignum.size() >> 1;
    bigint lm1(std::vector<u_int32_t>(mul1.bignum.begin(), mul1.bignum.begin() + shift));
    bigint lm2(std::vector<u_int32_t>(mul2.bignum.begin(), mul2.bignum.begin() + shift));
    bigint rm1(std::vector<u_int32_t>(mul1.bignum.begin() + shift, mul1.bignum.end()));
    bigint rm2(std::vector<u_int32_t>(mul2.bignum.begin() + shift, mul2.bignum.end()));

    bigint lhalf(karatsuba(lm1, lm2));
    bigint rhalf(karatsuba(rm1, rm2));
    bigint top(lm1 + rm1);
    bigint bottom(lm2 + rm2);
    bigint middle(karatsuba(top, bottom) - lhalf - rhalf);

    // Shift middle
    std::reverse(all(middle.bignum));
    for (u_int64_t i = 0; i < shift; i++)
        middle.bignum.emplace_back(0);
    std::reverse(all(middle.bignum));

    // Shift right half
    std::reverse(all(rhalf.bignum));
    for (u_int64_t i = 0; i < shift << 1; i++)
        rhalf.bignum.emplace_back(0);
    std::reverse(all(rhalf.bignum));

    return lhalf + middle + rhalf;
}


bigint& bigint::regular_multiplication(const bigint &num)
{
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


u_int64_t bigint::size() const
{
    u_int64_t ret = bignum.size();
    for (u_int64_t i = ret - 1; i > 0; i--)
    {
        if (bignum[i])
            break;
        else
            ret--;
    }
    return ret;
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

