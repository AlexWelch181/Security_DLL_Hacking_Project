// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the SECURITYHACK_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// SECURITYHACK_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SECURITYHACK_EXPORTS
#define SECURITYHACK_API __declspec(dllexport)
#else
#define SECURITYHACK_API __declspec(dllimport)
#endif

// This class is exported from the dll
class SECURITYHACK_API CSecurityHack {
public:
	CSecurityHack(void);
	// TODO: add your methods here.
};

extern SECURITYHACK_API int nSecurityHack;

SECURITYHACK_API int fnSecurityHack(void);
