#include <iostream>
#include <tuple>
#include <array>
#include <cstddef>

///TypeList
template <class... Ts>
struct TypeList {};

///TypeListIndex
template <class T, class L>
struct TypeListIndex;

template <class T>
struct TypeListIndex<T, TypeList<>> {
    static constexpr int remainder = -1;
    static constexpr int value = -1;
};

template <class T, class... Others>
struct TypeListIndex<T, TypeList<T, Others...>> {
    static constexpr int remainder = sizeof...(Others);
    static constexpr int value = 0;
};

template <class T, class U, class... Others>
struct TypeListIndex<T, TypeList<U, Others...>> {
    static constexpr int remainder = TypeListIndex<T, TypeList<Others...>>::remainder;
    static constexpr int value = (remainder==-1) ? -1 : 1 + TypeListIndex<T, TypeList<Others...>>::value;

    static_assert(value!=-1, "can't find type in TypeListIndex!!");
};


template <class From, class To>
using ExpandTo = To;

namespace gl {

///DataType
enum DataType {
    Byte1,
    Byte2,
    Word,
    Word2,
    Float1,
    Float2,
    Float3,
    Float4,
};

///AttributeDataTypeOf
template <class T, size_t N>
struct AttributeDataTypeOf;

template<>struct AttributeDataTypeOf<short, 2> : std::integral_constant<DataType, Word2> {};
template<>struct AttributeDataTypeOf<float, 1> : std::integral_constant<DataType, Float1> {};
template<>struct AttributeDataTypeOf<float, 2> : std::integral_constant<DataType, Float2> {};
template<>struct AttributeDataTypeOf<float, 3> : std::integral_constant<DataType, Float3> {};
template<>struct AttributeDataTypeOf<float, 4> : std::integral_constant<DataType, Float4> {};

///AttributeType
template <class T, size_t N>
struct AttributeType {
    static constexpr DataType datatype = AttributeDataTypeOf<T, N>::value;
    using Value = std::array<T, N>;
};

namespace detail {
///VertexData
template <class... Ts>
struct VertexData;

template <class T1>
struct VertexData<T1> {
    typename T1::Value a1;
};

template <class T1, class T2>
struct VertexData<T1, T2> {
    typename T1::Value a1;
    typename T2::Value a2;
};

template <class T1, class T2, class T3>
struct VertexData<T1, T2, T3> {
    typename T1::Value a1;
    typename T2::Value a2;
    typename T3::Value a3;
};

///AttributeData
struct AttributeData {
    DataType datatype;
    size_t offset;
};

///Descriptor
struct Descriptor {
    size_t stride;
    size_t count;
    AttributeData attributes[5];
};


///VertexDescriptor
template <class T>
struct VertexDescriptor;

template <class T1>
struct VertexDescriptor<VertexData<T1>> {
    using Type = VertexData<T1>;

    static constexpr Descriptor data = {
        sizeof(Type),
        1,
        {
            {T1::datatype, offsetof(Type, a1)}
        }
    };
};

template <class T1, class T2>
struct VertexDescriptor<VertexData<T1, T2>> {
    using Type = VertexData<T1, T2>;

    static constexpr Descriptor data = {
        sizeof(Type),
        2,
        {
            {T1::datatype, offsetof(Type, a1)},
            {T2::datatype, offsetof(Type, a2)}
        }
    };
};

template <class T1, class T2, class T3>
struct VertexDescriptor<VertexData<T1, T2, T3>> {
    using Type = VertexData<T1, T2, T3>;

    static constexpr Descriptor data = {
        sizeof(Type),
        3,
        {
            {T1::datatype, offsetof(Type, a1)},
            {T2::datatype, offsetof(Type, a2)},
            {T3::datatype, offsetof(Type, a3)}
        }
    };
};
}

///AttributeBinding
struct AttributeBinding {
    size_t stride;
    void* buffer;
    size_t offset;
    DataType datatype;
};

template <class T, size_t I>
AttributeBinding attributeBinding(void* buffer) {
    static_assert(I<T::data.count, "I overflow");

    return AttributeBinding{
        T::data.stride,
        buffer,
        T::data.attributes[I].offset,
        T::data.attributes[I].datatype
    };
}

template <class T>
struct LayoutProperties;

template <class... Ts>
struct LayoutProperties<TypeList<Ts...>> {
    using Properties = TypeList<Ts...>;
    using VertexData = detail::VertexData<typename Ts::Type...>;
    using VertexDescriptor = detail::VertexDescriptor<VertexData>;

    static auto attributeBindgs(void *buffer) {
        return std::tuple<ExpandTo<Ts, AttributeBinding>...>(
            attributeBinding<VertexDescriptor, TypeListIndex<Ts, Properties>::value>(buffer)...);
    }
};

}

namespace style {
 #define DefineProperty(Name_, Type_, N_) \
    struct Name_ { \
        using Type =  gl::AttributeType<Type_, N_>; \
        static constexpr const char* Name() { \
            return #Name_; \
        } \
    }

    DefineProperty(Color, float, 4);
    DefineProperty(Position, float, 4);
    DefineProperty(Alpha, float, 4);
}

using LayoutProperties = TypeList<style::Alpha, style::Color, style::Position>;

using FillLayerLayoutProperties = gl::LayoutProperties<LayoutProperties>;

using LayoutVertexData = FillLayerLayoutProperties::VertexData;
using LayoutVertexDescriptor = FillLayerLayoutProperties::VertexDescriptor;



int main(int argc, char **argv) {
    auto bindings = FillLayerLayoutProperties::attributeBindgs(reinterpret_cast<void *>(10));

    std::cout << std::get<0>(bindings).stride << "\n";
    std::cout << std::get<0>(bindings).offset << "\n";

    std::cout << std::get<1>(bindings).stride << "\n";
    std::cout << std::get<1>(bindings).offset << "\n";

    std::cout << std::get<2>(bindings).stride << "\n";
    std::cout << std::get<2>(bindings).offset << "\n";

    return 0;
} 
