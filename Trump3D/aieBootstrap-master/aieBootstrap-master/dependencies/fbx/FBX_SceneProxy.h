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
 * NAME:	FBX_SceneProxy.h (FBX Library, C++)
 *
 * COMMENTS:	
 *
 */

#ifndef __FBX_SCENEPROXY_H_INCLUDED__
#define __FBX_SCENEPROXY_H_INCLUDED__

#include "FBX_API.h"
#include "FBX_SceneProxySupport.h"
#include <UT/UT_ScopedPtr.h>
#include <UT/UT_StringArray.h>
#include <SYS/SYS_Types.h>


class FBX_ErrorManager;
class GU_Detail;

FBX_SCENEPROXY_DECLARE_ID(FBX_NodeID);
FBX_SCENEPROXY_DECLARE_ID(FBX_GeometryID);
FBX_SCENEPROXY_DECLARE_ID(FBX_ClipID);

class FBX_API FBX_SceneProxy
{
public:
		    FBX_SceneProxy();
		    ~FBX_SceneProxy();

    bool	    load(const char* path, bool convert_units,
                         FBX_ErrorManager& errors);

    fpreal	    globalFrameRate() const;
    void	    getGlobalTimeRange(fpreal& start, fpreal &stop) const;

    const char*	    nodeName(FBX_NodeID node) const;
    FBX_NodeID	    findNode(const char* name) const;

    int		    nodeCount() const;
    FBX_NodeID	    node(int i) const;

    FBX_NodeID	    rootNode() const;
    FBX_NodeID	    parentNode(FBX_NodeID node) const;
    int		    childNodeCount(FBX_NodeID node) const;
    FBX_NodeID	    childNode(FBX_NodeID node, int i) const;

    enum NodeType
    {   
        eUnknown,	    // Unknown
        eNull,		    // Null object
        eMarker,	    // Mocap marker OR IK/FK effector
        eSkeleton,	    // Bone/Joint
        eMesh,		    // Geometry
        eNurbs,		    // Geometry
        ePatch,		    // Geometry
        eCamera,	    // Camera
        eCameraStereo,	    // Stereo camera
        eCameraSwitcher,    // Switch between child cameras
        eLight,		    // Light
        eOpticalReference,  // Mocap marker
        eOpticalMarker,	    // Mocap marker OR IK/FK effector
        eNurbsCurve,	    // Geometry
        eTrimNurbsSurface,  // Geometry
        eBoundary,	    // Geometry
        eNurbsSurface,	    // Geometry
        eShape,		    // Blendshape
        eLODGroup,	    // Group of objects with different levels of detail
        eSubDiv,	    // Geometry
        eCachedEffect,	    // Particles, fluids, hair, etc. cache
        eLine		    // Geometry
    };
    NodeType	    nodeType(FBX_NodeID node) const;

    bool	    visibility(FBX_NodeID node) const;

    FBX_GeometryID  geometry(FBX_NodeID node) const;
    int		    geometrySkinDeformerCount(FBX_GeometryID geometry) const;
    bool	    convertGeometry(
			GU_Detail& detail,
			FBX_GeometryID geometry,
			FBX_ErrorManager& errors) const;

    UT_Matrix4D	    nodeGlobalTransform(
			FBX_NodeID node, fpreal time_seconds) const;

    int		    clipCount() const;
    FBX_ClipID	    clip(int i) const;
    const char*	    clipName(int i) const;
    void	    setActiveClip(FBX_ClipID clip);

    // Returns the *inclusive* time range for the given clip
    void	    getClipTimeRange(
			FBX_ClipID clip, fpreal& start, fpreal &stop) const;


private:
    template <typename FBX_T, typename ID_T>
    static inline FBX_T * castID(ID_T id);

private:
    struct Impl;
    UT_ScopedPtr<Impl> myImpl;
};

#endif // __FBX_SCENEPROXY_H_INCLUDED__
