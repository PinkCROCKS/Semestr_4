#include <climits>
#include "../include/bigint.hpp"

BigInt mod_exp(const BigInt& base, const BigInt& exp, const BigInt& mod){
    if (exp < BigInt(0) or mod< BigInt(0)){
        throw std::invalid_argument("Division by zero");
    }
    if (exp == BigInt(0)) {
        return BigInt{1};
    }
    BigInt a = mod_exp(base % mod, exp / BigInt(2), mod) % mod;
    if (exp.is_even()){
        return (a * a) % mod;
    } else {
        return ((base % mod) * ((a * a) % mod)) % mod;
    }
}

uint16_t multHigherHalf(uint16_t a, uint16_t b)
{
    uint32_t res = a * b;
    res >>= 16;
    return res;
}


uint16_t divide(uint16_t u, uint16_t v)
{
    uint16_t reciprocals_8[] = { 0xFF00,  0xE300,  0xCC00,  0xBA00,  0xAA00,  0x9D00,  0x9200,  0x8800, };
    int shift_to_left =  __builtin_clz(v) - 16;
    v <<= shift_to_left;

    uint16_t x = reciprocals_8[(v >> (8 + 3 + 1)) - 8];

    x = multHigherHalf(static_cast<uint16_t>((-(v * x)) >> 16), x) << 1;
    x = multHigherHalf(static_cast<uint16_t>((-(v * x)) >> 16), x) << 1;

    uint16_t q = multHigherHalf(x, u);
    q >>= 16 - shift_to_left - 1;
    v >>= shift_to_left;

    uint32_t reminder = u - q * v;
    if (reminder >= v)
    {
        reminder -= v;
        ++q;

        if (reminder >= v)
        {
            ++q;

        }
    }

    return q;
}
//
//int main(){
//    std::cout << BigInt::Newton_divide(BigInt(12), BigInt(3));
//}