#include <iostream>
#include <utility>
#include <vector>
#include <cmath>
#include <iomanip>
#include "complex"
#include <vector>
#include <utility>
#include <cstdint>

class BigInt {
private:
    using ftype = std::complex<double>;
    const int _size_base = 5;
    unsigned long long _base = 100000;
    std::vector<unsigned long long> data;
    bool negative = false;

    static bool is_correct_string(const std::string& string) noexcept{
        int i = 0;
        if (string[0] == '-'){
            i++;
        }
        for (;i < string.size(); i++){
            if ('0' > string[i] || string[i]> '9'){
                return false;
            }
        }
        return true;
    }

    static std::pair<std::string, std::string> splitArray(const std::vector<unsigned long long int>& arr) {
        size_t mid = (arr.size() + 1) / 2;
        auto convert = [](const std::vector<unsigned long long int> &vec) {
            if (vec.empty()) {
                return std::string("0");
            }
            std::ostringstream oss;
            for (size_t i = 0; i < vec.size(); ++i) {
                oss << vec[i];
            }
            return oss.str();
        };

        std::vector<unsigned long long int> first_part(arr.begin(), arr.begin() + mid);
        std::vector<unsigned long long int> second_part(arr.begin() + mid, arr.end());

        return {convert(first_part), convert(second_part)};
    }

    void delete_zeros(){
        for (int i = size() - 1; i > 0; --i) {
            if (data[i] == 0) {
                data.pop_back();
            } else {
                return;
            }
        }
    }

    void shift_left(size_t n_blocks) {
        if (n_blocks == 0) return;
        data.insert(data.begin(), n_blocks, 0);
//        for (;n_blocks > 0; n_blocks--){
//            *this *= BigInt(_base);
//        }
    }

public:
    unsigned long long operator[](size_t index) const noexcept{
        return data[index];
    }

    BigInt() {
        data.push_back(0);
    }

    explicit BigInt(const std::string& input) {
        if (input.empty()) {
            data = {0};
            return;
        }
        if (!is_correct_string(input)){
            throw std::invalid_argument("Invalid BigInt string: " + input);
        }

        size_t start = 0;
        if (!input.empty() && (input[0] == '-' || input[0] == '+')) {
            negative = (input[0] == '-');
            start = 1;
        }

        std::string num_str = input.substr(start);

        size_t first_non_zero = num_str.find_first_not_of('0');
        if (first_non_zero == std::string::npos) {
            data = {0};
            negative = false;
            return;
        }
        num_str = num_str.substr(first_non_zero);

        int length = num_str.length();

        for (int i = length; i > 0; i -= _size_base) {
            int start_pos = std::max(i - _size_base, 0);
            std::string block = num_str.substr(start_pos, i - start_pos);
            data.push_back(std::stoull(block));
        }
    }

    explicit BigInt(long long number){
        if (number == 0){
            data.push_back(0);
            return;
        }
        if (number < 0) {
            negative = true;
            number = -number;
        }
        unsigned long long carry = number % _base;
        while (number > 0){
            data.push_back(carry);
            number /= _base;
            carry = number % _base;
        }
    }

    BigInt(const BigInt& other) noexcept{
        negative = other.negative;
        data = other.data;
    }

    BigInt(BigInt&& other) noexcept : BigInt(){
        if (this != &other) {
            std::swap(this->data, other.data);
            std::swap(this->negative, other.negative);
        }
    }

    [[nodiscard]] BigInt abs() const{
        BigInt temp(*this);
        temp.negative = false;
        return temp;
    }

    BigInt& operator=(const BigInt& other) noexcept{
        negative = other.negative;
        data.resize(other.data.size());
        std::copy(other.data.begin(), other.data.end(), data.begin());
        return *this;
    }

    BigInt& operator=(BigInt&& other) noexcept {
        if (this != &other) {
            *this = other;
            other.data.clear();
            other.negative = false;
        }
        return *this;
    }

    BigInt& operator+=(const BigInt& other){
        auto length = std::max(size(), other.size());
        bool flag = false;
        if (negative){
            *this = -*this;
            *this += -other;
            *this = -*this;
            return *this;
        }
        if (other.negative) {
            return *this -= -other;
        }
        BigInt result{"0"};
        result.data.pop_back();
        size_t carry = 0;
        size_t i = 0;
        for (; i < length; ++i) {
            if (i < other.size() && i < data.size()) {
                result.data.push_back((other.data[i] + data[i] + carry) % _base);
                carry = (other.data[i] + data[i] + carry) / _base;
            } else if (i < data.size()) {
                result.data.push_back((data[i] + carry) % _base);
                carry = (data[i] + carry) / _base;
            } else {
                result.data.push_back((other.data[i] + carry) % _base);
                carry = (other.data[i] + carry) / _base;
            }
        }

        while (carry > 0){
            result.data.push_back((carry) % _base);
            carry = (carry) / _base;
        }
        *this = result;
        return *this;
    }

    BigInt operator+(const BigInt& other) const{
        BigInt temp{*this};
        temp += other;
        return temp;
    }

    BigInt& operator-=(const BigInt& other){
        if (*this == other) {
            *this = BigInt("0");
            return *this;
        }
        if (negative) {
            *this = -((-*this) + other);
            return *this;
        } else if (other.negative) {
            return *this += (-other);
        }
//        if (other > *this) {
//            BigInt result = other - *this;
//            result.negative = true;
//            *this = result;
//            return *this;
//        }
        long long carry = 0;
        for (size_t i = 0; i < other.data.size() || carry; ++i) {
            long long temp = 0;
            if (i < other.size()){
                temp = other[i];
            }
            long long diff = data[i] - temp - carry;
            carry = 0;
            if (diff < 0) {
                diff += _base;
                carry = 1;
            }
            data[i] = diff;
        }
        return *this;
    }

    BigInt operator-() const{
        BigInt temp{*this};
        temp.negative = !temp.negative;
        return temp;
    }

    BigInt operator-(const BigInt& other) const{
        BigInt temp{*this};
        temp -= other;
        return temp;
    }

    BigInt& operator*=(const BigInt& other){
        BigInt result(0);
        result.data.resize(size() + other.size());
        long long carry = 0;
        for (size_t i = 0; i < size(); ++i) {
            for (size_t j = 0; j < other.size() or carry; ++j) {
                long long current = 0;
                if (j < other.size()) {
                    current = result.data[i + j] + data[i] * other[j] + carry;
                } else {
                    current = result.data[i + j] + carry;
                }
                result.data[i + j] = current % _base;
                carry = current / _base;
            }
        }
        if (negative == other.negative){
            result.negative = false;
        } else {
            result.negative = true;
        }
        result.delete_zeros();
        if (result.abs() == BigInt(0) && result.is_negative()){
            result.negative = false;
        }
        *this = result;
        return *this;
    }

    BigInt operator*(const BigInt& other) const{
        BigInt temp{*this};
        temp *= other;
        return temp;
    }

    BigInt& operator/=(const BigInt& other) {
        if (other.size() == 1 && other[0] == 0) {
            throw std::invalid_argument("Division by zero");
        }
        bool result_negative = negative ^ other.negative;
        BigInt dividend = this->abs();
        BigInt divisor = other.abs();
        if (dividend < divisor) {
            *this = BigInt{0};
            negative = result_negative;
            return *this;
        }
        BigInt quotient;
        quotient.data.clear();
        BigInt current;
        current.data.pop_back();

        for (int i = dividend.size() - 1; i >= 0; --i) {
            current.data.insert(current.data.begin(), dividend.data[i]);
            current.delete_zeros();

            unsigned long long q = 0;
            unsigned long long left = 0, right = _base - 1;
            while (left <= right) {
                unsigned long long mid = (left + right) / 2;
                BigInt temp = divisor * BigInt(mid);
                if (temp <= current) {
                    q = mid;
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            quotient.data.insert(quotient.data.begin(), q);
            current -= divisor * BigInt(q);
        }

        quotient.delete_zeros();
        quotient.negative = result_negative;
        *this = quotient;

        return *this;
    }

    BigInt operator/(const BigInt& other) const{
        BigInt temp{*this};
        temp /= other;
        return temp;
    }

    BigInt& operator%=(const BigInt &num) {
        if (num == BigInt(0)){
            return *this;
        }
        BigInt tmp = *this / num;
        *this = *this - tmp * num;
        if(negative and num.negative) {
            negative = false;
        }
        return *this;
    }

    BigInt operator%(const BigInt &num) const {
        BigInt temp{*this};
        temp %= num;
        return temp;
    }

    bool operator>(const BigInt& other) const{
        if (negative && !other.negative){
            return false;
        }
        if (!negative && other.negative){
            return true;
        }
        if (size() > other.size()){
            return !negative;
        }
        if (size() < other.size()){
            return negative;
        }
        for (int i = 0; i < size(); i++){
            if (data[i] > other[i]){
                return !negative;
            }
            if (data[i] < other[i]){
                return negative;
            }
        }
        return negative;
    }

    bool operator==(const BigInt& other) const{
        if (size() != other.size() || negative != other.negative){
            return false;
        }
        for (int i = 0; i < size(); i++){
            if (data[i] != other[i]){
                return false;
            }
        }
        return true;
    }

    bool operator>=(const BigInt& other) const{
        return (*this > other || *this == other);
    }

    bool operator<(const BigInt& other) const{
        return !(*this >= other);
    }

    bool operator<=(const BigInt& other) const{
        return !(*this > other);
    }

    bool operator!=(const BigInt& other) const{
        return !(*this == other);
    }

    [[nodiscard]] size_t size() const noexcept{
        return data.size();
    }

    [[nodiscard]] unsigned long long base() const noexcept{
        return _base;
    }

    [[nodiscard]] bool empty() const noexcept{
        return data.empty();
    }

    [[nodiscard]] bool is_negative() const noexcept{
        return negative;
    }

    [[nodiscard]] bool is_even() const noexcept {
        return (*this % BigInt(2)) == BigInt(0);
    }

    [[nodiscard]] std::vector<unsigned long long> array() const{
        return data;
    }

    static void fft(std::vector<ftype>& a, bool invert) {
        int n = a.size();
        for(int i = 1, j = 0; i < n; i++) {
            int bit = n >> 1;
            for(; j >= bit; bit >>= 1)
                j -= bit;
            j += bit;
            if(i < j)
                std::swap(a[i], a[j]);
        }

        for(int len = 2; len <= n; len <<= 1) {
            double ang = 2 * acos(-1) / len * (invert ? -1 : 1);
            ftype wlen(cos(ang), sin(ang));
            for(int i = 0; i < n; i += len) {
                ftype w(1);
                for(int j = 0; j < len/2; j++) {
                    ftype u = a[i+j], v = a[i+j+len/2] * w;
                    a[i+j] = u + v;
                    a[i+j+len/2] = u - v;
                    w *= wlen;
                }
            }
        }

        if(invert)
            for(int i = 0; i < n; i++) {
                a[i] /= n;
            }
    }

    static BigInt multiply_fft(const BigInt& a, const BigInt& b) {
        std::vector<ftype> fa(a.data.begin(), a.data.end());
        std::vector<ftype> fb(b.data.begin(), b.data.end());

        size_t n = 1;
        while (n < fa.size() + fb.size())
            n <<= 1;
        fa.resize(n);
        fb.resize(n);

        fft(fa, false);
        fft(fb, false);

        for (size_t i = 0; i < n; i++)
            fa[i] *= fb[i];

        fft(fa, true);
        BigInt result;
        result.data.clear();
        unsigned long long carry = 0;
        const unsigned long long base = a.base();
        unsigned long long current;
        for (size_t i = 0; i < n; i++) {
            current = static_cast<unsigned long long>(round(fa[i].real())) + carry;
            carry = current / base;
            result.data.push_back(current % base);
        }

        while (carry) {
            result.array().push_back(carry % base);
            carry /= base;
        }

        result.delete_zeros();
        result.negative = a.is_negative() != b.is_negative();
        return result;
    }

    static BigInt karatsuba_multiply(const BigInt &a, const BigInt& b) {
        if(a == BigInt(0) or b == BigInt(0)) {
            return BigInt{0};
        }
        BigInt left{b};
        BigInt right{a};
        left.negative = false;
        right.negative = false;
        BigInt result = karatsuba(left, right);
        result.negative = a.is_negative() != b.is_negative();
        return result;
    }

    static BigInt karatsuba(BigInt a, BigInt b) {
        size_t n = 1;
        size_t max_size = std::max(a.data.size(), b.data.size());
        while (n < max_size) n <<= 1;
        a.data.resize(n, 0);
        b.data.resize(n, 0);

        if (n == 1) {
            return a * b;
        }

        BigInt A1, A2, B1, B2;
        A1.data.erase(A1.data.begin());
        A2.data.erase(A2.data.begin());
        B1.data.erase(B1.data.begin());
        B2.data.erase(B2.data.begin());
        for (size_t i = 0; i < n / 2; ++i) {
            A1.data.push_back(a.data[i]);
            A2.data.push_back(a.data[i + n / 2]);
            B1.data.push_back(b.data[i]);
            B2.data.push_back(b.data[i + n / 2]);
        }
        BigInt X = karatsuba(A1, B1);
        BigInt Y = karatsuba(A2, B2);
        BigInt Z = karatsuba(A1 + A2, B1 + B2) - X - Y;
        Z.shift_left((int) n / 2);
        Y.shift_left((int) n);
        BigInt result = X + Y + Z;
        result.delete_zeros();
        return result;
    }
};

std::ostream& operator<< (std::ostream& output, const BigInt& element){
    if (element.empty()){
        return output;
    }
    if (element.is_negative()){
        output << "-";
    }
    auto len_base = (int) log10l(element.base());
    for (long long int i = element.size() - 1; i >= 0; --i) {
        if ((size_t) i != element.size() - 1) {
            output << std::setw(len_base) << std::setfill('0') << element[i];
        } else {
            output << element[i];
        }
    }
    return output;
}


std::istream& operator>>(std::istream& is, BigInt& bigInt) {
    std::string input;
    char c;
    is >> std::ws;
    c = is.peek();
    if (c == '-' || c == '+') {
        input += c;
        is.get();
    }
    while (is.peek() != EOF && std::isdigit(is.peek())) {
        input += is.get();
    }
    if (input.empty() ||
        (input.size() == 1 && (input[0] == '+' || input[0] == '-'))) {
        is.setstate(std::ios::failbit);
        return is;
    }
    bigInt = BigInt(input);
    return is;
}