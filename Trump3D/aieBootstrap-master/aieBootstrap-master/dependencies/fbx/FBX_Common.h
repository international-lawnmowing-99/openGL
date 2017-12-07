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
* NAME:	FBX_Common.h (FBX Library, C++)
*
* COMMENTS:	
*
*/

#ifndef __FBX_Common_h__
#define __FBX_Common_h__

#include "FBX_API.h"
#include <UT/UT_Vector4.h>

#include <string>
#include <vector>
#include <map>
#include <set>


/********************************************************************************************************/
typedef std::vector<std::string> TStringVector;

enum FBX_TextureCoordinatesNamingType
{
    FBX_TextureCoordinatesNamingHoudini = 0,
    FBX_TextureCoordinatesNamingFBX
};
enum FBX_MaterialImportType
{
    FBX_MaterialImportAsFBXShaders = 0,
    FBX_MaterialImportAsVOPNetworks
};
enum FBX_CompatibilityModeType
{
    FBX_CompatibilityModeStandard = 0,
    FBX_CompatibilityModeMaya
};
/********************************************************************************************************/
class FBX_API FBX_ImportOptions
{
public:

    FBX_ImportOptions();

    /// If true, triangulates all patches and imports them as meshes.
    bool getTriangulatePatches() const;
    /// If true, triangulates all NURBs objects and imports them as meshes.
    bool getTriangulateNurbs() const;
    /// If true, animation will be resample into a linearly interpolated
    /// keys at every integer key frame.
    bool getResampleAnimation() const;
    /// Animation resampling interval, in frames, between each sample. Defaults to 1.0.
    double getResampleInterval() const;
    /// If true, forces the normalization of vertex weights on import.
    bool getForceNormalizeVertexWeights() const;
    /// This is a workaround around a bug in FBX SDK. If a vertex cache is created
    /// as a float type, the SDK cannot recognize any difference between that and
    /// the double type. If this option is true, the importer will assume any
    /// vertex caches are actually float type caches. If not, it assumes they are
    /// double. False by default.
    bool getTreatVertexCachesAsFloats() const;
    ///  If true, objects with Blend deforms on them will be imported as Blend SOPs. If false,
    /// they'll be imported as channels.
    bool getImportBlendingAsSOPs() const;
    /// If true, no nodes will be generated for the "Segment Scale Compensate" feature in Maya.
    /// Maya's FBX plugin changed its behaviour between the 2009 and 2010 versions to bake
    /// out the segment scale compensation explicitly, so for newer files, we shouldn't
    /// do it ourselves.  If we try to do it, we'll end up only compensating for the compensation,
    /// and thus undoing it. (Bug #57022)
    bool getIgnoreSegScaleComp() const;
    /// Determines whether uv coordinates are named Houdini-style or with native FBX names.
    FBX_TextureCoordinatesNamingType getUVNamingStyle() const;
    /// If true, and if importing UVs with original FBX names, this
    /// will create one set of UVs with standard Houdini names for OGL display.
    bool getCreateOGLDummyCoords() const;
    /// This will try to adjust the importer to specific file sources (Maya, Max, etc.) and
    /// their specific bugs.
    FBX_CompatibilityModeType getCompatibilityMode() const;
    /// Determines whether materials are imported as VOP Networks or as FBX Shaders.
    FBX_MaterialImportType getMaterialImportMode() const;
    /// The framerate to use to overwrite FBX's own rate. Defaults to -1 (meaning no override).
    double getOverrideFramerate() const;
    /// If true, joints attached to skins will have their display disabled.
    /// @{
    void setHideSkinJoints(bool value);
    bool getHideSkinJoints() const;
    /// @}
    /// If true, joints will have their rotation order converted to ZYX 
    /// @{
    void setConvertJointsToZYXRotOrder(bool value);
    bool getConvertJointsToZYXRotOrder() const;
    /// @}
    /// If true, null nodes will be created as subnets (except for joints). The children will then be
    /// placed within these subnets, instead of parented to them.
    bool getCreateNullsAsSubnets() const;
    /// If true, null nodes will be created as subnets (except for joints). The children will then be
    /// placed within these subnets, instead of parented to them.
    void setCreateNullsAsSubnets(bool value);
    /// If true, all geometry File SOPs will be left unlocked, which means on load, an original
    /// FBX file will be required (at the same location).
    void setUnlockGeometry(bool value);
    /// If true, all deformation File CHOPs will be left unlocked, which means on load, an original
    /// FBX file will be required (at the same location).
    void setUnlockDeformations(bool value);
    /// If true, all geometry File SOPs will be left unlocked, which means on load, an original
    /// FBX file will be required (at the same location).
    bool getUnlockGeometry() const;
    /// If true, all deformation File CHOPs will be left unlocked, which means on load, an original
    /// FBX file will be required (at the same location).
    bool getUnlockDeformations() const;
    /// If true, the importer will attempt to make all paths, including textures and .fbx files, relative
    /// (starting with $HIP). If false, paths will be absolute. The trick is that if paths are relative, the
    /// user *must* save the .hip file into the same directory as the .fbx file, otherwise all references
    /// will be lost.
    bool getMakePathsRelative() const;
    /// If true, objects are imported directly into /obj network, instead of a new subnet for each FBX
    /// file.
    bool getImportIntoObj() const;
    /// If true, the FBX scene will be converted to Y-up on import. If false, the scene will be imported
    /// as-is. True by default.
    bool getConvertToYUp() const;

    /// If true, the FBX scene will be converted to Y-up on import. If false, the scene will be imported
    /// as-is. True by default.
    void setConvertToYUp(bool value);
    /// If true, objects are imported directly into /obj network, instead of a new subnet for each FBX
    /// file.
    void setImportIntoObj(bool value);
    /// If true, the importer will attempt to make all paths, including textures and .fbx files, relative
    /// (starting with $HIP). If false, paths will be absolute. The trick is that if paths are relative, the
    /// user *must* save the .hip file into the same directory as the .fbx file, otherwise all references
    /// will be lost.
    void setMakePathsRelative(bool value);
    /// The framerate to use to overwrite FBX's own rate. Defaults to -1 (meaning no override).
    void setOverrideFramerate(double frate);
    /// Determines whether materials are imported as VOP Networks or as FBX Shaders.
    void setMaterialImportMode(FBX_MaterialImportType value);
    /// This will try to adjust the importer to specific file sources (Maya, Max, etc.) and
    /// their specific bugs.
    void setCompatibilityMode(FBX_CompatibilityModeType value);
    /// If true, and if importing UVs with original FBX names, this
    /// will create one set of UVs with standard Houdini names for OGL display.
    void setCreateOGLDummyCoords(bool value);
    /// Determines whether uv coordinates are named Houdini-style or with native FBX names.
    void setUVNamingStyle(FBX_TextureCoordinatesNamingType uv_style);
    ///  If true, objects with Blend deforms on them will be imported as Blend SOPs. If false,
    /// they'll be imported as channels.
    void setImportBlendingAsSOPs(bool value);
    /// If true, no nodes will be generated for the "Segment Scale Compensate" feature in Maya.
    /// Maya's FBX plugin changed its behaviour between the 2009 and 2010 versions to bake
    /// out the segment scale compensation explicitly, so for newer files, we shouldn't
    /// do it ourselves.  If we try to do it, we'll end up only compensating for the compensation,
    /// and thus undoing it. (Bug #57022)
    void setIgnoreSegScaleComp(bool value);
    /// This is a workaround around a bug in FBX SDK. If a vertex cache is created
    /// as a float type, the SDK cannot recognize any difference between that and
    /// the double type. If this option is true, the importer will assume any
    /// vertex caches are actually float type caches. If not, it assumes they are
    /// double. False by default.
    void setTreatVertexCachesAsFloats(bool value);
    /// If true, forces the normalization of vertex weights on import.
    void setForceNormalizeVertexWeights(bool value);
    /// Animation resampling interval, in frames, between each sample. Defaults to 1.0.
    void setResampleInterval(double frames);
    /// If true, triangulates all patches and imports them as meshes.
    void setTriangulatePatches(bool value);
    /// If true, triangulates all NURBs objects and imports them as meshes.
    void setTriangulateNurbs(bool value);
    /// If true, animation will be resample into a linearly interpolated
    /// keys at every integer key frame.
    void setResampleAnimation(bool value);

private:

    /// Triangulates all NURBs objects and imports them as meshes.
    bool myTriangulateNurbs;

    /// Triangulates all patches and imports them as meshes.
    bool myTriangulatePatches;

    /// If true, animation will be resample into a linearly interpolated
    /// keys.
    bool myResampleAnimation;

    /// Animation resampling interval, in frames, between each sample.
    double myResampleInterval;

    /// This is a workaround around a bug in FBX SDK. If a vertex cache is created
    /// as float type, FBX doesn't convert individual values from float to double; instead,
    /// it justs casts the entire pointer to the double* from float*. It's wrong, but we can cast
    /// it back. This option does that. Off by default.
    bool myTreatVertexCachesAsFloats;

    /// Force the normalization of vertex weights
    bool myForceNormalizeVertexWeights;

    ///  If true, objects with Blend deforms on them will be imported as Blend SOPs. If false,
    /// they'll be imported as channels.
    bool myImportBlendingAsSOPs;

    /// If true, no nodes will be generated for the "Segment Scale Compensate" feature in Maya.
    /// Maya's FBX plugin changed its behaviour between the 2009 and 2010 versions to bake
    /// out the segment scale compensation explicitly, so for newer files, we shouldn't
    /// do it ourselves.  If we try to do it, we'll end up only compensating for the compensation,
    /// and thus undoing it. (Bug #57022)
    bool myIgnoreSegScaleComp;

    /// Determines whether uv coordinates are named Houdini-style or with native FBX names.
    FBX_TextureCoordinatesNamingType myUVNamingStyle;

    /// If true, and if importing UVs with original FBX names, this
    /// will create one set of UVs with standard Houdini names for OGL display.
    bool myCreateOGLDummyCoords;

    /// This will try to adjust the importer to specific file sources (Maya, Max, etc.) and
    /// their specific bugs.
    FBX_CompatibilityModeType myCompatibilityMode;

    /// Determines whether materials are imported as VOP Networks or as FBX Shaders.
    FBX_MaterialImportType myMaterialImportMode;

    /// The framerate to use to overwrite FBX's own rate. Defaults to -1 (meaning no override).
    double myOverrideFramerate;

    /// If true, all geometry File SOPs will be left unlocked, which means on load, an original
    /// FBX file will be required (at the same location).
    bool myUnlockGeometry;

    /// If true, all deformation File CHOPs will be left unlocked, which means on load, an original
    /// FBX file will be required (at the same location).
    bool myUnlockDeformations;

    /// If true, the importer will attempt to make all paths, including textures and .fbx files, relative
    /// (starting with $HIP). If false, paths will be absolute. The trick is that if paths are relative, the
    /// user *must* save the .hip file into the same directory as the .fbx file, otherwise all references
    /// will be lost.
    bool myMakePathsRelative;

    /// If true, joints attached to skins will have their display disabled.
    bool myHideSkinJoints;

    /// If true, joints will import with the ZYX rotation order
    bool myConvertJointsToZYXRotOrder;

    /// If true, null nodes will be created as subnets (except for joints). The children will then be
    /// placed within these subnets, instead of parented to them.
    bool myCreateNullsAsSubnets;

    /// If true, objects are imported directly into /obj network, instead of a new subnet for each FBX
    /// file.
    bool myImportIntoObj;

    /// If true, the FBX scene will be converted to Y-up on import. If false, the scene will be imported
    /// as-is. True by default.
    bool myConvertToYUp;
};
/********************************************************************************************************/
class FBX_API FBX_ObjectTypeFilter
{
public:
    FBX_ObjectTypeFilter();

    void setDoGeometry(bool bValue);
    void setDoLights(bool bValue);
    void setDoCameras(bool bValue);
    void setDoBones(bool value);
    void setDoAnimation(bool value);
    void setDoMaterials(bool value);
    void setImportGlobalAmbientLight(bool value);

    bool getImportGlobalAmbientLight() const;
    bool getDoMaterials() const;
    bool getDoAnimation() const;
    bool getDoBones() const;
    bool getDoGeometry() const;
    bool getDoLights() const;
    bool getDoCameras() const;

private:

    bool myImportGlobalAmbientLight;

    bool myDoMaterials;
    bool myDoBones;
    bool myDoGeometry;
    bool myDoLights;
    bool myDoCameras;
    bool myDoAnimation;
};
/********************************************************************************************************/
#endif // __FBX_Common_h__
