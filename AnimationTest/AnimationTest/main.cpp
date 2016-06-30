#pragma once

#include "CommonIncludes.h"
#include "BaseGame.h"
#include <vld.h>
#include <atlbase.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR commandLine, int showCMD)
{
	auto FileConverter = make_unique<BaseGame>(hInstance, L"WindowClass", L"File Converter and Display", showCMD);

	//MessageBoxW(NULL, GetCommandLineW(), GetCommandLineW(), MB_OK);
	//return 0;

	//int argCount;
	//LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &argCount);

	//for (int i = 0; i < argCount; i++)
	//{
	//	MessageBox(FileConverter->GetWindowHandle(), szArglist[i], FileConverter->GetWindowTitle().c_str(), MB_OK);
	//	printf("%d: %ws\n", i, szArglist[i]);
	//}

	//CommandLineToArgvW()
	//commandLine = "MEAT.FBX";
	//MessageBox(NULL, (LPCTSTR)(commandLine), TEXT(""), MB_OK);

	// Free memory allocated for CommandLineToArgvW arguments.
//	LocalFree(szArglist);
//	LPSTR* commandLine1 = &commandLine;
	//LPWSTR* commandLineConv = commandLine1;

	//LPCSTR a = commandLine;
	//LPCWSTR w = A2W(a);
	//std::wcout << w << std::endl;

	//MessageBox(FileConverter->GetWindowHandle(), commandLine, FileConverter->GetWindowTitle().c_str(), MB_OK);


	try
	{
		FileConverter->Run();
	}
	catch (Exception ex)
	{
		MessageBox(FileConverter->GetWindowHandle(), ex.whatw().c_str(), FileConverter->GetWindowTitle().c_str(), MB_ABORTRETRYIGNORE);
	}
	return EXIT_SUCCESS;
}