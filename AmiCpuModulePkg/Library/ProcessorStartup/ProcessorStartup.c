#include <Token.h>
#include <PiPei.h>
#include "ProcessorStartupAutoGen.h"

VOID SecElinkCalls()
{
    int i;
    for (i = 0; SecElinkList[i] != NULL; ++i) {
        SecElinkList[i]();
    }
}
