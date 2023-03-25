#pragma once

#include "json_serializer.h"
#include "binary_serialize.h"
#include "xml_serializer.h"
#include "text_serializer.h"

#define SL_PROPERTY(A, B) sl::basic_property(&A, B)

#define SL_ARRAY_PROPERTY(A, B, C) sl::array_property(&A, &B, C)

#define SL_ARRAY_PROPERTY_WITH_ALLOCATOR(A, B, C, D) sl::builder_with_allocator<D>::array_property_with_allocator(&A, &B, C)

#define SL_AVOID_PROPERTY(A) sl::detail::avoid_property(A)

#define SL_OPTIONAL_PROPERTY(A, B) sl::optional_basic_property(&A, B)

#define SL_DUAL_PROPERTY(A, B, C) sl::dual_property(&A, &B, C)

#define SL_PROPERTIES static constexpr auto properties = std::make_tuple

#define SL_TMP_PROPERTIES static constexpr auto tmp_properties = std::make_tuple

#define SL_SERIALIZABLE friend class sl::access

#define SL_SERIALIZE_CLASS(A) namesapce sl{namespace serialization{namespace detail{\
                                    template<> struct class_data<A>{\
                                    static constexpr unsigned int version = 0;\
                                    static constexpr char name[] = #A;\
                                    static constexpr size_t size = dim(name) - 1;};}}}

#define SL_SERIALIZE_CLASS_VERSION_AND_NAME(A, version_value, NAME) namesapce sl{namespace serialization{namespace detail{\
                                                    template<> struct class_data<A>{\
                                                    static constexpr unsigned int version = version_value;\
                                                    static constexpr char name[] = NAME;\
                                                    static constexpr size_t size = dim(name) - 1;};}}}

#define SL_SERIALIZE_CLASS_NAME(A, NAME) namespace sl{namespace serialization{namespace detail{\
                                                    template<> struct class_data<A>{\
                                                    static constexpr unsigned int version = 0;\
                                                    static constexpr char name[] = NAME;\
                                                    static constexpr size_t size = dim(name) - 1;};}}}

#define COMBINE1(X, Y) X ## Y
#define COMBINE(X, Y) COMBINE1(X, Y)

#define SL_SERIALIZE_LINK_DERIVED(A, B) namespace {namespace COMBINE(PRIVATE, __LINE__){\
                                            struct link_register{link_register(){\
                                                using namespace sl;\
                                                detail::link_derived<json_iarchive, json_oarchive, A, B>();\
                                                detail::link_derived<json_iarchive, json_oarchive, A>();\
                                                detail::link_derived<json_iarchive, json_oarchive, B>();\
                                                detail::link_derived<binary_iarchive, binary_oarchive, A, B>();\
                                                detail::link_derived<binary_iarchive, binary_oarchive, A>();\
                                                detail::link_derived<binary_iarchive, binary_oarchive, B>();\
                                                detail::link_derived<xml_iarchive, xml_oarchive, A, B>();\
                                                detail::link_derived<xml_iarchive, xml_oarchive, A>();\
                                                detail::link_derived<xml_iarchive, xml_oarchive, B>();\
                                                detail::link_derived<text_iarchive, text_oarchive, A, B>();\
                                                detail::link_derived<text_iarchive, text_oarchive, A>();\
                                                detail::link_derived<text_iarchive, text_oarchive, B>();}};\
                                                link_register tmp;}}

#define SL_CAST_BASE(A, B) sl::cast_base<A>(B)