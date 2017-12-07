/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * Produced by:
 *	Side Effects Software
 *	123 Front St. West, Suite 1401
 *	Toronto, Ontario
 *	Canada   M5J 2M2
 *	416-504-9876
 *
 * NAME:	FBX_SceneSupport.h (FBX Library, C++)
 *
 * COMMENTS:	
 *
 */

#ifndef __FBX_SCENEPROXYSUPPORT_H_INCLUDED__
#define __FBX_SCENEPROXYSUPPORT_H_INCLUDED__

#include <boost/functional/hash.hpp>

#define FBX_SCENEPROXY_DECLARE_ID(TYPE) \
    class TYPE \
    { \
    public: \
	TYPE() : myPtr(nullptr) { } \
	operator bool() const { return myPtr!=nullptr; } \
	bool operator==(const TYPE& x) const { return myPtr == x.myPtr; } \
	bool operator!=(const TYPE& x) const { return !(*this == x); } \
	size_t hash() const { return boost::hash<void*>()(myPtr); } \
    private: \
	TYPE(void *p) : myPtr(p) { } \
	void* myPtr; \
	friend class FBX_SceneProxy; \
    }; \
    inline size_t hash_value(TYPE id) { return id.hash(); } \
    /**/

#endif // __FBX_SCENEPROXYSUPPORT_H_INCLUDED__
