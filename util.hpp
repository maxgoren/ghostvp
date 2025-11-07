#ifndef util_hpp
#define util_hpp
#include <iostream>
using namespace std;

class DepthTracker {
    private:
        int depth;
    public:
        DepthTracker() {
            depth = 0;
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
            for (int i = 0; i < depth; i++) cout<<" ";
            cout<<s<<endl;
        }
        void enter(string s) {
            enter();
            say(s);
        }
};

#endif