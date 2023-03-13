#include "input.h"

Input::Input(HINSTANCE hinst, HWND hwnd, int screenW, int screenH) {
  HRESULT hr = S_OK;
  m_screenW = screenW;
  m_screenH = screenH;

  m_directInput = nullptr;
  m_kb = nullptr;
  m_mouse = nullptr;

  hr = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
  
  if (SUCCEEDED(hr)) {
    hr = m_directInput->CreateDevice(GUID_SysKeyboard, &m_kb, NULL);
  }

  if (SUCCEEDED(hr)) {
    hr = m_kb->SetDataFormat(&c_dfDIKeyboard);
  }

  if (SUCCEEDED(hr)) {
    hr = m_kb->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
  }

  if (SUCCEEDED(hr)) {
    hr = m_kb->Acquire();
  }

  if (SUCCEEDED(hr)) {
    hr = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
  }

  if (SUCCEEDED(hr)) {
    hr = m_mouse->SetDataFormat(&c_dfDIMouse);
  }

  if (SUCCEEDED(hr)) {
    hr = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
  }

  if (SUCCEEDED(hr)) {
    hr = m_mouse->Acquire();
  }
}

Input::~Input() {
  if (m_mouse) {
    m_mouse->Unacquire();
    m_mouse->Release();
  }
  if (m_kb) {
    m_kb->Unacquire();
    m_kb->Release();
  }
  if (m_directInput) {
    m_directInput->Release();
  }
}

bool Input::readKB() {
  HRESULT kb = m_kb->GetDeviceState(sizeof(m_kbState), (LPVOID)&m_kbState);
  if (FAILED(kb)) {
    if((kb == DIERR_INPUTLOST) || (kb == DIERR_NOTACQUIRED))
      m_kb->Acquire();
    else
      return false;
  }
  return true;
}

bool Input::readMouse() {
  HRESULT mouse = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
  if (FAILED(mouse)) {
    if((mouse == DIERR_INPUTLOST) || (mouse == DIERR_NOTACQUIRED))
      m_mouse->Acquire();
    else
      return false;
  }
  return true;
}

bool Input::getState() {
  return readKB() && readMouse();
}

XMFLOAT3 Input::getMouseState() {
  if (m_mouseState.rgbButtons[0] || m_mouseState.rgbButtons[1] || m_mouseState.rgbButtons[2] && 0x80) {
    return XMFLOAT3(
        (float)m_mouseState.lX,
        (float)m_mouseState.lY,
        (float)m_mouseState.lZ);
  }
  return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Input::resize(int screenW, int screenH) {
  m_screenH = screenH;
  m_screenW = screenW;
}