#include "app/App.hpp"
#include "parser/Parser.hpp"

int main() {
    App app;
    app.Init();
    app.Run();
    // Parser::Benchmark();
    return 0;
}