#include "bigint.h"


/*Constructors, Destructors and Assignment*/
bigint::bigint() : neg(false), bignum(1, 0) {}

bigint::bigint(int64_t num) : neg(num < 0)
{
    num = abs(num);
    for (; num; num /= bigint::BASE)
        bignum.emplace_back(num % bigint::BASE);

    if (num_digits() == 0)
        bignum.emplace_back(0);
}

bigint::bigint(const std::string &num) : neg(num[0] == '-'), bignum((num.size() - (num[0] == '-' ? 2 : 1)) / 9 + 1, 0)
{
    int i, j = 0;
    for (i = num.size(); i >= (neg ? 10 : 9); i -= 9, j++)
        bignum[j] = atoi(num.substr(i - 9, 9).c_str());
    if (i)
        bignum[j] = atoi(num.substr(neg, neg ? i - 1 : i).c_str());
}

bigint::bigint(const bigint &num) : neg(num.neg), bignum(num.bignum) {}

bigint::bigint(const bigint &num, u_int32_t st, u_int32_t end) : neg(num.neg), bignum(num.bignum.begin() + st, num.bignum.begin() + end) {}

bigint::bigint(bigint &&num) noexcept : neg(num.neg), bignum(std::move(num.bignum)) {}

bigint &bigint::operator=(const bigint &num)
{
    if (this != &num)
        this->bignum = num.bignum;
    return *this;
}

bigint &bigint::operator=(bigint &&num) noexcept
{
    if (this != &num)
        this->bignum = std::move(num.bignum);
    return *this;
}

bigint::~bigint() {}


/*Public helpers*/
u_int64_t bigint::num_digits() const
{
    return bignum.size();
}

std::ostream &operator<<(std::ostream &o, const bigint &num)
{
    if (num.neg)
        o << "-";

    auto it = num.bignum.rbegin();
    o << *it;
    for (it++; it != num.bignum.rend(); it++)
        o << std::setfill('0') << std::setw(9) << *it;
    return o;
}


/*Arithmetic Operations*/
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

/*
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
*/


/*Logical Operations and private helpers*/
bool bigint::operator==(const bigint &num) const
{
    if (num.num_digits() != this->num_digits() || num.neg != this->neg)
        return false;

    for (size_t l = 0; l < num.num_digits(); l++)
    {
        if (num.bignum[l] != this->bignum[l])
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
    if (this->neg && !num.neg)
        return false;

    if (!this->neg && num.neg)
        return true;

    if (this->neg && num.neg)
        return this->abs_lesser_than(num);

    return this->abs_greater_than(num);
}

bool bigint::operator<(const bigint &num) const
{
    if (this->neg && !num.neg)
        return true;

    if (!this->neg && num.neg)
        return false;

    if (this->neg && num.neg)
        return this->abs_greater_than(num);

    return this->abs_lesser_than(num);
}

bool bigint::abs_greater_than(const bigint &num) const
{
    if (num.num_digits() > this->num_digits())
        return false;

    if (num.num_digits() < this->num_digits())
        return true;

    for (int64_t l = num.num_digits() - 1; l >= 0; l--)
    {
        if (num.bignum[l] > this->bignum[l])
            return false;

        if (num.bignum[l] < this->bignum[l])
            return true;
    }

    return false;
}

bool bigint::abs_lesser_than(const bigint &num) const
{
    if (num.num_digits() < this->num_digits())
        return false;

    if (num.num_digits() > this->num_digits())
        return true;

    for (int64_t l = num.num_digits() - 1; l >= 0; l--)
    {
        if (num.bignum[l] < this->bignum[l])
            return false;

        if (num.bignum[l] > this->bignum[l])
            return true;
    }

    return false;
}


/*Arithmetic Operations updating self and private helpers*/
bigint &bigint::operator+=(const bigint &addend)
{
    if (!this->neg && !addend.neg)
        bigint::add_with_shift(*this, addend, 0);
    else if (this->neg && !addend.neg)
        bigint::sub(*this, addend);
    else if (!this->neg && addend.neg)
        bigint::sub(*this, addend);
    else
        bigint::add_with_shift(*this, addend, 0);

    return *this;
}

bigint &bigint::operator-=(const bigint &sub)
{
    if (!this->neg && !sub.neg)
        bigint::sub(*this, sub);
    else if (this->neg && !sub.neg)
        bigint::add_with_shift(*this, sub, 0);
    else if (!this->neg && sub.neg)
        bigint::add_with_shift(*this, sub, 0);
    else
        bigint::sub(*this, sub);

    return *this;
}

bigint &bigint::operator*=(const bigint &num)
{
    if (this->num_digits() == 1 && !this->bignum[0])
        return *this;

    if (num.num_digits() == 1 && !num.bignum[0])
    {
        this->bignum.clear();
        this->bignum.emplace_back(0);
        this->neg = false;
        return *this;
    }

    this->neg ^= num.neg; // Sign
    *this = std::move(bigint::multiply(*this, num, 0, this->num_digits(), 0, num.num_digits()));
    return *this;
}

/*
bigint& bigint::operator/=(const bigint &num)
{
    return *this;
}

bigint& bigint::operator%=(const bigint &num)
{
    return *this;
}
*/

void bigint::add_with_shift(bigint &a, const bigint &b, u_int64_t sb)
{
    // Ensure there are no leading zeros
    if (a.num_digits() > 1)
        assert(a.bignum[a.num_digits() - 1]);
    if (b.num_digits() > 1)
        assert(b.bignum[b.num_digits() - 1]);

    for (uint32_t i = a.num_digits(); i < sb; i++)
        a.bignum.emplace_back(0);

    uint32_t carry = 0, i;
    for (i = sb; i < b.num_digits() + sb; i++)
    {
        if (i == a.num_digits())
        {
            a.bignum.emplace_back(b.bignum[i - sb] + carry);
            carry = a.bignum[i] / bigint::BASE;
            a.bignum[i] %= bigint::BASE;
        }

        else
        {
            a.bignum[i] += b.bignum[i - sb] + carry;
            carry = a.bignum[i] / bigint::BASE;
            a.bignum[i] %= bigint::BASE;
        }
    }

    for (; i < a.num_digits() && carry; i++)
    {
        a.bignum[i] += carry;
        carry = a.bignum[i] / bigint::BASE;
        a.bignum[i] %= bigint::BASE;
    }

    if (carry)
        a.bignum.emplace_back(carry);
}

bigint bigint::_add_split(const bigint &a, u_int32_t st, u_int32_t end, u_int32_t split)
{
    bigint ret;
    ret.bignum.clear();
    uint64_t i, j, carry = 0;
    for (i = st, j = split; i < split && j < end; i++, j++)
    {
        ret.bignum.emplace_back((a.bignum[i] + a.bignum[j] + carry) % bigint::BASE);
        carry = (a.bignum[i] + a.bignum[j] + carry) / bigint::BASE;
    }

    for (; i < split; i++)
    {
        ret.bignum.emplace_back((a.bignum[i] + carry) % bigint::BASE);
        carry = (a.bignum[i] + carry) / bigint::BASE;
    }

    for (; j < end; j++)
    {
        ret.bignum.emplace_back((a.bignum[j] + carry) % bigint::BASE);
        carry = (a.bignum[j] + carry) / bigint::BASE;
    }

    if (carry)
        ret.bignum.emplace_back(carry);

    return ret;
}

void bigint::_sub(bigint &a, const bigint &b)
{
    // Ensure there are no leading zeros
    if (a.num_digits() > 1)
        assert(a.bignum[a.num_digits() - 1]);
    if (b.num_digits() > 1)
        assert(b.bignum[b.num_digits() - 1]);

    size_t l, borrow, nborrow;
    for (l = 0, borrow = 0, nborrow = 0; l < b.num_digits(); l++, borrow = nborrow)
    {
        if (a.bignum[l] < borrow + b.bignum[l])
        {
            a.bignum[l] += BASE;
            nborrow = 1;
        }
        else
            nborrow = 0;
        a.bignum[l] -= borrow + b.bignum[l];
    }

    for (; l < a.num_digits(); l++, borrow = nborrow)
    {
        if (a.bignum[l] < borrow)
        {
            a.bignum[l] += BASE;
            nborrow = 1;
        }
        else
            nborrow = 0;
        a.bignum[l] -= borrow;
    }

    a.pop_leading_zeros();

    if (a.num_digits() > 1)
        assert(a.bignum[a.num_digits() - 1]);
}

void bigint::sub(bigint &a, const bigint &b)
{
    bool orig = a.neg;
    a.neg = b.neg;
    if (a.operator==(b))
    {
        a.bignum.clear();
        a.bignum.emplace_back(0);
        return;
    }

    if (a.abs_greater_than(b))
    {
        a.neg = orig;
        bigint::_sub(a, b);
    }

    else
    {
        bigint a_cpy = std::move(a); // Move a into a copy
        a = b;                       // Copy b into a
        bigint::_sub(a, a_cpy);
    }
}

bigint bigint::multiply(const bigint &mul1, const bigint &mul2, u_int32_t m1_st, u_int32_t m1_end, u_int32_t m2_st, u_int32_t m2_end)
{
    if (m1_end - m1_st < 100 || m2_end - m2_st < 100 || m1_end - m1_st != m2_end - m2_st)
    {
        // Regular multiplication in the "base case"
        bigint m1(mul1, m1_st, m1_end);
        bigint m2(mul2, m2_st, m2_end);
        bigint::regular_multiplication(m1, m2);
        return m1;
    }

    // Karatsuba
    u_int32_t mid = m1_end - m1_st != m2_end - m2_st ? std::min(m1_end, m2_end) - 1 : (m1_st + m1_end) >> 1;
    // u_int32_t mid = (m1_st + m1_end) >> 1;
    bigint lmul(bigint::multiply(mul1, mul2, m1_st, mid, m2_st, mid));
    bigint rmul(bigint::multiply(mul1, mul2, mid, m1_end, mid, m2_end));

    bigint top_sum(bigint::_add_split(mul1, m1_st, m1_end, mid));
    bigint bottom_sum(bigint::_add_split(mul2, m2_st, m2_end, mid));
    bigint midmul(bigint::multiply(top_sum, bottom_sum, 0, top_sum.num_digits(), 0, bottom_sum.num_digits()));
    bigint::_sub(midmul, lmul);
    bigint::_sub(midmul, rmul);

    mid -= m1_st;
    bigint::add_with_shift(lmul, midmul, mid);
    bigint::add_with_shift(lmul, rmul, mid << 1);
    return lmul;
}

void bigint::regular_multiplication(bigint &mul, const bigint &num)
{
    int new_len = mul.num_digits() + num.num_digits() + 1;
    bigint orig(std::move(mul));
    mul.bignum.resize(new_len);

    for (u_int64_t i = 0; i < num.num_digits(); i++)
    {
        if (num.bignum[i])
        {
            u_int64_t carry = 0, j;
            for (j = 0; j < orig.num_digits(); j++)
            {
                u_int64_t prod = (u_int64_t) orig.bignum[j] * num.bignum[i], new_dig = prod + carry + mul.bignum[i + j];
                mul.bignum[i + j] = new_dig % bigint::BASE;
                carry = new_dig / bigint::BASE;
            }

            for (u_int64_t idx = i + j; carry; idx++)
            {
                mul.bignum[idx] += carry;
                carry = mul.bignum[idx] / bigint::BASE;
                mul.bignum[idx] %= bigint::BASE;
            }
        }
    }

    mul.pop_leading_zeros();
}

bigint &bigint::operator*=(u_int32_t num)
{
    u_int32_t carry = 0;

    for (u_int32_t &dig : this->bignum)
    {
        u_int64_t prod = dig;
        prod *= num;
        prod += carry;
        dig = prod % bigint::BASE;
        carry = prod / bigint::BASE;
    }

    if (carry)
        this->bignum.emplace_back(carry);

    return *this;
}

/*
bigint& bigint::operator/=(u_int64_t num)
{

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
*/


/*Private helpers*/
void bigint::pop_leading_zeros()
{
    for (u_int64_t nd = num_digits(); nd > 1 && bignum[nd - 1] == 0; nd--)
        bignum.pop_back();
}
