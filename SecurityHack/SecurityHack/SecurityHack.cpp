// SecurityHack.cpp : Defines the exported functions for the DLL.
//

#include "framework.h"
#include "SecurityHack.h"


// This is an example of an exported variable
SECURITYHACK_API int nSecurityHack=0;

// This is an example of an exported function.
SECURITYHACK_API int fnSecurityHack(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CSecurityHack::CSecurityHack()
{
    return;
}
