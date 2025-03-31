

extern "C" {

void dylib_symbol_1();
void staticlib_symbol_1();

}

int main() {
    dylib_symbol_1();
    staticlib_symbol_1();
}