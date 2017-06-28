#include "JetDxfDef.h"


static int DXF_ERROR_COUNT = 0;
static int DXF_DEBUG_ERROR_MESSAGE_OPTION = 1;
#define MAX_MSG_LENGTH 2048
static char sMessage[MAX_MSG_LENGTH];


static bool __cdecl DXF_FormatMessage(const char*format, va_list args)
{
	// appends formatted message to sMessage[]
	int len = ((int)strlen(sMessage));
	if (len < 0)
		return false;
	if (MAX_MSG_LENGTH - 1 - len < 2)
		return false;
	sMessage[MAX_MSG_LENGTH - 1] = 0;
	_vsnprintf_s(sMessage + len, MAX_MSG_LENGTH - 1 - len, MAX_MSG_LENGTH, format, args);
	return true;
}

void __cdecl DXF_ErrorMessage(
	int message_type, // 0=warning - serious problem that code is designed to handle
	// 1=error - serious problem code will attempt to handle The thing causing the error is a bug that must be fixed.
	// 2=assert failed - crash is nearly certain
	const char* sErrorMessage
	)
{
	// error/warning/assert message is in sMessage[] buffer.  Modify this function
	// to do whatever you want to with the message.
	if (sErrorMessage && sErrorMessage[0])
	{

#if defined(ON_PURIFY_BUILD) && defined(ON_32BIT_POINTER)
		// 10 December 2003 Dale Lear
		//     Make ON_ERROR/ON_WARNING messages show up in Purify
		PurifyPrintf("%s", sErrorMessage);
#endif
		//::OutputDebugStringA("\n");
		//::OutputDebugStringA(sErrorMessage);
		//::OutputDebugStringA("\n");
	}
}


void __cdecl DXF_Error(const char* sFileName, int line_number,
	const char* sFormat, ...)
{
	DXF_ERROR_COUNT++; // <- Good location for a debugger breakpoint.

	if (DXF_DEBUG_ERROR_MESSAGE_OPTION)
	{
		bool bPrintMessage = false;
		sMessage[0] = 0;
		if (DXF_ERROR_COUNT < 50)
		{
			// put file and line number info for debug mode
			sprintf_s(sMessage, "openNURBS ERROR # %d %s:%d ", DXF_ERROR_COUNT, sFileName, line_number);
			bPrintMessage = true;
		}
		else if (50 == DXF_ERROR_COUNT)
		{
			sprintf_s(sMessage, "openNURBS ERROR # %d - Too many errors.  No more printed messages.", DXF_ERROR_COUNT);
			bPrintMessage = true;
		}

		if (bPrintMessage)
		{
			if (sFormat && sFormat[0])
			{
				// append formatted error message to sMessage[]
				va_list args;
				va_start(args, sFormat);
				bPrintMessage = DXF_FormatMessage(sFormat, args);
				va_end(args);
			}
			if (bPrintMessage)
				DXF_ErrorMessage(1, sMessage);
		}
	}
}