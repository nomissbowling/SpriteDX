/*
  test_SpriteDX.cpp

  DxLib
  veiled functions https://densanken.com/wiki/index.php?dx%A5%E9%A5%A4%A5%D6%A5%E9%A5%EA%B1%A3%A4%B7%B4%D8%BF%F4%A4%CE%A5%DA%A1%BC%A5%B8
  https://dxlib.xsrv.jp/dxfunc.html

  64 bit OK (sequence sensitive)
  g++ -o dist/test_SpriteDX_64.exe src/test_SpriteDX.cpp -DDX_GCC_COMPILE -DDX_NON_INLINE_ASM -I ./inc -I ./dist/x86_64 -L ./dist/x86_64 -lDxLib -lDxUseCLib -lDxDrawFunc -ljpeg -lpng -lzlib -ltiff -ltheora_static -lvorbis_static -lvorbisfile_static -logg_static -lbulletdynamics -lbulletcollision -lbulletmath -lopusfile -lopus -lsilk_common -lcelt
*/

#include <SpriteDX.hpp>
using namespace SpriteDX;
//#pragma warning(disable:4244)
//#define _CRT_SECURE_NO_WARNINGS
//#include <DxLib.h>
#include <math.h>

char keybuf[256]; // must be char[256]
int cols[][4] = {
  { 32,  32,  32, 255}, // black
  {240,  32,  32, 255}, // red
  { 32, 240,  32, 255}, // green
  {240, 240,  32, 255}, // yellow
  { 32,  32, 240, 255}, // blue
  {240,  32, 240, 255}, // magenta
  { 32, 240, 240, 255}, // cyan
  {240, 128,  32, 255}}; // orange
int bgc = GetColor(0, 0, 0);
int fgc = GetColor(240, 240, 240);
#define PNG_BKG "res/test_Bkg.png" // 380 x 460
#define PNG_IMAGE "res/test_Image.png"
#define PNG_MASK "res/test_Mask.png" // white 10,10-25,25
char fn[] = "res/panda_rot_N_120x136.png"; // 4 3 2 1 0 (N replaced later)
int frame_max = 60, frame = 0, cnt = 0;

int draw(SDXMap &pool)
{
  int hgr = pool["gr"]; // gr Graph test
  DrawGraph(0, 0, hgr, TRUE);
  DrawGraph(380, 0, hgr, TRUE);
  DrawGraph(760, 0, hgr, TRUE);
  DrawGraph(0, 460, hgr, TRUE);
  DrawGraph(380, 460, hgr, TRUE);
  DrawGraph(760, 460, hgr, TRUE);

  // primitive test
  DrawString(0, 0, "Hit any key", fgc); // may be (8+1) x 16
  DrawPixel(320, 240, 0xffff);
  DrawPixel(340, 240, 0xffff);
  DrawPixel(360, 240, 0xffff);
  DrawCircle(320, 280, 4, GetColor(255, 0, 0), TRUE);
  DrawCircle(340, 280, 4, GetColor(0, 255, 0), FALSE);
  DrawCircle(360, 280, 4, GetColor(0, 0, 255), TRUE);

  int hbk = pool["bk"]; // bk SoftImage (draw on memory) test
  FillSoftImage(hbk, 255, 255, 255, 255); // RGBA
  for(int i = 0; i <= 6; ++i){
    int x = 32 + (int)(cos(i + frame * 6.28f / frame_max) * 24);
    int y = 32 - (int)(sin(i + frame * 6.28f / frame_max) * 24);
    int *c = cols[7 - i];
    for(int j = -7; j <= 7; ++j){
      for(int k = -7; k <= 7; ++k){
        DrawPixelSoftImage(hbk, x+k, y+j, c[0], c[1], c[2], c[3]); // RGBA
      }
    }
  }
  DrawSoftImage(20, 20, hbk);

  int him = pool["im"]; // im SoftImage test
  DrawSoftImage(400, 0, him);

  int hmg = pool["mg"]; // mg Graph test
  GetDrawScreenGraph(412, 24, 412 + 64, 24 + 64, hmg);
  DrawGraph(400, 300, hmg, TRUE);

  int hmh = pool["mh"], hmsk = pool["msk"];
  // (MaskScreen) mh (file) msk (append draw on memory) Mask test
  CreateMaskScreen(); // must be in the draw loop ?
  FillMaskScreen(0); // 0: overdraw 1: through screen (fill 0 initialized)
  // SetUseMaskScreenFlag(TRUE);
  DrawMask(0, 0, hmh, DX_MASKTRANS_NONE); // _NONE _BLACK _WHITE
  unsigned char buf[16][32]; // 32 x 16
  for(int i = 0; i < 16; ++i){
    for(int j = 0; j <32; ++j){
      buf[i][j] = (j & 0x01) ? 0 : 0xff;
    }
  }
  // DrawMaskToDirectData(0, 16, 32, 16, buf, DX_MASKTRANS_NONE); // slow
  SetDataToMask(32, 16, buf, hmsk); // fast on repeat
  DrawMask(0, 16, hmsk, DX_MASKTRANS_NONE);
  DrawBox(0, 0, 32, 32, GetColor(128, 128, 0), TRUE); // overdraw primitive
  DeleteMaskScreen(); // must be in the draw loop ?

  int hsim = pool["sim"];
  // sim SoftImage (GetDrawScreenSoftImage and GetImageAddressSoftImage) test
  GetDrawScreenSoftImage(0, 0, 64, 64, hsim); // std. exists but undocumented
  if(FALSE && cnt == 1){
    int r, g, b, a;
    for(int i = 0; i < 16; ++i){
      for(int j = 0; j < 16; ++j){
        GetPixelSoftImage(hsim, j, i, &r, &g, &b, &a);
        fprintf(stdout, " %02x%02x%02x%02x", r, g, b, a);
      }
      fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
  }
  unsigned char *p = (unsigned char *)GetImageAddressSoftImage(hsim); // BGRA
  if(FALSE && cnt == 1){
    for(int i = 0; i < 16; ++i){
      for(int j = 0; j < 16; ++j){
        BGRA &o = *(BGRA *)(p + i * 256 + j * 4); // 64 * 4
        fprintf(stdout, " %02x%02x%02x%02x", o.r, o.g, o.b, o.a); // order
        // fprintf(stdout, " %08x", o); // ARGB *NOT* same as above RGBA
      }
      fprintf(stdout, "\n");
    }
    fprintf(stdout, "\n");
  }

  // nim SoftImage test (without LoadDivGraph CreateDivGraphFromSoftImage)
  for(int i = 0; i < 5; ++i)
    DrawSoftImage(600 - i * 140, 400, pool["nim"][i]);
  if(keybuf[KEY_INPUT_UP]){
    if(cnt++ % 6 == 0){
      if(frame + 1 == frame_max) frame = 0;
      else ++frame;
    }
  }
  DrawSoftImage(600 - (frame % 10) * 60, 200, pool["nim"][frame % 5]);
}

int main(int ac, char **av)
{
  ChangeWindowMode(TRUE); // FALSE: Full Screen
  SetGraphMode(800, 600, 32); // before init
  if(DxLib_Init() == -1) return -1;

  SDXMap pool; // auto Disposed *** after DxLib_End ***
  pool[SDXKey("gr")] = new SDXGraph(PNG_BKG);
  pool[SDXKey("bk")] = new SDXImage(64, 64);
  pool[SDXKey("im")] = new SDXImage(PNG_IMAGE);
  pool[SDXKey("mg")] = new SDXGraph(64, 64);
  pool[SDXKey("mh")] = new SDXMask(PNG_MASK);
  pool[SDXKey("msk")] = new SDXMask(32, 16);
  pool[SDXKey("sim")] = new SDXImage(64, 64);
  pool[SDXKey("nim")] = new SDXVector(5);
  for(int i = 0; i < 5; ++i){
    fn[14] = '0' + i;
    pool["nim"][SDXIdx(i)] = new SDXImage(fn);
  }
  SetBackgroundColor(0, 0, 0);
  while(!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()
  && !GetHitKeyStateAll(keybuf)){
    draw(pool);
    if(keybuf[KEY_INPUT_ESCAPE]) break;
    if(keybuf[KEY_INPUT_Q]) break;
  }

  DxLib_End();
  return 0;
}
