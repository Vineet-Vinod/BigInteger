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
    if ((!neg && i) || (neg && (i-1)))
        bignum[j] = atoi(num.substr(neg, neg ? i - 1 : i).c_str());
}

bigint::bigint(const bigint &num) : neg(num.neg), bignum(num.bignum) {}

bigint::bigint(const bigint &num, u_int32_t st, u_int32_t end) : neg(num.neg), bignum(num.bignum.begin() + st, num.bignum.begin() + end) {}

bigint::bigint(const std::vector<u_int32_t> bnum, bool sign)
{
    for (const auto &num: bignum)
        assert(num < bigint::BASE);

    this->neg = sign;
    this->bignum = bnum;
}

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

bigint& bigint::operator/=(const bigint &num)
{
    *this = std::move(div_mod(*this, num, true));
    return *this;
}

bigint& bigint::operator%=(const bigint &num)
{
    *this = std::move(div_mod(*this, num, false));
    return *this;
}


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

void bigint::_sub(std::vector<u_int32_t> &a, const std::vector<u_int32_t> &b)
{
    // Ensure there are no leading zeros
    if (a.size() > 1)
        assert(a[a.size() - 1]);
    if (b.size() > 1)
        assert(b[b.size() - 1]);

    size_t l, borrow, nborrow;
    for (l = 0, borrow = 0, nborrow = 0; l < b.size(); l++, borrow = nborrow)
    {
        if (a[l] < borrow + b[l])
        {
            a[l] += BASE;
            nborrow = 1;
        }
        else
            nborrow = 0;
        a[l] -= borrow + b[l];
    }

    for (; l < a.size(); l++, borrow = nborrow)
    {
        if (a[l] < borrow)
        {
            a[l] += BASE;
            nborrow = 1;
        }
        else
            nborrow = 0;
        a[l] -= borrow;
    }
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
        bigint::_sub(a.bignum, b.bignum);
        a.pop_leading_zeros();
    }

    else
    {
        bigint a_cpy = std::move(a); // Move a into a copy
        a = b;                       // Copy b into a
        bigint::_sub(a.bignum, a_cpy.bignum);
        a.pop_leading_zeros();
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
    bigint::_sub(midmul.bignum, lmul.bignum);
    midmul.pop_leading_zeros();
    bigint::_sub(midmul.bignum, rmul.bignum);
    midmul.pop_leading_zeros();

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

bigint bigint::div_mod(const bigint &dividend, const bigint &divisor, bool div)
{
    if (divisor.abs_greater_than(dividend))
        return div ? bigint(0) : dividend;

    if (!divisor.abs_lesser_than(dividend))
        return div ? bigint(dividend.neg ^ divisor.neg ? -1 : 1) : bigint(0);

    // Implement long division
    int32_t curr_idx = dividend.num_digits() - divisor.num_digits(), div_digits = curr_idx + 1;

    bigint remainder(dividend, static_cast<u_int32_t>(curr_idx), dividend.num_digits());
    remainder.bignum.reserve(divisor.num_digits() + 2);
    remainder.neg = false;
    
    bigint div_cpy(divisor);
    div_cpy.neg = false;
    
    bigint quotient;
    quotient.bignum.reserve(div_digits+1);

    for (--curr_idx; curr_idx >= 0; curr_idx--)
    {
        if (remainder.abs_lesser_than(divisor))
        {
            quotient.bignum.emplace_back(0);
            auto& rem = remainder.bignum;
            rem.emplace_back(0);
            for (u_int32_t idx = rem.size() - 1; idx > 0; idx--)
                rem[idx] = rem[idx - 1];
        }

        else
            quotient.bignum.emplace_back(bigint::div(remainder, div_cpy, divisor, true));

        remainder.bignum[0] = dividend.bignum[curr_idx];
    }

    if (!remainder.abs_lesser_than(divisor))
        quotient.bignum.emplace_back(bigint::div(remainder, div_cpy, divisor, false));

    quotient.reverse_num();
    quotient.pop_leading_zeros();
    quotient.neg = divisor.neg ^ dividend.neg;
    remainder.neg = dividend.neg;
    return div ? quotient : remainder;
}

u_int32_t bigint::div(bigint &dividend, bigint &div_cpy, const bigint &divisor, bool shift)
{
    u_int32_t l = 1, r = bigint::BASE - 1;

    while (l <= r)
    {
        u_int32_t m = (l + r) >> 1;
        bigint::mul_dig_in_place(div_cpy, divisor, m);
        if (div_cpy.abs_greater_than(dividend))
            r = m - 1;
        else
            l = m + 1;
    }

    bigint::mul_dig_in_place(div_cpy, divisor, r);
    if (!shift)
        dividend -= div_cpy;
    
    else
    {
        dividend.bignum.emplace_back(0); // No reallocation needed because sufficient space was reserved
        auto &divi = dividend.bignum;
        auto &div = div_cpy.bignum;

        for (u_int32_t idx = dividend.num_digits() - 1; idx > 0; idx--)
        {
            divi[idx] = divi[idx - 1];

            if (idx <= div_cpy.num_digits())
            {
                if (divi[idx] < div[idx - 1]) // Cannot happen at the Most Significant Digit coz dividend >= div_cpy (by bin search)
                {
                    divi[idx + 1]--;
                    divi[idx] += bigint::BASE;
                }
                divi[idx] -= div[idx - 1];
            }
        }

        divi[0] = 0;
        dividend.pop_leading_zeros();
    }

    return r;
}

void bigint::mul_dig_in_place(bigint &prod, const bigint &orig, u_int32_t digit)
{
    for (u_int32_t idx = 0; idx < orig.num_digits(); idx++)
        prod.bignum[idx] = orig.bignum[idx];

    for (u_int32_t idx = orig.num_digits(); idx < prod.num_digits(); idx++)
        prod.bignum[idx] = 0;
    
    prod *= digit;
    prod.pop_leading_zeros();
}


/*Private helpers*/
void bigint::pop_leading_zeros()
{
    for (u_int64_t nd = num_digits(); nd > 1 && bignum[nd - 1] == 0; nd--)
        bignum.pop_back();
}

void bigint::reverse_num()
{
    std::reverse(all(bignum));
}
