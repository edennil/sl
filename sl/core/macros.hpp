#pragma once

#include "json_serializer.hpp"
#include "binary_serialize.hpp"
#include "xml_serializer.hpp"
#include "text_serializer.hpp"
#include "access.hpp"
#include "basic.hpp"

#define SL_PROPERTY(A, B) sl::basic_property(&A, B)

#define SL_ARRAY_PROPERTY(A, B, C) sl::array_property(&A, &B, C)

#define SL_ARRAY_PROPERTY_WITH_ALLOCATOR(A, B, C, D) sl::builder_with_allocator<D>::array_property_with_allocator(&A, &B, C)

#define SL_AVOID_PROPERTY(A) sl::detail::avoid_property(A)

#define SL_OPTIONAL_PROPERTY(A, B) sl::optional_basic_property(&A, B)

#define SL_DUAL_PROPERTY(A, B, C) sl::dual_property(&A, &B, C)

#define SL_PROPERTIES static constexpr auto properties = std::make_tuple

#define SL_TMP_PROPERTIES static constexpr auto tmp_properties = std::make_tuple

#define SL_SERIALIZABLE friend class sl::access

#define SL_SERIALIZE_PROPERTY(A) std::make_tuple(std::make_tuple(A, this), false) 

#define SL_SERIALIZE_LAST_PROPERTY(A) std::make_tuple(A, this)

#define SL_TO_SERIALIZE(A) std::make_tuple(A, this, false)

#define SL_TO_SERIALIZE_LAST(A) std::make_tuple(A, this, true)

#define SL_SERIALIZE_CLASS(A) namespace sl{namespace detail{\
                                    template<> struct class_data<A>{\
                                    static constexpr unsigned int version = 0;\
                                    static constexpr char name[] = #A;\
                                    static constexpr size_t size = dim(name) - 1;};}}

#define SL_SERIALIZE_CLASS_VERSION_AND_NAME(A, version_value, NAME) namespace sl{namespace detail{\
                                                    template<> struct class_data<A>{\
                                                    static constexpr unsigned int version = version_value;\
                                                    static constexpr char name[] = NAME;\
                                                    static constexpr size_t size = dim(name) - 1;};}}

#define SL_SERIALIZE_CLASS_NAME(A, NAME) namespace sl{namespace detail{\
                                                    template<> struct class_data<A>{\
                                                    static constexpr unsigned int version = 0;\
                                                    static constexpr char name[] = NAME;\
                                                    static constexpr size_t size = dim(name) - 1;};}}

#define COMBINE1(X, Y) X ## Y
#define COMBINE(X, Y) COMBINE1(X, Y)

#define SL_SERIALIZE_LINK_DERIVED(BASE, DERIVED) namespace {namespace COMBINE(PRIVATE, __LINE__){\
                                            struct link_register{link_register(){\
                                                using namespace sl;\
                                                detail::link_derived<BASE, DERIVED>();\
                                                detail::link_derived<BASE>();\
                                                detail::link_derived<DERIVED>();}};\
                                                link_register tmp;}}

#define SL_CAST_BASE(A, B) sl::cast_base<A>(B)