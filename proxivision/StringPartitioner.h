#include <FXString.h>

class StringPartitioner {
  FX::FXString const  multi;
  FX::FXchar const    at;

public:
  mutable FX::FXint beg;
  mutable FX::FXint end;

  StringPartitioner(FX::FXString const& with_tokens, FX::FXchar token) :
    multi(with_tokens), at(token), beg(0), end(0) { }

  void reset(void) { beg = end = 0; }
  FX::FXbool next(void) const {
    beg = end; //if(!multi[beg]) return false;
    while(multi[beg] == at) beg++;
    end = beg; while(multi[end] && multi[end] != at) end++;
    if(end == beg) return false;
    return true;
  }
  FX::FXString left(void) const { return multi.left(end); }
  FX::FXString mid(void) const { return multi.mid(beg, end-beg); }
  FX::FXString right(void) const { return multi.right(multi.length() - end - 1); }
};
