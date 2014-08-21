//
// Copyright (c) 2014 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_DI_POLICIES_PERMISSIONS_HPP
#define BOOST_DI_POLICIES_PERMISSIONS_HPP

#include "boost/di/aux_/mpl.hpp"
#include "boost/di/type_traits/remove_accessors.hpp"
#include "boost/di/scopes/deduce.hpp"

namespace boost {
namespace di {
namespace policies {

BOOST_DI_HAS_MEMBER_TYPE(element_type);
BOOST_DI_HAS_MEMBER_TYPE(value_type);

struct allow_smart_ptrs {
    template<typename T>
    using allow = has_element_type<typename type_traits::remove_accessors<T>::type>;
};

struct allow_refs {
    template<typename T>
    using allow = bool_<
        std::is_reference<T>::value &&
        !std::is_const<typename std::remove_reference<T>::type>::value &&
        !has_element_type<typename type_traits::remove_accessors<T>::type>::value
    >;
};

struct allow_const_refs {
    template<typename T>
    using allow = bool_<
        std::is_const<typename std::remove_reference<T>::type>::value &&
        std::is_reference<T>::value &&
        !has_element_type<typename type_traits::remove_accessors<T>::type>::value
    >;
};

struct allow_rvalue_refs {
    template<typename T>
    using allow = std::is_rvalue_reference<T>;
};

struct allow_ptrs {
    template<typename T>
    using allow = std::is_pointer<T>;
};

struct allow_copies {
    template<typename T>
    using allow = bool_<
        !std::is_reference<T>::value &&
        !std::is_pointer<T>::value &&
        !std::is_rvalue_reference<T>::value &&
        !has_element_type<typename type_traits::remove_accessors<T>::type>::value
    >;
};

template<typename TValueType>
struct allow_type {
    template<typename T>
    using allow = std::is_same<T, TValueType>;
};

template<typename TExpr>
struct allow_type_expr {
    //template<typename T>
    //using allow = ::mpl::apply<TExpr, T>::type;
};

template<typename TScope>
struct allow_scope {
    template<typename T>
    using allow = std::is_same<T, TScope>;
};

template<>
struct allow_scope<scopes::deduce>; // disabled

template<typename... Ts>
class restrict_types {
    struct not_resolved {
        typedef not_resolved type;

        template<typename>
        struct rebind {
            typedef not_resolved other;
        };
    };

    template<typename T>
    using is_resolvable = bool_<
        !std::is_same<
            typename T::binder::template resolve<
                typename T::type
              , type_list<>
              , not_resolved
            >::type
          , not_resolved
        >::value
    >;

    template<typename T, typename TAllow>
    struct is_type_restricted
        : TAllow::template allow<T>
    { };

    template<typename T>
    using assert_types = or_<
        bool_<is_type_restricted<typename T::type, Ts>::value>...
    >;

public:
    template<typename T>
    void assert_policy() const {
        static_assert(assert_types<T>::value, "Type is not allowed");
    }
};

} // namespace policies
} // namespace di
} // namespace boost

#endif


