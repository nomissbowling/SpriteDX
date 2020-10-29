/*
  SpriteDX.hpp
*/

#ifndef __SPRITEDX_H__
#define __SPRITEDX_H__

#pragma warning(disable:4244)
#define _CRT_SECURE_NO_WARNINGS

#include <DxLib.h>
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
class SDXPool;

class SDXSprite {
  friend SDXGraph;
  friend SDXImage;
  friend SDXMask;
  friend SDXVector;
  friend SDXMap;
  friend SDXPool;
protected:
  int (*disposer)(SDXSprite *);
  int hnd;
public:
  virtual operator int(){ return hnd; }
  virtual SDXSprite &operator [](size_t i){ return *this; } // no check NULL or throw
  virtual SDXSprite &operator [](const char *key){ return *this; } // no check NULL or throw
  virtual SDXSprite *const*operator [](const char *key) const { return NULL; } // no check NULL or throw
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

typedef vector<SDXSprite *> sdxvec_t;
class SDXVector : public SDXSprite {
  friend SDXPool;
protected:
  static inline int d_SDXVector(SDXSprite *self){ return 0; } // do nothing
  sdxvec_t v;
public:
  virtual operator int(){ return v[0]->hnd; } // no check NULL (safety hnd) or throw
  virtual SDXSprite &operator [](size_t i){ return *v[i]; } // no check NULL
  SDXVector(size_t n) : SDXSprite(d_SDXVector) { hnd = 0; v.reserve(n); }
  virtual int Dispose(){
    for(sdxvec_t::iterator i = v.begin(); i != v.end(); ++i)
      if(*i){ delete *i; *i = NULL; }
  }
  virtual ~SDXVector(){ Dispose(); }
};

typedef map<const string, SDXSprite *> sdxmap_t;
class SDXMap : public SDXSprite {
  friend SDXPool;
protected:
  static inline int d_SDXMap(SDXSprite *self){ return 0; } // do nothing
  sdxmap_t m;
public:
  virtual operator int(){ return m[""]->hnd; } // no check NULL (safety hnd) or throw
  virtual SDXSprite &operator [](const char *key){ return *m[key]; } // no check NULL
  virtual SDXSprite *const*operator [](const char *key) const { return &m.at(key); } // no check NULL
  SDXMap() : SDXSprite(d_SDXMap) { hnd = 0; }
  virtual int Dispose(){
    for(sdxmap_t::iterator i = m.begin(); i != m.end(); ++i)
      if(i->second){ delete i->second; i->second = NULL; }
  }
  virtual ~SDXMap(){ Dispose(); }
};

typedef map<const string, SDXSprite *> pool_t;
class SDXPool {
protected:
  pool_t m;
public:
  SDXPool() {}
  SDXSprite &operator [](const char *key){ return *m[key]; } // no check NULL
  int h(string key){ return m[key] ? m[key]->hnd : -1; }
  int r(string key, SDXSprite *p){ m[key] = p; return p->hnd; }
#if 0
  string knd(string key, int n, int d){
    ostringstream ss;
    ss << key << '_' << hex << setfill('0') << setw(d) << n; // setbase(16)
    return ss.str();
  }
  int hn(string key, int n, int d=16){
    string k = knd(key, n, d);
    return m[k] ? m[k]->hnd : -1;
  }
  int rn(string key, SDXSprite *p, int n, int d=16){
    // cout << knd(key, n, d) << endl;
    m[knd(key, n, d)] = p;
    return p->hnd;
  }
#else
  int hn(string key, int n){
    return ((SDXVector *)m[key])->v[n]->hnd; // no check NULL no check type
  }
  int rn(string key, SDXSprite *p, int n){
    ((SDXVector *)m[key])->v[n] = p; // no check NULL no check type
    return p->hnd;
  }
#endif
  virtual int Dispose(){
    for(pool_t::iterator i = m.begin(); i != m.end(); ++i)
      if(i->second){ delete i->second; i->second = NULL; }
  }
  virtual ~SDXPool(){ Dispose(); }
};

}

#endif // __SPRITEDX_H__

