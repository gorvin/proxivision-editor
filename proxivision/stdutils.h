#ifndef STDUTILS_H
#define STDUTILS_H

namespace stdutils {

template<typename T> class auto_ptr {
    T* p;
    char const* msg;
    void (*fndelete)(T*);
    static void fndelete_default(T* ptr) { delete ptr; }
public:
    char const* message(void) const { return msg; }
    auto_ptr<T>& operator=(auto_ptr<T>& right) { reset(right.release()); return *this; }
    auto_ptr<T>& operator=(T* pnew) { reset(pnew); return *this; }

    T& operator*(void) const { if(!p) throw "Dereferenciras NULL ptr"; return *p; }
    T* operator->(void) const { if(!p) throw "Dereferenciras NULL ptr"; return p; }
    T* get(void) const { return p; }
    T* release(void) { T* tmp = p; p = 0; return tmp; }
    void reset(T* pnew = 0) { if(pnew == p) return; if(p) fndelete(p); p = pnew; }
    virtual ~auto_ptr(void){ if(p) fndelete(p); }
    auto_ptr(T* ptr = 0, char const* error_msg=0, void (*scope_end)(T*)=0) :
      p(ptr), msg(error_msg), fndelete(scope_end?scope_end:fndelete_default) { }
};

template<typename T> class auto_arr_ptr : public auto_ptr<T> {
  static void scope_end(T* p) { if(p) delete [] p; }
public:
  auto_arr_ptr(T* ptr=0, char const* error_string = 0) :
   auto_ptr<T>(ptr, error_string, scope_end) { }
};

template<typename T> void auto_ptr_check(auto_ptr<T> const& ap);

}

#endif //STDUTILS_H
