//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/uri
//

#ifndef BOOST_URI_DETAIL_CONFIG_HPP
#define BOOST_URI_DETAIL_CONFIG_HPP

#include <boost/config.hpp>
#ifndef BOOST_URI_STANDALONE
# include <boost/assert.hpp>
# include <boost/system/error_code.hpp>
# include <boost/system/system_error.hpp>
# include <boost/utility/string_view.hpp>
#else
# include <cassert>
# include <string_view>
# include <system_error>
#endif
#include <stdint.h>

// detect 32/64 bit
#if UINTPTR_MAX == UINT64_MAX
# define BOOST_URI_ARCH 64
#elif UINTPTR_MAX == UINT32_MAX
# define BOOST_URI_ARCH 32
#else
# error Unknown or unsupported architecture, please open an issue
#endif

// VFALCO Copied from <boost/config.hpp>
//        This is a derivative work.
#ifdef __has_cpp_attribute
// clang-6 accepts [[nodiscard]] with -std=c++14, but warns about it -pedantic
# if __has_cpp_attribute(nodiscard) && !(defined(__clang__) && (__cplusplus < 201703L))
#  define BOOST_URI_NODISCARD [[nodiscard]]
# else
#  define BOOST_URI_NODISCARD
# endif
#else
# define BOOST_URI_NODISCARD
#endif

#ifndef BOOST_URI_FORCEINLINE
# ifdef _MSC_VER
#  define BOOST_URI_FORCEINLINE __forceinline
# else
#  define BOOST_URI_FORCEINLINE inline
# endif
#endif

#ifndef BOOST_NO_EXCEPTIONS
# define BOOST_URI_THROW(x) throw(x)
#else
# define BOOST_URI_THROW(x) do{}while(0)
#endif

#ifndef BOOST_URI_STANDALONE
# define BOOST_URI_ASSERT BOOST_ASSERT
#else
# define BOOST_URI_ASSERT assert
#endif

#define BOOST_URI_STATIC_ASSERT( ... ) static_assert(__VA_ARGS__, #__VA_ARGS__)

#ifndef BOOST_URI_NO_SSE2
# if (defined(_M_IX86) && _M_IX86_FP == 2) || \
      defined(_M_X64) || defined(__SSE2__)
#  define BOOST_URI_USE_SSE2
# endif
#endif

#ifndef BOOST_URI_STANDALONE
# if defined(GENERATING_DOCUMENTATION)
#  define BOOST_URI_DECL
# elif defined(BOOST_URI_HEADER_ONLY)
#  define BOOST_URI_DECL  inline
# else
#  if (defined(BOOST_URI_DYN_LINK) || defined(BOOST_ALL_DYN_LINK)) && !defined(BOOST_URI_STATIC_LINK)
#   if defined(BOOST_URI_SOURCE)
#    define BOOST_URI_DECL  BOOST_SYMBOL_EXPORT
#    define BOOST_URI_BUILD_DLL
#   else
#    define BOOST_URI_DECL  BOOST_SYMBOL_IMPORT
#   endif
#  endif // shared lib
#  ifndef  BOOST_URI_DECL
#   define BOOST_URI_DECL
#  endif
#  if !defined(BOOST_URI_SOURCE) && !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_URI_NO_LIB)
#   define BOOST_LIB_NAME boost_uri
#   if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_URI_DYN_LINK)
#    define BOOST_DYN_LINK
#   endif
//#   include <boost/config/auto_link.hpp>
#  endif  // auto-linking disabled
# endif
#else
# ifdef BOOST_URI_HEADER_ONLY
#  define BOOST_URI_DECL  inline
# else
#  define BOOST_URI_DECL
# endif
#endif

// These macros are private, for tests, do not change
// them or else previously built libraries won't match.
#ifndef BOOST_URI_MAX_OBJECT_SIZE
# define BOOST_URI_NO_MAX_OBJECT_SIZE
# define BOOST_URI_MAX_OBJECT_SIZE  0x7ffffffe
#endif
#ifndef BOOST_URI_MAX_ARRAY_SIZE
# define BOOST_URI_NO_MAX_ARRAY_SIZE
# define BOOST_URI_MAX_ARRAY_SIZE   0x7ffffffe
#endif
#ifndef BOOST_URI_MAX_STRING_SIZE
# define BOOST_URI_NO_MAX_STRING_SIZE
# define BOOST_URI_MAX_STRING_SIZE  0x7ffffffe
#endif
#ifndef BOOST_URI_MAX_STACK_SIZE
# define BOOST_URI_NO_MAX_STACK_SIZE
# define BOOST_URI_MAX_STACK_SIZE  ((::size_t)(-1))
#endif
#ifndef BOOST_URI_PARSER_BUFFER_SIZE
# define BOOST_URI_NO_PARSER_BUFFER_SIZE
# define BOOST_URI_PARSER_BUFFER_SIZE 2048
#endif

namespace boost {
namespace uri {
namespace detail {

template<class...>
struct make_void
{
    using type =void;
};

template<class... Ts>
using void_t = typename
    make_void<Ts...>::type;

template<class T>
struct remove_const
{
    using type = T;
};

template<class T>
struct remove_const<T const>
{
    using type = T;
};

template<class T>
struct remove_reference
{
    using type = T;
};

template<class T>
struct remove_reference<T&>
{
    using type = T;
};

template<class T>
constexpr
typename remove_reference<T>::type&&
move(T&& t) noexcept
{
    return static_cast<typename
        remove_reference<T>::type&&>(t);
}

template<class T, class U>
inline
T
exchange(T& t, U u) noexcept
{
    T v = move(t);
    t = move(u);
    return v;
}

template<class T>
using is_string_viewish = typename std::enable_if<
    std::is_convertible<
        T const&, string_view>::value &&
    ! std::is_convertible<
        T const&, char const*>::value
            >::type;

/*  This is a derivative work, original copyright:

    Copyright Eric Niebler 2013-present

    Use, modification and distribution is subject to the
    Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)

    Project home: https://github.com/ericniebler/range-v3
*/
template<typename T>
struct static_const
{
    static constexpr T value {};
};
template<typename T>
constexpr T static_const<T>::value;

#define BOOST_URI_INLINE_VARIABLE(name, type) \
    namespace \
    { \
        constexpr auto& name = \
            ::boost::uri::detail::static_const<type>::value; \
    }

struct primary_template
{
};

template<class T>
using is_specialized =
    std::integral_constant<bool,
        ! std::is_base_of<primary_template, T>::value>;

template<class T>
using remove_cr =
    typename remove_const<
    typename remove_reference<T>::type>::type;

} // detail
} // uri
} // boost

#endif
