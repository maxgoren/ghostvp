#include <iostream>
#include "util.hpp"
using namespace std;

int main(int argc, char* argv[]) {
	FileStringBuffer fsb;
	fsb.readFile("util.hpp");
    while (!fsb.done()) {
        fsb.get();
        fsb.advance();
    }
}
