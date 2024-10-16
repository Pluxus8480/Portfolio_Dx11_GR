#pragma once

#include "Base.h"

BEGIN(Engine)

enum class eKeyState
{
	Down,
	Pressed,
	Up,
	None,
};

enum class eKeyCode
{
	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,
	Left, Right, Down, Up,
	Space, LButton, RButton,
	LCtrl, LShift, NUMPAD0,
	NUMPAD1,
	One, Two, Three, Four,
	Enter,
	End,
};

class CKey_Manager : public CBase
{
public:
	struct Key
	{
		eKeyCode eKeyCode;
		eKeyState eState;
		bool bPressed;
	};

	void Initialize();
	void Update();

	bool GetKeyDown(eKeyCode _keyCode) { return vecKey[(UINT)_keyCode].eState == eKeyState::Down; }
	bool GetKeyUp(eKeyCode _keyCode) { return	vecKey[(UINT)_keyCode].eState == eKeyState::Up; }
	bool GetKey(eKeyCode _keyCode) { return	vecKey[(UINT)_keyCode].eState == eKeyState::Pressed; }
	bool GetKeyNone(eKeyCode _keyCode) { return vecKey[(UINT)_keyCode].eState == eKeyState::None; }

private:
	CKey_Manager() {};
	~CKey_Manager() = default;

	void createKeys();
	void updateKeys();
	void updateKey(Key& key);
	void updateKeyDown(Key& key);
	void updateKeyUp(Key& key);
	bool isKeyDown(eKeyCode code);
	void clearKeys();

private:
	vector<Key> vecKey;

public:
	static CKey_Manager* Create();
	void Free() override;
};

END
