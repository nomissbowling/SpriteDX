/*
  SpriteDX.hpp
*/

#ifndef __SPRITEDX_H__
#define __SPRITEDX_H__

#pragma warning(disable:4244)
#define _CRT_SECURE_NO_WARNINGS

#include <DxLib.h>
#include <exception>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
// #include <unordered_map> // -std=c++11 or -std=gnu++11
#include <vector>

namespace SpriteDX {

using namespace std;

typedef struct _BGRA {
  unsigned char b, g, r, a;
} BGRA;

class SDXSprite;
class SDXGraph;
class SDXImage;
class SDXMask;
class SDXVector;
class SDXMap;

struct SDXIdx { size_t i; SDXIdx(size_t s) : i(s) {} };
typedef vector<SDXSprite *> sdxvec_t;
struct SDXKey { const char *k; SDXKey(const char *s) : k(s) {} };
typedef map<const string, SDXSprite *> sdxmap_t;

class SDXSprite {
  friend SDXGraph;
  friend SDXImage;
  friend SDXMask;
  friend SDXVector;
  friend SDXMap;
protected:
  int (*disposer)(SDXSprite *);
  int hnd;
public:
  virtual operator int(){ return hnd; }
  virtual SDXSprite &operator [](size_t i){ throw runtime_error("[size_t] not SDXVector"); }
  virtual SDXSprite *&operator [](SDXIdx i){ throw runtime_error("[SDXIdx] not SDXVector"); }
  virtual SDXSprite &operator [](const char *key){ throw runtime_error("[const char *] not SDXMap"); }
  virtual SDXSprite *&operator [](SDXKey key){ throw runtime_error("[SDXKey] not SDXMap"); }
  SDXSprite(int (*d)(SDXSprite *)) : disposer(d), hnd(-1) {}
  SDXSprite(const SDXSprite &o) : disposer(o.disposer), hnd(o.hnd) {}
  virtual int Dispose(){ if(hnd != -1){ disposer(this); hnd = -1; } return 0; }
  virtual ~SDXSprite(){ Dispose(); } // (when copy src may be deleted)
};

class SDXGraph : public SDXSprite {
protected:
  static inline int d_SDXGraph(SDXSprite *self){ DeleteGraph(self->hnd); return 0; } // (hnd, FALSE)
public:
  SDXGraph() : SDXSprite(d_SDXGraph) {}
  SDXGraph(int w, int h) : SDXSprite(d_SDXGraph) { hnd = MakeGraph(w, h); }
  SDXGraph(const char *fn) : SDXSprite(d_SDXGraph) { hnd = LoadGraph(fn); }
};

class SDXImage : public SDXSprite {
protected:
  static inline int d_SDXImage(SDXSprite *self){ DeleteSoftImage(self->hnd); return 0; }
public:
  SDXImage() : SDXSprite(d_SDXImage) {}
  SDXImage(int w, int h) : SDXSprite(d_SDXImage) { hnd = MakeARGB8ColorSoftImage(w, h); }
  SDXImage(const char *fn) : SDXSprite(d_SDXImage) { hnd = LoadSoftImage(fn); }
};

class SDXMask : public SDXSprite {
protected:
  static inline int d_SDXMask(SDXSprite *self){ DeleteMask(self->hnd); return 0; }
public:
  SDXMask() : SDXSprite(d_SDXMask) {}
  SDXMask(int w, int h) : SDXSprite(d_SDXMask) { hnd = MakeMask(w, h); }
  SDXMask(const char *fn) : SDXSprite(d_SDXMask) { hnd = LoadMask(fn); }
};

class SDXVector : public SDXSprite {
protected:
  static inline int d_SDXVector(SDXSprite *self){ return 0; } // do nothing
  sdxvec_t v;
public:
  virtual operator int(){ throw runtime_error("bad (int) SDXVector"); }
  virtual SDXSprite &operator [](size_t i){ return *v[i]; } // no check NULL
  virtual SDXSprite *&operator [](SDXIdx i){ if(!v[i.i]){ v.at(i.i) = NULL; } return v.at(i.i); } // set NULL to skip throw v.at first access exception std::out_of_range
  SDXVector(size_t n) : SDXSprite(d_SDXVector) { hnd = 0; v.resize(n, NULL); } // v.reserve(n);
  virtual int Dispose(){
    for(sdxvec_t::iterator i = v.begin(); i != v.end(); ++i)
      if(*i){ delete *i; *i = NULL; }
  }
  virtual ~SDXVector(){ Dispose(); }
};

class SDXMap : public SDXSprite {
protected:
  static inline int d_SDXMap(SDXSprite *self){ return 0; } // do nothing
  sdxmap_t m;
public:
  virtual operator int(){ throw runtime_error("bad (int) SDXMap"); }
  virtual SDXSprite &operator [](const char *key){ return *m[key]; } // no check NULL
  virtual SDXSprite *&operator [](SDXKey key){ if(!m[key.k]){ m.at(key.k) = NULL; } return m.at(key.k); } // set NULL to skip throw m.at first access exception std::out_of_range
  SDXMap() : SDXSprite(d_SDXMap) { hnd = 0; }
  virtual int Dispose(){
    for(sdxmap_t::iterator i = m.begin(); i != m.end(); ++i)
      if(i->second){ delete i->second; i->second = NULL; }
  }
  virtual ~SDXMap(){ Dispose(); }
};

}

#endif // __SPRITEDX_H__

