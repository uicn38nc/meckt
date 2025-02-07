#include "Parser.hpp"
#include <filesystem>

using namespace Parser;
using namespace Parser::Impl;

////////////////////////////////
//         Node class         //
////////////////////////////////

Node::Node() :
    m_Value(MakeShared<NodeHolder>()),
    m_Depth(0)
{}

Node::Node(const Node& node) :
    m_Value((node.m_Value == nullptr) ? nullptr : node.m_Value->Copy()),
    m_Depth(0)
{}

Node::Node(const RawValue& value) : 
    m_Value(MakeShared<LeafHolder>(value)),
    m_Depth(0)
{}

Node::Node(const sf::Color& color) : 
    m_Value(MakeShared<LeafHolder>(std::vector<double>{(double) color.r, (double) color.g, (double) color.b})), 
    m_Depth(0)
{}

Node::Node(const std::map<Key, std::pair<Operator, Node>>& values) :
    m_Value(MakeShared<NodeHolder>(values)),
    m_Depth(0)
{}

ValueType Node::GetType() const {
    return m_Value->GetType();
}

bool Node::Is(ValueType type) const {
    return this->GetType() == type;
}

bool Node::IsList() const {
    ValueType t = this->GetType();
    return (t == ValueType::NUMBER_LIST)
        || (t == ValueType::BOOL_LIST)
        || (t == ValueType::STRING_LIST);

}

uint Node::GetDepth() const {
    return m_Depth;
}

void Node::SetDepth(uint depth) {
    m_Depth = depth;
    m_Value->SetDepth(depth);
}

void Node::Push(const RawValue& value) {
    if(this->GetType() == ValueType::NODE)
        throw std::runtime_error("error: invalid use of 'Node::Push' on non-leaf node.");
    
    auto holder = this->GetLeafHolder();

    #define CreateList(T) holder->m_Value = std::vector<T>{std::get<T>(holder->m_Value)};

    // Check if the value to push into the list
    // is a single value or another list.
    #define PushToList(T) \
        if(value.index() < 3) { \
            std::get<std::vector<T>>(holder->m_Value).push_back(std::get<T>(value)); \
        } \
        else { \
            for(const auto& v : std::get<std::vector<T>>(value)) \
                std::get<std::vector<T>>(holder->m_Value).push_back(v); \
        } \

    switch(this->GetType()) {
        case ValueType::NUMBER:
            CreateList(double);
            PushToList(double);
            break;
        case ValueType::BOOL:
            CreateList(bool);
            PushToList(bool);
            break;
        case ValueType::STRING:
            CreateList(std::string);
            PushToList(std::string);
            break;
        case ValueType::NUMBER_LIST:
            PushToList(double);
            break;
        case ValueType::BOOL_LIST:
            PushToList(bool);
            break;
        case ValueType::STRING_LIST:
            PushToList(std::string);
            break;
        default:
            break;
    }
}

Node& Node::Get(const Key& key) {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::Get' on leaf node.");
    return this->GetNodeHolder()->m_Values[key].second;
}

const Node& Node::Get(const Key& key) const {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::Get' on leaf node.");
    return this->GetNodeHolder()->m_Values[key].second;
}

template <typename T>
T Node::Get(const Key& key, T defaultValue) const {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::Get' on leaf node.");
    auto it = this->GetNodeHolder()->m_Values.find(key);
    if(it == this->GetNodeHolder()->m_Values.end())
        return defaultValue;
    return it->second.second;
}

template int Node::Get<int>(const Key&, int) const;
template double Node::Get<double>(const Key&, double) const;
template bool Node::Get<bool>(const Key&, bool) const;
template std::string Node::Get<std::string>(const Key&, std::string) const;
template Date Node::Get<Date>(const Key&, Date) const;
template ScopedString Node::Get<ScopedString>(const Key&, ScopedString) const;
template std::vector<double> Node::Get<std::vector<double>>(const Key&, std::vector<double>) const;
template std::vector<bool> Node::Get<std::vector<bool>>(const Key&, std::vector<bool>) const;
template std::vector<std::string> Node::Get<std::vector<std::string>>(const Key&, std::vector<std::string>) const;
template RawValue Node::Get<RawValue>(const Key&, RawValue) const;
template Key Node::Get<Key>(const Key&, Key) const;
template sf::Color Node::Get<sf::Color>(const Key&, sf::Color) const;

Operator Node::GetOperator(const Key& key) {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::GetOperator' on leaf node.");
    return this->GetNodeHolder()->m_Values[key].first;
}

std::map<Key, std::pair<Operator, Node>>& Node::GetEntries() {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::GetEntries' on leaf node.");
    return this->GetNodeHolder()->m_Values;
}

const std::map<Key, std::pair<Operator, Node>>& Node::GetEntries() const {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::GetEntries' on leaf node.");
    return this->GetNodeHolder()->m_Values;
}

std::vector<Key> Node::GetKeys() const {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::GetKeys' on leaf node.");
    
    std::vector<Key> keys;
    auto holder = this->GetNodeHolder();
    for(const auto& [key, pair] : holder->m_Values)
        keys.push_back(key);

    return keys;
}

bool Node::ContainsKey(const Key& key) const{
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::ContainsKey' on leaf node.");
    return this->GetNodeHolder()->m_Values.count(key) > 0;
}

void Node::Put(const Key& key, const Node& node, Operator op) {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::Put' on leaf node.");
    this->GetNodeHolder()->m_Values[key] = std::make_pair(op, node);
    this->GetNodeHolder()->m_Values[key].second.SetDepth(m_Depth + 1);
}

void Node::Put(const Key& key, const RawValue& value, Operator op) {
    this->Put(key, Node(value), op);
}

void Node::Put(const Key& key, const sf::Color& color, Operator op) {
    this->Put(key, Node(color), op);
}

Node Node::Remove(const Key& key) {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'Node::Remove' on leaf node.");
    Node value = std::move(this->GetNodeHolder()->m_Values[key].second);
    this->GetNodeHolder()->m_Values.erase(key);
    return value;
}

Node::operator int() const {
    if(!this->Is(ValueType::NUMBER))
        throw std::runtime_error("error: invalid cast from 'node' to type 'int'");
    return (int) std::get<double>(this->GetLeafHolder()->m_Value);
}

Node::operator double() const {
    if(!this->Is(ValueType::NUMBER))
        throw std::runtime_error("error: invalid cast from 'node' to type 'double'");
    return std::get<double>(this->GetLeafHolder()->m_Value);
}

Node::operator bool() const {
    if(!this->Is(ValueType::BOOL))
        throw std::runtime_error("error: invalid cast from 'node' to type 'bool'");
    return std::get<bool>(this->GetLeafHolder()->m_Value);
}

Node::operator std::string() const {
    if(!this->Is(ValueType::STRING))
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::string'");
    return std::get<std::string>(this->GetLeafHolder()->m_Value);
}

Node::operator Date() const {
    if(!this->Is(ValueType::DATE))
        throw std::runtime_error("error: invalid cast from 'node' to type 'Date'");
    return std::get<Date>(this->GetLeafHolder()->m_Value);
}

Node::operator ScopedString() const {
    if(!this->Is(ValueType::SCOPED_STRING))
        throw std::runtime_error("error: invalid cast from 'node' to type 'ScopedString'");
    return std::get<ScopedString>(this->GetLeafHolder()->m_Value);
}

Node::operator std::vector<double>&() const {
    if(!this->Is(ValueType::NUMBER_LIST))
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::vector<double>&'");
    return std::get<std::vector<double>>(this->GetLeafHolder()->m_Value);
}

Node::operator std::vector<bool>&() const {
    if(!this->Is(ValueType::BOOL_LIST))
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::vector<bool>&'");
    return std::get<std::vector<bool>>(this->GetLeafHolder()->m_Value);
}

Node::operator std::vector<std::string>&() const {
    if(!this->Is(ValueType::STRING_LIST))
        throw std::runtime_error("error: invalid cast from 'node' to type 'std::vector<std::string>&'");
    return std::get<std::vector<std::string>>(this->GetLeafHolder()->m_Value);
}

Node::operator RawValue&() const {
    if(this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid cast from 'node' to type 'RawValue&'");
    return this->GetLeafHolder()->m_Value;
}

Node::operator Key() const {
    switch(this->GetType()) {
        case ValueType::NUMBER:
            return std::get<double>(this->GetLeafHolder()->m_Value);
        case ValueType::STRING:
            return std::get<std::string>(this->GetLeafHolder()->m_Value);
        case ValueType::DATE:
            return std::get<Date>(this->GetLeafHolder()->m_Value);
        case ValueType::SCOPED_STRING:
            return std::get<ScopedString>(this->GetLeafHolder()->m_Value);
        default:
            throw std::runtime_error("error: invalid cast from 'node' to type 'Key'");
    }
}

Node::operator sf::Color() const {
    if(!this->Is(ValueType::NUMBER_LIST))
        throw std::runtime_error("error: invalid cast from 'node' to type 'sf::Color&'");
    
    std::vector<double> values = std::get<std::vector<double>>(this->GetLeafHolder()->m_Value);
    
    if(values.size() < 3)
        throw std::runtime_error("error: invalid cast from 'node' to type 'sf::Color&'");

    // Check if the numbers are float between 0 and 1.
    // If so, then parse the list as an HSV color code.
    // TODO: Fix issues where it is parsed as HSV for low RGB values.
    if((values[0] >= 0.0 && values[0] < 1.0)
    && (values[1] >= 0.0 && values[1] < 1.0)
    && (values[2] >= 0.0 && values[2] < 1.0)) {
        float h = values[0] * 360.f;
        float s = values[1];
        float v = values[2];
        return sf::HSVColor(h, s, v).toRgb();
    }

    return sf::Color((int) values[0], (int) values[1], (int) values[2]);
}

Node& Node::operator=(const RawValue& value) {
    if(this->GetType() == ValueType::NODE)
        m_Value = MakeShared<LeafHolder>(value);
    else
        this->GetLeafHolder()->m_Value = value;
    return *this;
}

Node& Node::operator=(const Node& value) {
    if(!this->Is(ValueType::NODE) && !value.Is(ValueType::NODE))
        this->GetLeafHolder()->m_Value = value.GetLeafHolder()->m_Value;
    else {
        auto holder = value.m_Value->Copy();
        holder->SetDepth(m_Depth);
        m_Value = value.m_Value->Copy();
    }
    return *this;
}

Node& Node::operator [](const Key& key) {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'operator[]' on leaf node.");
    return this->Get(key);
}

const Node& Node::operator [](const Key& key) const {
    if(!this->Is(ValueType::NODE))
        throw std::runtime_error("error: invalid use of 'operator[] const' on leaf node.");
    return this->Get(key);
}

SharedPtr<LeafHolder> Node::GetLeafHolder() {
    return std::dynamic_pointer_cast<LeafHolder>(m_Value);
}

const SharedPtr<LeafHolder> Node::GetLeafHolder() const {
    return std::dynamic_pointer_cast<LeafHolder>(m_Value);
}

SharedPtr<NodeHolder> Node::GetNodeHolder() {
    return std::dynamic_pointer_cast<NodeHolder>(m_Value);
}

const SharedPtr<NodeHolder> Node::GetNodeHolder() const {
    return std::dynamic_pointer_cast<NodeHolder>(m_Value);
}

////////////////////////////////
//      NodeHolder class      //
////////////////////////////////


NodeHolder::NodeHolder() {

}

NodeHolder::NodeHolder(const NodeHolder& n) {
    for(auto [key, pair] : n.m_Values) {
        m_Values[key] = *(&pair);
    }
}

NodeHolder::NodeHolder(const std::map<Key, std::pair<Operator, Node>>& values) {
    m_Values = values;
}

ValueType NodeHolder::GetType() const {
    return ValueType::NODE;
}

SharedPtr<AbstractValueHolder> NodeHolder::Copy() const {
    SharedPtr<NodeHolder> copy = MakeShared<NodeHolder>();
    for(const auto&[key, pair] : m_Values) {
        copy->m_Values[key] = *(&pair);
    }
    return copy;
}

void NodeHolder::SetDepth(uint depth) {
    for(auto& [key, pair] : m_Values)
        pair.second.SetDepth(depth + 1);
}

////////////////////////////////
//      LeafHolder class      //
////////////////////////////////

LeafHolder::LeafHolder() : m_Value(0.0) {}

LeafHolder::LeafHolder(const LeafHolder& l) : m_Value(l.m_Value) {}

LeafHolder::LeafHolder(const RawValue& value) : m_Value(value) {}

ValueType LeafHolder::GetType() const {
    return (ValueType) m_Value.index();
}

SharedPtr<AbstractValueHolder> LeafHolder::Copy() const {
    SharedPtr<LeafHolder> copy = MakeShared<LeafHolder>();
    copy->m_Value = m_Value;
    return copy;
}

void LeafHolder::SetDepth(uint depth) {}

Node Parser::ParseFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string content = File::ReadString(file);
    file.close();
    return Parse(content);
}

Node Parser::ParseFile(std::ifstream& file) {
    std::string content = File::ReadString(file);
    return Parse(content);
}

Node Parser::Parse(const std::string& content) {
    std::deque<PToken> tokens = Parser::Lex(content);
    Node node = Parse(tokens);
    node.SetDepth(0);
    return node;
}

Node Parser::Parse(std::deque<PToken>& tokens, uint depth) {
    enum ParsingState { KEY, OPERATOR, VALUE };
    ParsingState state = KEY;

    Node values;
    Key key;
    Operator op = Operator::EQUAL;

    while(!tokens.empty()) {
        PToken token = tokens.front();
        tokens.pop_front();

        if(token->Is(TokenType::RIGHT_BRACE)) {
            values.SetDepth(depth);
            return values;
        }

        // Comments are discarded in the lexer (until they are actually used).
        // if(token->Is(TokenType::COMMENT))
        //     continue;

        switch(state) {
            case KEY:
                if(token->Is(TokenType::IDENTIFIER)) {
                    key = ParseIdentifier(token, tokens);
                    state = ParsingState::OPERATOR;
                    break;
                }
                
                if(token->Is(TokenType::NUMBER)) {
                    key = std::get<double>(token->GetValue());
                    state = ParsingState::OPERATOR;
                    break;
                }
                
                if(token->Is(TokenType::DATE)) {
                    key = std::get<Date>(token->GetValue());
                    state = ParsingState::OPERATOR;
                    break;
                }

                throw std::runtime_error(fmt::format("Unexpected token while parsing key (type={}).", (int) token->GetType()));
                break;

            case OPERATOR:
                if(token->Is(TokenType::EQUAL)
                    || token->Is(TokenType::GREATER)
                    || token->Is(TokenType::GREATER_EQUAL)
                    || token->Is(TokenType::LESS)
                    || token->Is(TokenType::LESS_EQUAL)
                ) {
                    state = ParsingState::VALUE;
                    op = (Operator)(((int) token->GetType()) - 3);
                    break;
                }
                fmt::println("{}", key);
                throw std::runtime_error(fmt::format("Unexpected token while parsing operator (key={}, type={}).", key, (int) token->GetType()));
                break;
                
            case VALUE:
                tokens.push_front(token);
                Node node = ParseNode(tokens);

                if(values.ContainsKey(key)) {
                    Node& current = values[key];

                    if(!current.Is(ValueType::NODE)) {
                        current.Push((RawValue) node);
                    }

                    // TODO: handle array of nodes?
                }
                else {
                    values.Put(key, std::move(node), op);
                }
                state = ParsingState::KEY;
                break;
        }
    }

    // Only the root (first of Parse) can be defined without being
    // enclosed by curly brackets.
    if(depth > 0)
        throw std::runtime_error("error: missing closing curly bracket ('}').");
    
    if(state == ParsingState::OPERATOR)
        throw std::runtime_error(fmt::format("error: unexpected end after key {}.", key));
    if(state == ParsingState::VALUE)
        throw std::runtime_error(fmt::format("error: unexpected end after operator {}.", op));

    values.SetDepth(depth);
    return values;
}

Node Parser::Impl::ParseNode(std::deque<PToken>& tokens) {
    PToken token = tokens.front();
    tokens.pop_front();

    // Handle RANGE keyword by generating a list of all the numbers between A and B
    // as in: RANGE { A  B }
    if(token->Is(TokenType::IDENTIFIER) && std::get<std::string>(token->GetValue()) == "RANGE") {
        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing range.");

        // Remove LEFT_BRACE token from the list.
        token = tokens.front();
        tokens.pop_front();

        if(!token->Is(TokenType::LEFT_BRACE))
            throw std::runtime_error("error: unexpected token while parsing range.");

        return ParseRange(tokens);
    }

    // Skip LIST keyword.
    if(token->Is(TokenType::IDENTIFIER) && std::get<std::string>(token->GetValue()) == "LIST") {
        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing list.");

        // Remove LEFT_BRACE token from the list.
        token = tokens.front();
        tokens.pop_front();

        if(!token->Is(TokenType::LEFT_BRACE))
            throw std::runtime_error("error: unexpected token while parsing list.");
    }

    // Skip hsv keyword
    if(token->Is(TokenType::IDENTIFIER) && std::get<std::string>(token->GetValue()) == "hsv"
    && !tokens.empty() && tokens.front()->Is(TokenType::LEFT_BRACE)) {
        // Remove LEFT_BRACE token from the list.
        token = tokens.front();
        tokens.pop_front();
    }

    // Handle simple/raw values such as number, bool, string...
    else if(!token->Is(TokenType::LEFT_BRACE)) {
        return ParseRaw(token, tokens);
    }

    // Handle lists: { 1 2 3 4 5 }
    // Check if two successive tokens are of the same type.
    // So, if there isn't any operators for the second tokens,
    // it has to be a list.
    if(IsList(tokens)) {
        // The current token, which is LEFT_BRACE, won't be useful, so we can discard it.
        token = tokens.front();

        if(token->Is(TokenType::NUMBER))
            return ParseList<double>(tokens);
        
        if(token->Is(TokenType::IDENTIFIER) || token->Is(TokenType::STRING))
            return ParseList<std::string>(tokens);
    }
    
    return Parse(tokens, 1);
    // throw std::runtime_error("error: failed to parse node value.");
}

Node Parser::Impl::ParseRaw(PToken token, std::deque<PToken>& tokens) {
    switch(token->GetType()) {
        case TokenType::BOOLEAN:
            return Node(std::get<bool>(token->GetValue()));
        case TokenType::DATE:
            return Node(std::get<Date>(token->GetValue()));
        case TokenType::IDENTIFIER:
            return ParseIdentifier(token, tokens);
        case TokenType::NUMBER:
            return Node(std::get<double>(token->GetValue()));
        case TokenType::STRING:
            return Node(std::get<std::string>(token->GetValue()));
        default:
            throw std::runtime_error("error: unexpected token while parsing value.");
    }
}

Node Parser::Impl::ParseIdentifier(PToken token, std::deque<PToken>& tokens) {
    if(!token->Is(TokenType::IDENTIFIER))
        throw std::runtime_error("error: unexpected token while parsing string.");

    std::string firstValue = std::get<std::string>(token->GetValue());

    // Check if the token is a scope such as in: "scope:value"
    if(tokens.size() < 2 || !tokens.at(0)->Is(TokenType::TWO_DOTS) || !tokens.at(1)->Is(TokenType::IDENTIFIER))
        return Node(firstValue);

    std::string secondValue = std::get<std::string>(tokens.at(1)->GetValue());
    tokens.pop_front();
    tokens.pop_front();
    
    return Node(ScopedString(firstValue, secondValue));
}

Node Parser::Impl::ParseRange(std::deque<PToken>& tokens) {
    
    // First, check if the first two tokens are numbers
    // and initialize the minimum and maximum between
    // the two, as default for the range.
    if(tokens.size() < 3)
        throw std::runtime_error("error: unexpected end while parsing range.");
    
    PToken firstToken = tokens.at(0);
    PToken secondToken = tokens.at(1);
    
    tokens.pop_front();
    tokens.pop_front();

    if(!firstToken->Is(TokenType::NUMBER) || !secondToken->Is(TokenType::NUMBER))
        throw std::runtime_error("error: unexpected token while parsing range.");

    if(tokens.empty())
        throw std::runtime_error("error: unexpected end while parsing list.");

    int first = (int) std::get<double>(firstToken->GetValue());
    int second = (int) std::get<double>(secondToken->GetValue());
    int min = std::min(first, second), max = std::max(first, second);

    // Loop over the list and keep the minimum and the maximum,
    // then generate a list/vector of all the numbers in that range.
    PToken token;

    // The RIGHT_BRACE token must be removed from the list before returning.
    // The first token exists because the size of the list has been checked
    // at the beginning of the function.
    token = tokens.front();
    tokens.pop_front();

    while(!token->Is(TokenType::RIGHT_BRACE)) {
        if(!token->Is(TokenType::NUMBER))
            throw std::runtime_error("error: unexpected token while parsing range.");
        
        int n = (int) std::get<double>(token->GetValue());
        min = std::min(min, n);
        max = std::max(max, n);

        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing range.");
        
        token = tokens.front();
        tokens.pop_front();
    }

    std::vector<double> list;
    for(int i = min; i <= max; i++)
        list.push_back((double) i);

    return Node(list);
}

template<typename T>
Node Parser::Impl::ParseList(std::deque<PToken>& tokens) {
    std::vector<T> list;
    PToken token;
    
    // The RIGHT_BRACE token must be removed from the list before returning.
    token = tokens.front();
    tokens.pop_front();

    while(!token->Is(TokenType::RIGHT_BRACE)) {
        list.push_back(std::get<T>(token->GetValue()));

        if(tokens.empty())
            throw std::runtime_error("error: unexpected end while parsing list.");
        
        token = tokens.front();
        tokens.pop_front();
    }
    
    return Node(list);
}
    
bool Parser::Impl::IsList(std::deque<PToken>& tokens) {
    if(tokens.size() < 2)
        return false;

    PToken firstToken = tokens.at(0);
    PToken secondToken = tokens.at(1);

    #define IS_LIST_TYPE(t) (t->Is(TokenType::IDENTIFIER) || t->Is(TokenType::NUMBER) || t->Is(TokenType::STRING))

    // Check if two successive tokens are of the same type.
    // So, if there isn't any operators for the second tokens,
    // it has to be a list.

    // Or if there is only an element in the list, check if the second
    // token is a RIGHT_BRACE.
    return (secondToken->Is(firstToken->GetType()) && IS_LIST_TYPE(secondToken))
        || (secondToken->Is(TokenType::RIGHT_BRACE) && IS_LIST_TYPE(firstToken));
}

void Parser::Benchmark() {
    sf::Clock clock;

    // std::string filePath = "test_mod/map_data/default.map";
    std::string filePath = "test_mod/parser_test.txt";

    // std::string filePath = "test_mod/map_data/island_region.txt";
    // std::string filePath = "test_mod/map_data/geographical_regions/00_agot_geographical_region.txt";
    // std::string filePath = "test_mod/history/characters/00_agot_char_vale_ancestors.txt";

    std::ifstream file(filePath);

    std::string content = File::ReadString(file);
    file.close();
    
    sf::Time elapsedFile = clock.restart();

    std::deque<PToken> tokens = Parser::Lex(content);
    int tokensCount = tokens.size();

    sf::Time elapsedLexer = clock.restart();

    Node result = Parser::Parse(tokens);
    sf::Time elapsedParser = clock.getElapsedTime();

    fmt::println("file path = {}", filePath);
    fmt::println("file size = {}", String::FileSizeFormat(std::filesystem::file_size(filePath)));
    fmt::println("tokens = {}", tokensCount);
    fmt::println("entries = {}", result.GetEntries().size());
    fmt::println("elapsed file   = {}", String::DurationFormat(elapsedFile));
    fmt::println("elapsed lexer  = {}", String::DurationFormat(elapsedLexer));
    fmt::println("elapsed parser = {}", String::DurationFormat(elapsedParser));
    fmt::println("elapsed total  = {}", String::DurationFormat(elapsedFile + elapsedLexer + elapsedParser));

    // Display the result.
    fmt::println("\n------------------RESULT--------------------\n");
    fmt::println("{}", result);
    fmt::println("\n--------------------------------------------\n");

    ///////////////////////////
    // Test default.map file //
    ///////////////////////////
    
    if(filePath == "test_mod/map_data/default.map") {
        fmt::println("\nlakes = {}", result.Get("lakes"));
        fmt::println("\nisland_region => {}", result.Get("island_region"));
    }

    ///////////////////////////////
    // Test parser_test.txt file //
    ///////////////////////////////

    if(filePath == "test_mod/parser_test.txt") {
        // Test number list.
        fmt::println("\nlist = {}", result.Get("list"));
            
        // Test range.
        fmt::println("\nrange = {}", result.Get("range"));
        
        // Test string list.
        fmt::println("\nstring_list = {}", result.Get("string_list"));
        
        // Test for single raw values.
        fmt::println("\nidentifier => {}", result.Get("identifier"));
        fmt::println("string => {}", result.Get("string"));
        fmt::println("number => {}", result.Get("number"));
        fmt::println("bool => {}", result.Get("bool"));
        fmt::println("date => {}", result.Get("date"));
        fmt::println("scope:value => {}", result.Get(ScopedString("scope", "value")));
        // fmt::println("scope => {}", (ScopedString) result.Get("scope"));
        
        // Test date as key
        fmt::println("1000.1.1 => {}", result.Get(Date(1000, 1, 1)));
        
        // Test utf8 characters as value
        fmt::println("utf8 => {}", result.Get("utf8"));

        // Test depth
        fmt::println("depth: {}", result.Get("depth").GetDepth());
        fmt::println("depth.name: {} {}", result.Get("depth").Get("name"), result.Get("depth").Get("name").GetDepth());
        fmt::println("depth.1: {}", result.Get("depth").Get(1.0).GetDepth());
        fmt::println("depth.1.name: {} {}", result.Get("depth").Get(1.0).Get("name"), result.Get("depth").Get(1.0).Get("name").GetDepth());
        fmt::println("depth.1.2: {}", result.Get("depth").Get(1.0).Get(2.0).GetDepth());
        fmt::println("depth.1.2.name: {} {}", result.Get("depth").Get(1.0).Get(2.0).Get("name"), result.Get("depth").Get(1.0).Get(2.0).Get("name").GetDepth());
        fmt::println("depth.1.3: {}", result.Get("depth").Get(1.0).Get(2.0).Get(3.0).GetDepth());
        fmt::println("depth.1.3.name: {} {}", result.Get("depth").Get(1.0).Get(2.0).Get(3.0).Get("name"), result.Get("depth").Get(1.0).Get(2.0).Get(3.0).Get("name").GetDepth());
        
        fmt::println("operators = {}", result.Get("operators"));
    }
}