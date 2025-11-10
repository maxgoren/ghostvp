#ifndef stack_hpp
#define stack_hpp

template <class T>
class InspectableStack {
    private:
        T *data;
        int maxN;
        int sp;
        void grow() {
            T* tmp = data;
            data = new T[2*maxN];
            for (int i = 0; i < sp; i++)
                data[i] = tmp[i];
            delete [] tmp;
            maxN *= 2;
        }
    public:
        InspectableStack() {
            sp = 0;
            maxN = 50;
            data = new T[maxN];
        }
        InspectableStack(const InspectableStack& st) {
            sp = 0;
            maxN = st.maxN;
            data = new T[maxN];
            for (int i = 0; i < st.sp; i++)
                data[i] = st.data[i];
        }
        InspectableStack& operator=(const InspectableStack& st) {
            if (this == &st)
                return *this;
            sp = 0;
            maxN = st.maxN;
            data = new T[maxN];
            for (int i = 0; i < st.sp; i++)
                data[i] = st.data[i];
            return *this;    
        }
        ~InspectableStack() {
            delete [] data;
        }
        bool empty() {
            return sp == 0;
        }
        int size() {
            return sp;
        }
        void push(T info) {
            if (sp+1 == maxN)
                grow();
            data[sp++] = info;
        }
        T pop() {
            return data[--sp]; 
        }
        T& top() {
            return data[sp-1];
        }
        T& get(int i) {
            return data[i];
        }
};

#endif