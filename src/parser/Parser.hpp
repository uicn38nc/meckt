#pragma once

#include "parser/Lexer.hpp"
#include "util/OrderedMap.hpp"

#include <fmt/format.h>
#include <fmt/compile.h>
#include <ranges>

namespace Parser {
    
    class Object;
    class AbstractHolder;
    class ScalarHolder;
    class ArrayHolder;
    class ObjectHolder;
    
    using Scalar = std::variant<int, double, bool, std::string, Date, ScopedString>;
    using Array = std::variant<std::vector<int>, std::vector<double>, std::vector<bool>, std::vector<std::string>, std::vector<Date>, std::vector<ScopedString>, std::vector<SharedPtr<Object>>>;

    enum class Operator {
        EQUAL,
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        NOT_EQUAL,
        EXIST,
    };

    enum class ObjectType {
        INT,
        DECIMAL,
        BOOL,
        STRING,
        DATE,
        SCOPED_STRING,
        OBJECT,
        ARRAY
    };

    class Object {
        public:
            Object();
            Object(const Object& object);
            Object(const Scalar& scalar);
            Object(const Array& array);
            Object(const std::vector<SharedPtr<Object>>& array);
            Object(const sf::Color& color);

            ObjectType GetType() const;
            ObjectType GetArrayType() const;
            bool Is(ObjectType type) const;

            void ConvertToArray();

            // Functions to use with ArrayHolder or ObjectHolder.
            void Push(const SharedPtr<Object>& object);
            void Push(const Scalar& scalar);
            void Push(const Array& array);
            void Merge(const SharedPtr<Object>& object);

            // Functions to use with ObjectHolder.
            template <typename T> T Get(const Scalar& key) const;
            template <typename T> T Get(const Scalar& key, T defaultValue) const;
            template <typename T> std::vector<T> GetArray(const Scalar& key) const;
            template <typename T> std::vector<T> GetArray(const Scalar& key, std::vector<T> defaultValue) const;
            SharedPtr<Object> GetObject(const Scalar& key) const;
            Operator GetOperator(const Scalar& key);

            OrderedMap<Scalar, std::pair<Operator, SharedPtr<Object>>>& GetEntries();
            const OrderedMap<Scalar, std::pair<Operator, SharedPtr<Object>>>& GetEntries() const;
            std::vector<Scalar> GetKeys() const;

            bool ContainsKey(const Scalar& key) const;
            void Put(const Scalar& key, const SharedPtr<Object>& value, Operator op = Operator::EQUAL);
            void Put(const Scalar& key, const Scalar& value, Operator op = Operator::EQUAL);
            void Put(const Scalar& key, const Array& value, Operator op = Operator::EQUAL);
            void Put(const Scalar& key, const sf::Color& value, Operator op = Operator::EQUAL);
            SharedPtr<Object> Remove(const Scalar& key);

            // Overload cast for scalars.
            Scalar& AsScalar();
            Scalar AsScalar() const;
            operator int() const;
            operator double() const;
            operator bool() const;
            operator std::string() const;
            operator Date() const;
            operator ScopedString() const;
            operator Scalar() const;

            // Overload cast for arrays
            Array& AsArray();
            const Array& AsArray() const;
            operator std::vector<int>&() const;
            operator std::vector<double>&() const;
            operator std::vector<bool>&() const;
            operator std::vector<std::string>&() const;
            operator std::vector<Date>&() const;
            operator std::vector<ScopedString>&() const;
            operator std::vector<SharedPtr<Object>>&() const;
            operator Array&() const;
            operator sf::Color() const;

            Object& operator=(const Scalar& value);
            Object& operator=(const Array& value);
            Object& operator=(const Object& value);

        private:
            // Function to access the underlying value holder.
            SharedPtr<AbstractHolder> GetHolder();
            const SharedPtr<AbstractHolder> GetHolder() const;
            
            SharedPtr<ScalarHolder> GetScalarHolder();
            const SharedPtr<ScalarHolder> GetScalarHolder() const;

            SharedPtr<ArrayHolder> GetArrayHolder();
            const SharedPtr<ArrayHolder> GetArrayHolder() const;

            SharedPtr<ObjectHolder> GetObjectHolder();
            const SharedPtr<ObjectHolder> GetObjectHolder() const;

            SharedPtr<AbstractHolder> m_Value;
    };

    class AbstractHolder {
        public:
            virtual ObjectType GetType() const = 0;
            virtual SharedPtr<AbstractHolder> Copy() const = 0;
    };

    class ScalarHolder : public AbstractHolder {
        friend Object;

        public:
            ScalarHolder();
            ScalarHolder(const ScalarHolder& holder);
            ScalarHolder(const Scalar& scalar);

            virtual ObjectType GetType() const;
            virtual SharedPtr<AbstractHolder> Copy() const;

        private:
            Scalar m_Value;
    };
    
    class ArrayHolder : public AbstractHolder {
        friend Object;

        public:
            ArrayHolder();
            ArrayHolder(const ArrayHolder& holder);
            ArrayHolder(const Array& value);

            virtual ObjectType GetType() const;
            virtual SharedPtr<AbstractHolder> Copy() const;

            ObjectType GetArrayType() const;

        private:
            Array m_Values;
    };
    
    class ObjectHolder : public AbstractHolder {
        friend Object;

        public:
            ObjectHolder();
            ObjectHolder(const ObjectHolder& holder);
            ObjectHolder(const SharedPtr<Object>& value);

            virtual ObjectType GetType() const;
            virtual SharedPtr<AbstractHolder> Copy() const;

        private:
            OrderedMap<Scalar, std::pair<Operator, SharedPtr<Object>>> m_Values;
    };

    SharedPtr<Object> ParseFile(const std::string& filePath);
    SharedPtr<Object> ParseFile(std::ifstream& file);
    SharedPtr<Object> Parse(const std::string& content);
    SharedPtr<Object> Parse(std::deque<PToken>& tokens, uint depth = 0);

    namespace Impl {
        SharedPtr<Object> ParseObject(std::deque<PToken>& tokens);
        SharedPtr<Object> ParseScalar(PToken token, std::deque<PToken>& tokens);
        SharedPtr<Object> ParseString(PToken token, std::deque<PToken>& tokens);
        SharedPtr<Object> ParseRange(std::deque<PToken>& tokens);

        template<typename T>
        SharedPtr<Object> ParseList(std::deque<PToken>& tokens);
        template <>
        SharedPtr<Object> ParseList<SharedPtr<Object>>(std::deque<PToken>& tokens);

        bool IsList(std::deque<PToken>& tokens);
    }

    namespace Format {
        template <typename T>
        std::string FormatNumbersList(const Scalar& key, const SharedPtr<Object>& object, uint depth);
        std::string FormatStringsList(const Scalar& key, const SharedPtr<Object>& object, uint depth);

        template <typename T>
        bool IsRange(const std::vector<T>& numbers);

        std::string FormatObject(const SharedPtr<Object>& object, uint depth, bool isRoot = false);
        std::string FormatObjectFlat(const SharedPtr<Object>& object, uint depth);
    }

    void Benchmark();
    void Tests();
}

///////////////////////////////////////////
//          Formatters for fmt           //
///////////////////////////////////////////

template <>
class fmt::formatter<Parser::Operator> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Parser::Operator& key, Context& ctx) const {
        switch(key) {
            case Parser::Operator::EQUAL: return format_to(ctx.out(), "=");
            case Parser::Operator::LESS: return format_to(ctx.out(), "<");
            case Parser::Operator::LESS_EQUAL: return format_to(ctx.out(), "<=");
            case Parser::Operator::GREATER: return format_to(ctx.out(), ">");
            case Parser::Operator::GREATER_EQUAL: return format_to(ctx.out(), ">=");
            case Parser::Operator::NOT_EQUAL: return format_to(ctx.out(), "!=");
            case Parser::Operator::EXIST: return format_to(ctx.out(), "?=");
        }
        return format_to(ctx.out(), "");
    }
};

template <>
class fmt::formatter<Parser::Scalar> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Parser::Scalar& key, Context& ctx) const {
        switch((Parser::ObjectType) key.index()) {
            case Parser::ObjectType::INT: return format_to(ctx.out(), "{}", std::get<int>(key));
            case Parser::ObjectType::DECIMAL: return format_to(ctx.out(), "{}", std::get<double>(key));
            case Parser::ObjectType::BOOL: return format_to(ctx.out(), "{}", (std::get<bool>(key) ? "yes" : "no"));
            case Parser::ObjectType::STRING: return format_to(ctx.out(), "{}", std::get<std::string>(key));
            case Parser::ObjectType::DATE: return format_to(ctx.out(), "{}", std::get<Date>(key));
            case Parser::ObjectType::SCOPED_STRING: return format_to(ctx.out(), "{}", std::get<ScopedString>(key));
            default: break;
        }
        return format_to(ctx.out(), "");
    }
};

template <>
class fmt::formatter<Parser::Array> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Parser::Array& array, Context& ctx) const {
        switch((Parser::ObjectType) array.index()) {
            case Parser::ObjectType::INT: {
                const std::vector<int>& numbers = std::get<std::vector<int>>(array);
                if(Parser::Format::IsRange<int>(numbers)) {
                    return format_to(ctx.out(), "RANGE {{ {} {} }}", numbers[0], numbers[numbers.size()-1]);
                }
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(numbers, [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            }
            case Parser::ObjectType::DECIMAL: {
                const std::vector<double>& numbers = std::get<std::vector<double>>(array);
                if(Parser::Format::IsRange<double>(numbers)) {
                    return format_to(ctx.out(), "RANGE {{ {} {} }}", numbers[0], numbers[numbers.size()-1]);
                }
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(numbers, [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            }
            case Parser::ObjectType::BOOL:
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(std::get<std::vector<bool>>(array), [](const auto& v) {
                        return fmt::format("{}", (v ? "yes" : "no"));
                    }), " ")
                );
            case Parser::ObjectType::STRING:
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(std::get<std::vector<std::string>>(array), [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            case Parser::ObjectType::DATE:
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(std::get<std::vector<Date>>(array), [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            case Parser::ObjectType::SCOPED_STRING:
                return format_to(ctx.out(), "{{ {} }}", fmt::join(
                    std::views::transform(std::get<std::vector<ScopedString>>(array), [](const auto& v) {
                        return fmt::format("{}", v);
                    }), " ")
                );
            case Parser::ObjectType::OBJECT: {
                const auto& objects = std::get<std::vector<SharedPtr<Parser::Object>>>(array);
                if(objects.empty())
                    return format_to(ctx.out(), "{{ }}");
                return format_to(ctx.out(), "{{\n{}\n}}", fmt::join(
                    std::views::transform(objects, [](const auto& v) {
                        return Parser::Format::FormatObjectFlat(v, 0);
                    }), "\n")
                );
            }
            default:
                return format_to(ctx.out(), "");
        }
    }
};

template <>
class fmt::formatter<Parser::Object> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const Parser::Object& object, Context& ctx) const {
        return format_to(ctx.out(), "{}", Parser::Format::FormatObject(MakeShared<Parser::Object>(object), 0, true));
    }
};

template <>
class fmt::formatter<SharedPtr<Parser::Object>> {
public:
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template <typename Context>
    constexpr auto format(const SharedPtr<Parser::Object>& object, Context& ctx) const {
        return format_to(ctx.out(), "{}", Parser::Format::FormatObject(object, 0, true));
    }
};