#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Windows.h>
#include <iostream>
#include "UI.h"

typedef int(__stdcall* _SayFunc)(const char* msg);
typedef int(__cdecl* _SkillFunc)(int a1, char a2);
typedef int(__cdecl* _CallToSendFunc)(int a1, int a2);
typedef int(__cdecl* _DoUseItemFunc)(int a1, int a2, int a3);

class Controller
{
private:
	HMODULE hModule;
	uintptr_t moduleBase;

	WNDCLASSEX wc;
	HWND hwnd;

	_SayFunc SayFunc;
	_SkillFunc SkillFunc;
	_CallToSendFunc CallToSendFunc;
	_DoUseItemFunc DoUseItemFunc;

	FILE* f;

	const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	ImGuiIO io;

	void InitUI();
	void Render();
	void Close();
	void ClearUI();
public:
	Controller(HMODULE hModule);
	DWORD WINAPI Logic();
};

DWORD WINAPI MainThread(HMODULE hModule);

#endif