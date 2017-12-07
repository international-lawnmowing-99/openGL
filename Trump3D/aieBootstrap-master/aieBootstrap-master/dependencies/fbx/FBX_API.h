
#ifndef __FBX_API_h__
#define __FBX_API_h__

#include <SYS/SYS_Visibility.h>

#ifdef FBX_EXPORTS
#define FBX_API SYS_VISIBILITY_EXPORT
#else
#define FBX_API SYS_VISIBILITY_IMPORT
#endif

#endif
