#include "Input.hpp"

Input::EKeyState Input::keyStates[Input::kMaxKeyCodes];

void Input::init() {
	for (int i = 0; i < kMaxKeyCodes; ++i)
		keyStates[i] = kNotPressed;
}

void Input::KeyDownEvent(EKeyCode Key) {
	if (keyStates[Key] == kNotPressed)
		keyStates[Key] = kPressed;
	else
		keyStates[Key] = kHeld;
}

void Input::KeyUpEvent(EKeyCode Key) {
	keyStates[Key] = kNotPressed;
}

bool Input::KeyHit(EKeyCode eKeyCode) {
	if (keyStates[eKeyCode] == kPressed) {
		keyStates[eKeyCode] = kHeld;
		return true;
	}

	return false;
}

bool Input::KeyHeld(EKeyCode eKeyCode) {
	if (keyStates[eKeyCode] == kNotPressed)
		return false;

	keyStates[eKeyCode] = kHeld;

	return true;
}
