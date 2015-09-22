// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#ifndef _Valarray_h
#define _Valarray_h

#include <vector>
#include <complex>
#include <cmath>
#include <functional>
#include <type_traits>
#include "Vector.h"
//using std::vector; // during development and testing
using epl::vector; // after submission
using namespace std::rel_ops;
namespace epl{
    //forward declaration
    template <typename BASE>
    class vec_wrap;
    
    template <typename Left, typename Right, typename BinaryOperation>
    struct BinaryOperationProxy;
    
    template <typename T>
    struct scalar;

    //use code from Dr. Chase's example
    template <typename T> struct rank;
    
    template <typename T, typename Unused = decltype(rank<T>::value)>
    using valarray = vec_wrap<vector<T>>;
    
    template <> struct rank<int> { static constexpr int value = 1; };
    template <> struct rank<float> { static constexpr int value = 2; };
    template <> struct rank<double> { static constexpr int value = 3; };
    template <typename T> struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };
    template <typename BASE> struct rank<vec_wrap<BASE>> { static constexpr int value = rank<typename BASE::value_type>::value; };
    
    template <int R>
    struct stype;
    
    template <> struct stype<1> { using type = int; };
    template <> struct stype<2> { using type = float; };
    template <> struct stype<3> { using type = double; };
    
    template <typename T> struct is_complex { static constexpr bool value = false; };
    template <typename T> struct is_complex<std::complex<T>> { static constexpr bool value = true; };
    
    template <bool p, typename T> struct ctype;
    template <typename T> struct ctype<false, T> { using type = T; };
    template <typename T> struct ctype<true, T> { using type = std::complex<T>; };
    
    template <typename T1, typename T2>
    struct choose_type {
        static constexpr int t1_rank = rank<T1>::value;
        static constexpr int t2_rank = rank<T2>::value;
        static constexpr int max_rank = (t1_rank < t2_rank) ? t2_rank : t1_rank;
        
        using my_stype = typename stype<max_rank>::type;
        
        static constexpr bool t1_comp = is_complex<T1>::value;
        static constexpr bool t2_comp = is_complex<T2>::value;
        static constexpr bool my_comp = t1_comp || t2_comp;
        
        using type = typename ctype<my_comp, my_stype>::type;
    };
    //if the content in vec_wrap is a vector, then we store a reference, if a proxy, then we store a value
    template <typename T>
    struct to_ref { using type = T; };
    
    template <typename T>
    struct to_ref<vec_wrap<vector<T>>> {
        using type = const vec_wrap<vector<T>>&;
    };
    template <typename T>
    using Ref = typename to_ref<T>::type;
    
    template <typename T>
    struct wrap;
    
    template <> struct wrap<int> { using type = scalar<int>; };
    template <> struct wrap<float> { using type = scalar<float>; };
    template <> struct wrap<double> { using type = scalar<double>; };
    template <typename T> struct wrap<std::complex<T>> { using type = scalar<std::complex<T>>; };
    
    template <typename T>
    struct wrap<vec_wrap<T>> {
        using type = vec_wrap<T>;
    };
    
    template <typename T>
    using WRAP = typename wrap<T>::type;
    
    template <typename T1, typename T2>
    using choose = typename choose_type<typename WRAP<T1>::value_type, typename WRAP<T2>::value_type>::type;
    
    //operator +
    template <typename T1, typename T2>
    vec_wrap<BinaryOperationProxy<std::plus<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>
    operator+(const T1& x, const T2& y) {
        return vec_wrap<BinaryOperationProxy<std::plus<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>(std::plus<choose<T1, T2>>(), x, y);
    }
    //operator -
    template <typename T1, typename T2>
    vec_wrap<BinaryOperationProxy<std::minus<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>
    operator-(const T1& x, const T2& y) {
        return vec_wrap<BinaryOperationProxy<std::minus<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>(std::minus<choose<T1, T2>>(), x, y);
    }
    //operator *
    template <typename T1, typename T2>
    vec_wrap<BinaryOperationProxy<std::multiplies<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>
    operator*(const T1& x, const T2& y) {
        return vec_wrap<BinaryOperationProxy<std::multiplies<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>(std::multiplies<choose<T1, T2>>(), x, y);
    }
    //operator /
    template <typename T1, typename T2>
    vec_wrap<BinaryOperationProxy<std::divides<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>
    operator/(const T1& x, const T2& y) {
        return vec_wrap<BinaryOperationProxy<std::divides<choose<T1, T2>>, WRAP<T1>, WRAP<T2>>>(std::divides<choose<T1, T2>>(), x, y);
    }
    
    //iterator for proxies
    template <typename Proxy, typename T>
    class GeneralIterator {
    private:
        //class members
        Proxy p;
        uint64_t index;
    public:
        using iterator = GeneralIterator<Proxy, T>;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;
        
        GeneralIterator(Proxy _p, uint64_t _index) : p(_p), index(_index) {};
        //copy constructor
        GeneralIterator(const GeneralIterator& that) : p(that.p), index(that.index) {};
        //assigment operator
        iterator& operator=(const iterator& that) {
            if (this != &that) {
                copy_iterator(that);
            }
            return *this;
        }
        //dereference return type shouldn't be a reference, otherwise it will become a reference to a local variable
        T operator*() {
            return p.operator[](index);
        }
        //operator []
        T operator[](uint64_t n) {
            return p.operator[](index + n);
        }
        //operator ==
        bool operator==(const iterator& that) const {
            return this->index == that.index;
        }
        bool operator<(const iterator& that) const {
            return this->index < that.index;
        }
        //pre-increment operator
        iterator& operator++() {
            ++index;
            return *this;
        }
        //post_increment operator
        iterator operator++(int) {
            iterator temp{*this};
            ++index;
            return temp;
        }
        //operator iterator + int
        iterator operator+(int64_t n) {
            return iterator(this->p, index + n);
        }
        //operator int + iterator
        friend GeneralIterator operator+(int64_t n, iterator i) {
            return i + n;
        }
        //operator iterator - n
        iterator operator-(int64_t n) {
            return iterator(this->p, index - n);
        }
        //operator +=
        iterator& operator+=(int64_t n) {
            index += n;
            return *this;
        }
        //operator -=
        iterator& operator-=(int64_t n) {
            index -= n;
            return *this;
        }
    };
    
    
    template <typename Arg, typename Result>
    struct square_root : public std::unary_function<Arg, Result> {
        Result operator()(const Arg& x) const {//why If I don't add const then it can not compile.
            return (Result)std::sqrt(x);
        }
    };
    
    template <typename T>
    struct scalar {
        using value_type = T;
        T val;
        scalar(T _v) : val(_v) {};
        T& operator[](uint64_t index) {
            return val;
        }
        const T& operator[](uint64_t index) const {
            return val;
        }
        uint64_t size() const {
            return -1;
        }
    };
    
    //unary operation proxy
    template <typename T1, typename UnaryOperation>
    struct UnaryOperationProxy {
        using value_type = typename UnaryOperation::result_type;
        //members
        UnaryOperation op;
        Ref<vec_wrap<T1>> val;
        //constructor
        UnaryOperationProxy(UnaryOperation _op, const vec_wrap<T1>& _v) : op(_op), val(_v) {};
        
        //derefernce
        value_type operator[](uint64_t index) const {
            return (value_type)op(this->val[index]);
        }
        uint64_t size() const {
            return (uint64_t)val.size();//need cast?
        }
        
        using iterator = GeneralIterator<UnaryOperationProxy, value_type>;
        using const_iterator = GeneralIterator<UnaryOperationProxy, value_type>;
        
        iterator begin(void) {
            return iterator(*this, 0);
        }
        iterator end(void) {
            return iterator(*this, this->size());
        }
        const_iterator begin(void) const {
            return const_iterator(*this, 0);
        }
        const_iterator end(void) const {
            return const_iterator(*this, this->size());
        }
    };
    
    //binary operation proxy
    template <typename BinaryOpertaion, typename Left, typename Right>
    struct BinaryOperationProxy {
        using value_type = typename choose_type<typename Left::value_type, typename Right::value_type>::type;
        using iterator = GeneralIterator<BinaryOperationProxy, value_type>;
        using const_iterator = GeneralIterator<BinaryOperationProxy, value_type>;
        
        //data members
        BinaryOpertaion op;
        Ref<Left> left;
        Ref<Right> right;
        
        //constructor
        BinaryOperationProxy(BinaryOpertaion _op, const Left& _left, const Right& _right) : op(_op), left(_left), right(_right) {};
        value_type operator[](uint16_t index) const {
            auto leftvalue = (value_type)(this->left[index]);
            auto rightvalue = (value_type)(this->right[index]);
            
            return this->op(leftvalue, rightvalue);
        }
        //copy constructor
        BinaryOperationProxy(const BinaryOperationProxy& that) : op(that.op), left(that.left), right(that.right) {};
        uint64_t size() const {
            return std::min(static_cast<uint64_t>(left.size()), static_cast<uint64_t>(right.size()));
        }
        
        iterator begin() {
            return iterator(*this, 0);
        }
        iterator end() {
            return iterator(*this, this->size());
        }
        const_iterator begin() const {
            return const_iterator(*this, 0);
        }
        const_iterator end() const {
            return const_iterator(*this, this->size());
        }
        
    };
    
    template <typename BASE>
    class vec_wrap : public BASE {
    public:
        using BASE::BASE;
        using value_type = typename BASE::value_type; //so will the vector<T> has a value_type?
        
        template <typename BASE2>
        vec_wrap& operator=(const vec_wrap<BASE2>& that) {
            uint64_t size1 = this->size();
            uint64_t size2 = that.size();
            if (size1 > size2) {
                for (int i = 0; i < size1 - size2; ++i) {
                    this->pop_back();
                }
            }
       
            for (int i = 0; i < size1 && i < size2; ++i) {
                (*this)[i] = (value_type)that[i];
            }
            return *this;
        }
        
        //non-argument constructor for vec_wrap
        vec_wrap(void) : BASE(){};
        
        //copy cosntructor
        template <typename BASE2>
        vec_wrap(const vec_wrap<BASE2>& that) {
            auto b = that.begin();
            auto e = that.end();
            while (b != e) {
                this->push_back((value_type)*b);
                ++b;
            }
        }
        
        //assign a scalar to the valarray, maybe a problem
        vec_wrap& operator=(const value_type& val) {
            for (int i = 0; i < this->size(); ++i) {
                this->operator[](i) = val;
            }
            return *this;
        }
        
        //apply unary function to the element in valarray
        template <typename UnaryOperation>
        vec_wrap<UnaryOperationProxy<BASE, UnaryOperation>>
        apply(UnaryOperation op) const {
            return vec_wrap<UnaryOperationProxy<BASE, UnaryOperation>>(op, *this);
        }
        
        // sqrt fuction object
        template <typename T1 = typename choose_type<double, value_type>::type>
        vec_wrap<UnaryOperationProxy<BASE, square_root<value_type, T1>>>
        sqrt() {
            return apply(square_root<value_type, T1>());
        }
        
        //accumulate function
        template <typename Acc>
        typename Acc::result_type accumulate(Acc fun) {
            if (this->size() == 0) {
                typename Acc::result_type t{};
                return t;
            } else {
                typename Acc::result_type temp = (*this)[0];
                for (int i = 1; i < this->size(); ++i) {
                    temp = fun(temp, (*this)[i]);
                }
                return (typename Acc::result_type)temp;
            }
        }
        
        //sum function
        value_type sum() {
            return accumulate(std::plus<value_type>());
        }
    };
    
    template <typename BASE>
    std::ostream& operator<<(std::ostream& out, const vec_wrap<BASE>& that) {
        auto b = that.begin();
        auto e = that.end();
        while (b != e) {
            out << *b;
            out << std::endl;
            ++b;
        }
        return out;
    }
    
    // negation
    template<typename T>
    vec_wrap<UnaryOperationProxy<T, std::negate<typename T::value_type>>>
    operator-(const vec_wrap<T>& v) {
        return v.apply(std::negate<typename T::value_type>());
    }

}// namespace epl




#endif /* _Valarray_h */

