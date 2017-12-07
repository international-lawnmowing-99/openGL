/*
* PROPRIETARY INFORMATION.  This software is proprietary to
* Side Effects Software Inc., and is not to be reproduced,
* transmitted, or disclosed in any way without written permission.
*
* Produced by:
*	Oleg Samus
*	Side Effects
*	123 Front St. West, Suite 1401
*	Toronto, Ontario
*	Canada   M5J 2M2
*	416-504-9876
*
* NAME:	FBX_ErrorManager.h (FBX Library, C++)
*
* COMMENTS:	
*
*/

#ifndef __FBX_ErrorManager_h__
#define __FBX_ErrorManager_h__

#include "FBX_Common.h"
#include <UT/UT_String.h>


/********************************************************************************************************/
enum FBX_ErrorType
{
    FBX_ErrorGeneric = 0,
    FBX_ErrorIncorrectPassword,
    FBX_ErrorLights
};
/********************************************************************************************************/
class FBX_API FBX_Error
{
public:
    FBX_Error(const char* pMessage, bool bIsCritical, FBX_ErrorType eType);
    virtual ~FBX_Error();

    bool getIsCritical(void) const;
    const char* getMessage(void) const;
    FBX_ErrorType getType(void) const;

private:
    std::string myMessage;
    bool myIsCritical;
    FBX_ErrorType myType;
};
typedef std::vector<FBX_Error*> TErrorVector;
/********************************************************************************************************/
class FBX_API FBX_ErrorManager
{
public:
    FBX_ErrorManager();
    virtual ~FBX_ErrorManager();

    void addError(const char* pcsError, bool bIsCritical = true, FBX_ErrorType eType = FBX_ErrorGeneric);
    void addError(const char* pcsErrorPart1, const char* pcsErrorPart2, const char* pcsErrorPart3, bool bIsCritical = true, FBX_ErrorType eType = FBX_ErrorGeneric);

    int getNumItems(void) const;

    bool getDidReportCriticalErrors(void) const;

    void reset(void);

    void appendAllErrors(UT_String& string_out) const;
    void appendAllWarnings(UT_String& string_out) const;

private:

    TErrorVector myErrors;
    bool myDidReportCricialErrors;
};
/********************************************************************************************************/

#endif // __FBX_ErrorManager_h__

