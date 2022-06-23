#include "..\public\Input_Device.h"

IMPLEMENT_SINGLETON(Input_Device)

Input_Device::Input_Device()
{
}

HRESULT Input_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;
	
	/* 키보드 초기화. */
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE/* | DISCL_NOWINKEY*/)))
		return E_FAIL;

	if (FAILED(m_pKeyBoard->Acquire()))
		return E_FAIL;

	/* 마우스 초기화. */
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE/* | DISCL_NOWINKEY*/)))
		return E_FAIL;

	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

HRESULT Input_Device::SetUp_InputDeviceState()
{
	if (nullptr == m_pKeyBoard ||
		nullptr == m_pMouse)
		return E_FAIL;

	for (int i = 0; i < 255; i++)
	{
	//	if ((m_KeyBoardState[i] & 0x8000) && (GetAsyncKeyState(i) & 0x0000))
			//m_KeyBoardState[i] = 0x0000;

		if (((m_eKeyState[i] == KeyState::DOWN) || (m_eKeyState[i] == KeyState::PRESS)) 
				&& !(GetAsyncKeyState(i) & 0x8000))
			m_eKeyState[i] = UP;
		else if ((m_eKeyState[i] == DOWN) && (GetAsyncKeyState(i) & 0x8000))
			m_eKeyState[i] = PRESS;
		
	}


	if (FAILED(m_pKeyBoard->GetDeviceState(256, m_KeyBoardState)))
		return E_FAIL;

	if (FAILED(m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState)))
		return E_FAIL;

	return S_OK;
}

_bool Engine::Input_Device::GetKeyDown(_ubyte key)
{
	//이미 눌려있던상태 
	if (m_eKeyState[key] == DOWN || m_eKeyState[key] == PRESS)
	{
		return false;
	}

	//새로 누른상태 
	if (GetAsyncKeyState(key) & 0x8000)
	{
		m_eKeyState[key] = DOWN;
			return true;
	}

	m_eKeyState[key] = UP;

	return false;
}

void Input_Device::Free()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInputSDK);
}
