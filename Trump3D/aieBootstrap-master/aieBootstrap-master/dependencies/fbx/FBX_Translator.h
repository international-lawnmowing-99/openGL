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
 * NAME:	FBX_Translator.h (FBX Library, C++)
 *
 * COMMENTS:	
 *
 */

#ifndef __FBX_Translator_h__
#define __FBX_Translator_h__

#include "FBX_Common.h"
#include "FBX_ErrorManager.h"

#include <UT/UT_StringArray.h>


class FBX_IntTranslator;
class GU_Detail;
class OP_Node;
class OP_Network;
/********************************************************************************************************/
class FBX_API FBX_LoadCallbacks
{
public:
    virtual ~FBX_LoadCallbacks() { }
    virtual int onFbxFileLoaded(GU_Detail *gdp, FBX_IntTranslator *ptranslator) = 0;
};
/********************************************************************************************************/
class FBX_API FBX_CookGeoCallback : public FBX_LoadCallbacks
{
public:
    virtual ~FBX_CookGeoCallback() { }
    virtual int onFbxFileLoaded(GU_Detail *gdp, FBX_IntTranslator *ptranslator);
};
/********************************************************************************************************/
class FBX_API FBX_LoadAnimCallback : public FBX_LoadCallbacks
{
public:
    FBX_LoadAnimCallback()
    {
	myTargetHdNode = NULL;
	myHdParmIndex = 0;
	myFbxComponent = -1;
	myResampleAnimation = false;
	myResampleInterval = 1.0;
    }
    virtual ~FBX_LoadAnimCallback() { }
    virtual int onFbxFileLoaded(GU_Detail *gdp, FBX_IntTranslator *ptranslator);

    void setHdNode(OP_Node* node) { myTargetHdNode = node; }
    void setTargetParm(const char* parm_name, int idx) 
    { 
	if(!parm_name)
	    return;

	myHdParmName = parm_name;
	myHdParmIndex = idx;
    }
    void setFbxProperty(const char* pname, int icomp)
    {
	if(pname)
	    myFbxPropertyName = pname;
	myFbxComponent = icomp;
    }

    void setFbxTakeName(const char* pname)
    { myFbxTakeName = pname;   }
    void setResampleAnimation(bool value)
    { myResampleAnimation = value; }
    void setResampleInterval(double freq)
    { myResampleInterval = freq; }

private:
    OP_Node* myTargetHdNode;
    std::string myHdParmName;
    int myHdParmIndex;
    std::string myFbxPropertyName;
    int myFbxComponent;

    std::string myFbxTakeName;

    bool myResampleAnimation;
    double myResampleInterval;
};
/********************************************************************************************************/
#ifdef FBX_ENABLED

class FBX_API FBX_Translator
{
public: 
    /// Constructor.
    FBX_Translator();

    /// Destructor.
    virtual ~FBX_Translator();

    /// Initializes the Filmbox translator's environment.
    /// It should be called once at startup.
    static void			     init(void);

    /// Uninitializes the Filmbox translator's environment.
    /// It should be called once at termination.
    static void			     uninit(void);

    const FBX_ErrorManager* getErrorManager(void) const;

    /// Imports the Filmbox scene into the current Houdini session or into a new scene.
    /// @param	fileURI	    A path to the FBX file to import. Please note that network (UNC)
    ///			    paths are currently not supported.
    /// @param	is_merging  If true, the file will be merged with the current scene, if
    ///			    false, a new scene will be created and the file will be imported
    ///			    there.
    /// @param	options	    Various options that determine how a file is to be imported. See
    ///			    FBX_ImportOptions for more details. If not set, the defaults for
    ///			    every option will be used.
    /// @param	filter	    Object type filter settings which determine what types of objects
    ///			    (cameras, geometry, lights, etc.) are to be imported. If not set,
    ///			    the defaults are used, which import everything.
    /// @param	password    A password to use if the FBX file is password-protected. Currently
    ///			    not used.
    bool			     importScene(   const char *fileURI, bool is_merging,
						    FBX_ImportOptions *options = NULL,
						    FBX_ObjectTypeFilter *filter = NULL,
						    const char* password = NULL,
						    OP_Network** parent_net = NULL);

    static int			     importGeo(GU_Detail *gdp,
					       const char *fileURI, const char* fbx_node_name, bool convert_surfaces);

    static int			     ensureFileLoaded(GU_Detail *gdp, const char *fileURI, const char* fbx_node_name, bool convert_surfaces, FBX_LoadCallbacks* pcallbacks);

    static int			     exportGeo(const GU_Detail *gdp,
					       const char *fileURI);

    static bool isSupported(void)
	{ return true; }

private:
    static void			     installCommands();

private:

    FBX_ErrorManager myErrorManager;

};

#else

class FBX_API FBX_Translator
{
public: 
    /// Constructor.
    FBX_Translator() { }

    /// Destructor.
    virtual ~FBX_Translator() { }

    /// Initializes the Filmbox translator's environment.
    /// It should be called once at startup.
    static void			     init(void) { }

    /// Uninitializes the Filmbox translator's environment.
    /// It should be called once at termination.
    static void			     uninit(void) { }

    const FBX_ErrorManager* getErrorManager(void) const { return NULL; }

    /// Imports the Filmbox scene into the current Houdini session or into a new scene.
    /// @param	fileURI	    A path to the FBX file to import. Please note that network (UNC)
    ///			    paths are currently not supported.
    /// @param	is_merging  If true, the file will be merged with the current scene, if
    ///			    false, a new scene will be created and the file will be imported
    ///			    there.
    /// @param	options	    Various options that determine how a file is to be imported. See
    ///			    FBX_ImportOptions for more details. If not set, the defaults for
    ///			    every option will be used.
    /// @param	filter	    Object type filter settings which determine what types of objects
    ///			    (cameras, geometry, lights, etc.) are to be imported. If not set,
    ///			    the defaults are used, which import everything.
    /// @param	password    A password to use if the FBX file is password-protected. Currently
    ///			    not used.
    bool			     importScene(   const char *fileURI, bool is_merging,
						    FBX_ImportOptions *options = NULL,
						    FBX_ObjectTypeFilter *filter = NULL,
						    const char* password = NULL,
						    OP_Network** parent_net = NULL)
					{ return true; }

    static int			     importGeo(GU_Detail *gdp,
	const char *fileURI, const char* fbx_node_name, bool convert_surfaces) { return 0; }

    static int			     ensureFileLoaded(GU_Detail *gdp, const char *fileURI, 
						    const char* fbx_node_name, bool convert_surfaces, 
						    FBX_LoadCallbacks* pcallbacks) { return false; }

    static int			     exportGeo(const GU_Detail *gdp,
	const char *fileURI) { return 0;  }

    static bool isSupported(void)
	{ return false; }
private:
    static void			     installCommands() { }
};
#endif
/********************************************************************************************************/    

#endif // __FBX_Translator_h__

