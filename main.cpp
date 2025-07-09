#include "bigint.h"
#include "random"
#include "gmpxx.h"
#include "chrono"


// Macros for benching performance against the GMP Library
#define TIME(loc, src) { auto start = std::chrono::high_resolution_clock::now(); loc; auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration = end - start; std::cout << src << " elapsed time: " << duration.count() << " seconds\n"; }

#define RACE(loc1, loc2, var1, var2) { auto start = std::chrono::high_resolution_clock::now(); loc1; auto end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration1 = end - start; start = std::chrono::high_resolution_clock::now(); loc2; end = std::chrono::high_resolution_clock::now(); std::chrono::duration<double> duration2 = end - start; if (duration1 < duration2) var1++; else var2++; }


static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> distr(0,9);
static std::uniform_int_distribution<> distr1(1,9);


std::string generate(int num_digs)
{
    std::string ret;
    
    if (distr(gen) & 1)
    {
        ret.reserve(num_digs+1);
        ret.push_back('-');
    }
    else
        ret.reserve(num_digs);

    ret.push_back(distr1(gen)+'0');
    for (int i = 1; i < num_digs; i++)
    {
        int c = distr(gen);
        ret.push_back(c+'0');
    }
    return ret;
}


int main(int argc, char const *argv[])
{
    assert(argc == 5);
    int l1 = atoi(argv[1]);
    int l2 = atoi(argv[2]);
    int iter = atoi(argv[4]);

    int dark = 0, gmp = 0;
    for (int i = 0; i < iter; i++)
    {
        std::string n1 = generate(l1), n2 = generate(l2);

        bigint num1(n1);
        bigint num2(n2);
        bigint my_res, convres;

        mpz_class mpz1(n1);
        mpz_class mpz2(n2);
        mpz_class res;

        switch (argv[3][0])
        {
        case '+':
            #ifdef TIMER
            TIME(my_res = num1 + num2, "Dark")
            TIME(res = mpz1 + mpz2, "GMP")
            #endif
            #ifndef TIMER
            RACE(my_res = num1 + num2, res = mpz1 + mpz2, dark, gmp)
            #endif
            convres = res.get_str();
            #ifdef PRINT
            std::cout << convres << "\n" << my_res << "\n" << std::endl;
            #endif
            assert(my_res == convres);
            break;
        
        case '-':
            #ifdef TIMER
            TIME(my_res = num1 - num2, "Dark")
            TIME(res = mpz1 - mpz2, "GMP")
            #endif
            #ifndef TIMER
            RACE(my_res = num1 - num2, res = mpz1 - mpz2, dark, gmp)
            #endif
            convres = res.get_str();
            #ifdef PRINT
            std::cout << convres << "\n" << my_res << "\n" << std::endl;
            #endif
            assert(my_res == convres);
            break;

        case 'x':
            #ifdef TIMER
            TIME(my_res = num1 * num2, "Dark")
            TIME(res = mpz1 * mpz2, "GMP")
            #endif
            #ifndef TIMER
            RACE(my_res = num1 * num2, res = mpz1 * mpz2, dark, gmp)
            #endif
            convres = res.get_str();
            #ifdef PRINT
            std::cout << convres << "\n" << my_res << "\n" << std::endl;
            #endif
            assert(my_res == convres);
            break;
        
        case '/':
            #ifdef TIMER
            TIME(my_res = num1 / num2, "Dark")
            TIME(res = mpz1 / mpz2, "GMP")
            #endif
            #ifndef TIMER
            RACE(my_res = num1 / num2, res = mpz1 / mpz2, dark, gmp)
            #endif
            convres = res.get_str();
            #ifdef PRINT
            std::cout << convres << "\n" << my_res << "\n" << std::endl;
            #endif
            assert(my_res == convres);
            break;

        case '%':
            #ifdef TIMER
            TIME(my_res = num1 % num2, "Dark")
            TIME(res = mpz1 % mpz2, "GMP")
            #endif
            #ifndef TIMER
            RACE(my_res = num1 % num2, res = mpz1 % mpz2, dark, gmp)
            #endif
            convres = res.get_str();
            #ifdef PRINT
            std::cout << convres << "\n" << my_res << "\n" << std::endl;
            #endif
            assert(my_res == convres);
            break;
        
        default:
            std::cout << "NOT IMPLEMENTED!" << std::endl;
        }
    }

    std::cout << "Final Score\nDark: " << dark << "\nGMP: " << gmp << std::endl;
    return 0;
}
