#include "Exception.h"

Exception::Exception(const char* const& message, HRESULT hr) : exception(message), mHR(hr)
{ }

HRESULT Exception::HR() const
{
	return mHR;
}

std::wstring Exception::whatw() const
{
	std::string whatString(what());
	std::wstring whatw;
	whatw.assign(whatString.begin(), whatString.end());

	return whatw;
}