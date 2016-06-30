#pragma once
#include <Windows.h>
#include <exception>
#include <string>

class Exception : public std::exception
{
public:
	Exception(const char* const& message, HRESULT hr = S_OK);
	HRESULT HR() const;
	std::wstring whatw() const;

private:
	HRESULT mHR;
};