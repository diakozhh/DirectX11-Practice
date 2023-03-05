#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <directxmath.h>
#include <windows.h>

using namespace DirectX;

class Input {
private:
  bool readKB();
  bool readMouse();

  IDirectInput8* m_directInput;
  IDirectInputDevice8* m_kb;
  IDirectInputDevice8* m_mouse;

  unsigned char m_kbState[256];
  DIMOUSESTATE m_mouseState = {};

  int m_screenW = 0, m_screenH = 0;

public:
  Input(HINSTANCE hinst, HWND hwnd, int screenW, int screenH);
  ~Input();
  bool getState();
  XMFLOAT3 getMouseState();
  void resize(int screenW, int screenH);
};