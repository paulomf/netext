/*=========================================================================================================
  Copyright (c) 2013-2015 Rodney Viana
  http://netext.codeplex.com/

  Distributed under GNU General Public License version 2 (GPLv2) (http://www.gnu.org/licenses/gpl-2.0.html)
============================================================================================================*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#include <engextcpp.hpp>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <map>
#include <atlbase.h>
#include <crtdbg.h>
#include <cor.h>
#include <vector>
#include <list>
#include <time.h>
#include <mscoree.h>
#include <CorHdr.h>
#include <corhlpr.h>
#include <tuple>
#include <stack>
#include <string>
#include <regex>
#include <iostream>
#include <sstream>
#include <stdarg.h>





#if _DEBUG
#if _WIN64
#import  "..\ClrMemDiagExt\bin\x64\Debug\NetExtShim.tlb" auto_rename
#else
#import  "..\ClrMemDiagExt\bin\x86\Debug\NetExtShim.tlb" auto_rename
#endif
#else
#if _WIN64
#import  "..\ClrMemDiagExt\bin\x64\Release64\NetExtShim.tlb" auto_rename
#else
#import  "..\ClrMemDiagExt\bin\x86\Release32\NetExtShim.tlb" auto_rename
#endif
#endif

extern int vsnprintf(char * s, size_t n, const char * format, va_list arg );

using namespace std;


#ifdef _WIN64
#define CLRDATA_ADDRESS UINT64
#else
#define CLRDATA_ADDRESS UINT64
#endif

extern ExtExtension* g_ExtInstancePtr;
extern bool wasInterrupted;

#define MAX_STACK 1000
//Alignment constant for allocation
#ifndef _WIN64
#define ALIGNCONST 3
#else
#define ALIGNCONST 7
#endif

typedef std::map<CLRDATA_ADDRESS, std::wstring> MTFieldsMap;
typedef std::vector<CLRDATA_ADDRESS> MTList;

extern	MTFieldsMap typesMap;
extern	MTList mtList;
extern	std::vector<std::string> typeList;

//The large object heap uses a different alignment
#define ALIGNCONSTLARGE 7

extern WDBGEXTS_CLR_DATA_INTERFACE Query;
extern bool isCLRInit;
extern WCHAR NameBuffer[MAX_CLASS_NAME];

extern bool IsValidMemory(CLRDATA_ADDRESS Address, INT64 Size);


//----------------------------------------------------------------------------
//
// Base extension class.
// Extensions derive from the provided ExtExtension class.
//
// The standard class name is "Extension".  It can be
// overridden by providing an alternate definition of
// EXT_CLASS before including engextcpp.hpp.
//
//----------------------------------------------------------------------------

typedef std::map<std::string, CLRDATA_ADDRESS> RegsMap;

extern const char *reg64[];

#define MAX_MTNAME MAX_CLASS_NAME

extern const char *reg32[];
extern WCHAR NameBuffer[MAX_MTNAME];

inline CLRDATA_ADDRESS GetRegAddr(const char* Reg);

// Callback for C# Dll to print out plain text
void CallOutput(wchar_t* Text);

// Callback for C# Dll to print out formatted text
void CallOutputDml(wchar_t* Text);

// Callback for C# Dll to check if interrupt was pressed
long IsManagedInterrupt();

extern bool NET2;
HRESULT INIT_API();


typedef void(*pOutput)(wchar_t *Text);
typedef void(*pSetOutput)(pOutput,pOutput);
typedef long(*pIsInterrupted)();
typedef void(*pSetInterrupt)(pIsInterrupted);

typedef void(*pIMDTarget)(const wchar_t* ProbePath, IDebugClient *Client, NetExtShim::IMDTarget **iTarget);
extern HINSTANCE hDll;
extern pSetOutput setOutputCallBack;
extern pOutput echo;
extern pIMDTarget createFromDebug;
extern IUnknown* clrData;


extern CComPtr<NetExtShim::IMDActivator> pAct;
extern CComPtr<NetExtShim::IMDTarget> pTarget;
extern CComPtr<NetExtShim::IMDRuntime> pRuntime;
extern CComPtr<NetExtShim::IMDRuntimeInfo> pRuntimeInfo;
extern CComPtr<NetExtShim::IMDHeap> pHeap;
typedef std::vector<std::string> columns;
typedef std::vector<columns> table;

std::string tickstotimespan(UINT64 Ticks);
std::wstring tickstodatetime(UINT64 Ticks);
std::wstring formatnumber(UINT64 Number);
std::wstring formatnumber(double Number);
std::string formathex(UINT64 Number);
std::string tickstoCTime(UINT64 Ticks);

struct FromFlags
{
	bool fgac;
	bool fstack;
	bool ftype;
	bool fmt;
	bool fobj;
	bool nofield;
	bool farray;
	bool nospace;
	bool ffieldName;
	bool ffieldtype;
	bool fimplement;
	bool fwithpointer;
	std::vector<std::string> type;
	std::vector<CLRDATA_ADDRESS> mt;
	CLRDATA_ADDRESS obj;
	std::string typeStr;
	std::string mtStr;
	std::string fieldName;
	std::string fieldType;
	std::string implStr;
	std::string cmd;
};

#define EXITPOINTEXT(s) \
		if(hr != S_OK)		\
		{ \
			g_ExtInstancePtr->Out(s);  \
			g_ExtInstancePtr->Out(". Error: %x\n", hr);  \
			return hr; \
		}

#define EXITPOINT(s) \
		if(hr != S_OK)		\
		{ \
			Out(s);  \
			Out(". Error: %x\n", hr);  \
			return; \
		}

#define DO_INIT_API \
	if(FAILED(INIT_API())) return;

class CallCSDll
{
private:

public:
	static void LoadDll();
	static void GetInterface(NetExtShim::IMDTarget **iTarget);
	
	static void Echo(wchar_t* Message);

};

class Thread
{
private:
	vector<NetExtShim::MD_ThreadData> threads;
public:
	typedef vector<NetExtShim::MD_ThreadData>::const_iterator const_iterator;
	Thread()
	{  };
	const UINT32 Size();
	const NetExtShim::MD_ThreadData* operator[](UINT32 i);
	bool Request(bool IsOrdered=false);
	const_iterator GetThreadByAddress(CLRDATA_ADDRESS Address);
	const_iterator Thread::GetThreadInRange(CLRDATA_ADDRESS Begin, CLRDATA_ADDRESS End);
	const_iterator begin();
	const_iterator end();
	void Clear() { threads.clear(); }
	bool IsValid() { return  threads.size() > 0; }
	static ULONG GetOSThreadIDByAddress(CLRDATA_ADDRESS ThreadAddress);

};

extern std::map<std::wstring, std::wstring> varsDict;

#define GetVar(x) envVars.find(x) == envVars.end() ? L"" : envVars[x]

class EXT_CLASS : public ExtExtension
{
public:
	static std::string Execute(const std::string &Command)
	{
		auto_ptr<ExtCaptureOutputA> execContext(new ExtCaptureOutputA());

		execContext->Execute(Command.c_str());
		std::string retStr;
		if(execContext->m_Text) retStr.assign(execContext->m_Text);

		return retStr;
	}

	const char *ExecuteChar(const std::string &Command)
	{
		auto_ptr<ExtCaptureOutputA> execContext(new ExtCaptureOutputA());

		execContext->Execute(Command.c_str());
		return execContext->GetTextNonNull();
	}

	static std::wstring ReadTypedUnicode(CLRDATA_ADDRESS Address)
	{
		if (!IsValidMemory(Address, 8 + g_ExtInstancePtr->m_PtrSize))
		{
			return L"";
		}
		USHORT realSize;
		ExtRemoteData size(Address, sizeof(realSize));


		ExtRemoteData stringPtr(Address + 8, g_ExtInstancePtr->m_PtrSize);
		ZeroMemory(NameBuffer, sizeof(NameBuffer));

		realSize = size.GetUshort();

		if (realSize > (MAX_MTNAME - 2))
			realSize = MAX_MTNAME - 2;

		ULONG64 Addr = stringPtr.GetPtr();
		if (!IsValidMemory(Addr, realSize))
		{
			return L"";
		}

		ExtRemoteData buffer(Addr, realSize);
		buffer.ReadBuffer(NameBuffer, realSize, false);


		std::wstring uniStr((wchar_t*)NameBuffer);

		return uniStr;

	}
	static std::wstring GetProcessName()
	{
		ExtRemoteTyped peb("(ntdll!_PEB*)@$extin", g_ExtInstancePtr->EvalExprU64("@$peb"));
		ExtRemoteTyped processName = peb.Field("ProcessParameters.ImagePathName");
		return ReadTypedUnicode(processName.m_Offset);


	}

	static std::wstring GetProcessCommandLine()
	{
		ExtRemoteTyped peb("(ntdll!_PEB*)@$extin", g_ExtInstancePtr->EvalExprU64("@$peb"));
		ExtRemoteTyped cmdLine = peb.Field("ProcessParameters.CommandLine");
		return ReadTypedUnicode(cmdLine.m_Offset);
	}


	static std::map<std::wstring, std::wstring> GetProcessEnvVar(bool EnableCache = true)
	{
		if (EnableCache && varsDict.size() > 0)
		{
			return varsDict;
		}

		if (!EnableCache)
		{
			varsDict.clear();
		}

		ExtRemoteTyped peb("(ntdll!_PEB*)@$extin", g_ExtInstancePtr->EvalExprU64("@$peb"));
		ExtRemoteTyped envVars = peb.Field("ProcessParameters.Environment");
		ExtRemoteTyped size = peb.Field("ProcessParameters.EnvironmentSize");

		
		if (!size.m_ValidData)
		{
			return varsDict;
		}
		UINT64 totalSize = size.GetLong64();


		ExtRemoteData data(envVars.GetPtr(), totalSize);
		if (!IsValidMemory(data.m_Offset, totalSize))
		{
			return varsDict;
		}
		byte *buffer = (byte*)calloc(totalSize + 4, sizeof(byte));
		data.ReadBuffer(buffer, totalSize, true);
		
		for (wchar_t* strArray = (wchar_t*)buffer; L'\0' != *strArray; strArray += lstrlenW(strArray) + 1)
		{
			std::wstring str(strArray);
			if (str.find_first_of(L"=") != std::wstring::npos)
				varsDict.emplace(str.substr(0, str.find_first_of(L"=")), str.substr(str.find_first_of(L"=") + 1));
		}

		free(buffer);

		return varsDict;
	}

	static std::wstring GetProcessAccount()
	{
		auto envVars = GetProcessEnvVar();
		auto userName = GetVar(L"USERNAME");
		auto domainName = GetVar(L"USERDOMAIN");
		auto computerName = GetVar(L"COMPUTERNAME");

		if (domainName.size() == 0)
		{
			domainName = computerName;
		}
		
		if (domainName.size() != 0)
		{
			domainName.append(L"\\");
		}

		return domainName + userName;
	}

	static Thread SessionThreads;
	void Uninitialize();
	EXT_COMMAND_METHOD(wver);
	EXT_COMMAND_METHOD(wstack);
	EXT_COMMAND_METHOD(wclrstack);
	EXT_COMMAND_METHOD(wdo);
	EXT_COMMAND_METHOD(wselect);
	EXT_COMMAND_METHOD(wheap);
	EXT_COMMAND_METHOD(windex);
	EXT_COMMAND_METHOD(regmatch);
	EXT_COMMAND_METHOD(regsearch);
	EXT_COMMAND_METHOD(weval);
	EXT_COMMAND_METHOD(wfrom);
	EXT_COMMAND_METHOD(wdict);
	EXT_COMMAND_METHOD(whash);
	EXT_COMMAND_METHOD(whttp);
	EXT_COMMAND_METHOD(wcookie);
	EXT_COMMAND_METHOD(wconfig);
	EXT_COMMAND_METHOD(wkeyvalue);
	EXT_COMMAND_METHOD(wclass);
	EXT_COMMAND_METHOD(wservice);
	EXT_COMMAND_METHOD(whelp);
	EXT_COMMAND_METHOD(wtoken);
	EXT_COMMAND_METHOD(wruntime);
	EXT_COMMAND_METHOD(wopendownloadpage);
	EXT_COMMAND_METHOD(wupdate);
	EXT_COMMAND_METHOD(wgchandle);
	EXT_COMMAND_METHOD(wthreads);
	EXT_COMMAND_METHOD(wdae);
	EXT_COMMAND_METHOD(wpe);
	EXT_COMMAND_METHOD(wdomain);
	EXT_COMMAND_METHOD(wsocket);
	EXT_COMMAND_METHOD(wxml);
	EXT_COMMAND_METHOD(wtime);
	EXT_COMMAND_METHOD(wmodule);
	EXT_COMMAND_METHOD(wmakesource);
	EXT_COMMAND_METHOD(wapppool);
	EXT_COMMAND_METHOD(wsql);
	EXT_COMMAND_METHOD(wvar);

	EXT_COMMAND_METHOD(wconcurrentdict);

	EXT_COMMAND_METHOD(wk);
	EXT_COMMAND_METHOD(wopensource);

	regex_constants::syntax_option_type GetFlavor(const string& flavor);
	std::ostringstream regexmatch(const string& Target, const string& Pattern, bool CaseSensitive, const string& Flavor, bool Run, const string& Format);
	std::ostringstream EXT_CLASS::regexsearch(const string& Target, const string& Pattern, bool Not, bool CaseSensitive, const string& Flavor);
	void wfrom_internal(FromFlags flags);
	void HashInternal(CLRDATA_ADDRESS Addr);
	void OnSessionAccessible(ULONG64 Argument);



};

