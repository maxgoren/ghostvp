#ifndef util_hpp
#define util_hpp
#include <iostream>
using namespace std;

class DepthTracker {
    private:
        int depth;
        bool loud;
    public:
        DepthTracker(bool trace = false) {
            depth = 0;
            loud = trace;
        }
        void reset() {
            depth = 0;
        }
        void enter() {
            depth++;
        }
        void leave() {
            depth--;
        }
        void say(string s) {
            if (loud == false)
                return;
            for (int i = 0; i < depth; i++) cout<<" ";
            cout<<s<<endl;
        }
        void enter(string s) {
            enter();
            say(s);
        }
};

#endif