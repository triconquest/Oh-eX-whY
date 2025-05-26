#pragma once

#include <Windows.h>

namespace CInterface
{
	template<typename T>
	T* GetInterface(const char* moduleName, const char* lib)
	{
		const auto handle = GetModuleHandle(lib);

		if (!handle)
			return nullptr;

		const auto functionAddr = GetProcAddress(handle, "CreateInterface");

		if (!functionAddr)
			return nullptr;

		using Fn = T * (*)(const char*, int*);

		const auto CreateInterface = reinterpret_cast<Fn>(functionAddr);

		return CreateInterface(moduleName, nullptr);
	}
}