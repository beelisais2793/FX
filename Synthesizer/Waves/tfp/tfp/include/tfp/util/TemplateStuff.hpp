#include <string>

namespace tfp {

template <bool B, class T=void>  struct enable_if                   {};
template <class T>               struct enable_if<true, T>          { typedef T type; };

template <class T>               struct remove_const                { typedef T type; };
template <class T>               struct remove_const<const T>       { typedef T type; };

template <class T>               struct remove_volatile             { typedef T type; };
template <class T>               struct remove_volatile<volatile T> { typedef T type; };
template <class T>               struct remove_cv                   { typedef typename remove_volatile<typename remove_const<T>::type>::type type; };

template <class T, class U>      struct is_same                     { enum { value = false }; };
template <class T>               struct is_same<T, T>               { enum { value = true }; };
template <class T, class U>      struct same_type                   { typedef typename enable_if<is_same<T, U>::value, T>::type type; };

template <class T>               struct is_float                    { enum { value = is_same<float, typename remove_cv<T>::type>::value }; };
template <class T>               struct is_double                   { enum { value = is_same<double, typename remove_cv<T>::type>::value }; };
template <class T>               struct is_long_double              { enum { value = is_same<long double, typename remove_cv<T>::type>::value }; };
template <class T>               struct is_floating_point           { enum { value = is_float<T>::value || is_double<T>::value || is_long_double<T>::value }; };
template <class T>               struct is_std_string               { enum { value = is_same<std::string, typename remove_cv<T>::type>::value }; };
template <class T>               struct is_c_string                 { enum { value = is_same<const char*, T>::value }; };
template <class T>               struct is_string                   { enum { value = is_std_string<T>::value || is_c_string<T>::value }; };

template <bool, class T, class U> struct conditional                 { typedef T type; };
template <class T, class U>       struct conditional<false, T, U>    { typedef U type; };

} // namespace tfp
