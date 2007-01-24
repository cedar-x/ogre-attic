/*
-----------------------------------------------------------------------------
This source file is part of LEXIExporter

Copyright 2006 NDS Limited

Author(s):
Mark Folkenberg,
Bo Krohn

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#include "LexiStdAfx.h"
#include "LexiExportObjectMesh.h"
#include "LexiOgreMeshCompiler.h"
#include "LexiOgreMaterialCompiler.h"
#include "LexiIntermediateBuilder.h"
#include "LexiIntermediateMesh.h"

#include <dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")
//

CMeshExportObject::CMeshExportObject() : CExportObject("mesh")
{
	m_pDDMetaDesc = BuildMetaDesc();
	//m_pDDMetaDesc->SaveASCII("C:\\METADESC.ddconf");
}

CMeshExportObject::~CMeshExportObject()
{
	m_pDDMetaDesc->Release();
}

//

void CMeshExportObject::Read(CDDObject* pConfig)
{
	CExportObject::Read(pConfig);
}

void CMeshExportObject::Write(CDDObject* pConfig) const
{
	CExportObject::Write(pConfig);
}

//

bool CMeshExportObject::SupportsClass(SClass_ID nClass) const
{
	if(nClass == GEOMOBJECT_CLASS_ID) return true;
	return false;
}

//

CDDObject* CMeshExportObject::GetMetaDesc() const
{
	return m_pDDMetaDesc;
}

CDDObject* CMeshExportObject::GetEditMeta() const
{
	return m_pDDEditMeta;
}

CDDObject* CMeshExportObject::BuildMetaDesc( void )
{
	if(m_pDDMetaDesc != NULL)
		return m_pDDMetaDesc;

	CDDObject* pDDMetaDesc = new CDDObject();

	fastvector< const CDDObject* > lSettings;
	CDDObject* pDDMetaElement;

	pDDMetaElement = new CDDObject();
	pDDMetaElement->SetString("ID","normalsID");
	pDDMetaElement->SetString("Type","bool");
	pDDMetaElement->SetString("Group","Export Settings");
	pDDMetaElement->SetString("Caption","Vertex Normals");
	pDDMetaElement->SetString("Help","Export vertex normals");
	//pDDMetaElement->SetBool("normalsID",false);
	pDDMetaElement->SetBool("Default", true);
	lSettings.push_back(pDDMetaElement);

	pDDMetaElement = new CDDObject();
	pDDMetaElement->SetString("ID","vertexColorsID");
	pDDMetaElement->SetString("Type","bool");
	pDDMetaElement->SetString("Group","Export Settings");
	pDDMetaElement->SetString("Caption","Vertex Colors");
	pDDMetaElement->SetString("Help","Export baked vertex colors");
	//pDDMetaElement->SetBool("vertexColorsID",false);
	pDDMetaElement->SetBool("Default", true);
	lSettings.push_back(pDDMetaElement);

	pDDMetaElement = new CDDObject();
	pDDMetaElement->SetString("ID","uvID");
	pDDMetaElement->SetString("Type","bool");
	pDDMetaElement->SetString("Group","Export Settings");
	pDDMetaElement->SetString("Caption","Texture Coordinates");
	pDDMetaElement->SetString("Help","Export texture vertex coordinates");
	//pDDMetaElement->SetBool("uvID",false);
	pDDMetaElement->SetBool("Default", true);
	lSettings.push_back(pDDMetaElement);

	pDDMetaElement = new CDDObject();
	pDDMetaElement->SetString("ID","reindexID");
	pDDMetaElement->SetString("Type","bool");
	pDDMetaElement->SetString("Group","Export Settings");
	pDDMetaElement->SetString("Caption","Reindex Vertices");
	pDDMetaElement->SetString("Help","Optimize Index and Vertex buffer");
	//pDDMetaElement->SetBool("reindexID",false);
	pDDMetaElement->SetBool("Default", true);
	lSettings.push_back(pDDMetaElement);

	//pDDMetaElement = new CDDObject();
	//pDDMetaElement->SetString("ID","collapseHierarchy");
	//pDDMetaElement->SetString("Type","bool");
	//pDDMetaElement->SetString("Group","Export Settings");
	//pDDMetaElement->SetString("Caption","Collapse Hierarchy");
	//pDDMetaElement->SetString("Help","Collapse entire hierarchy into one mesh");
	////pDDMetaElement->SetBool("collapseHierarchy",false);
	//pDDMetaElement->SetBool("Default", false);
	//lSettings.push_back(pDDMetaElement);

	//pDDMetaElement = new CDDObject();
	//pDDMetaElement->SetString("ID","SkeletonID");
	//pDDMetaElement->SetString("Type","bool");
	//pDDMetaElement->SetString("Group","Export Settings");
	//pDDMetaElement->SetString("Caption","Export Skeleton");
	//pDDMetaElement->SetString("Help","Export Skeleton if there is one");
	////pDDMetaElement->SetBool("collapseHierarchy",false);
	//pDDMetaElement->SetBool("Default", false);
	//lSettings.push_back(pDDMetaElement);

	pDDMetaElement = new CDDObject();
	pDDMetaElement->SetString("ID","copyTextureMaps");
	pDDMetaElement->SetString("Type","bool");
	pDDMetaElement->SetString("Group","Resources");
	pDDMetaElement->SetString("Caption","Copy Textures");
	pDDMetaElement->SetString("Help","Copy Textures To Output Folder");
	//pDDMetaElement->SetBool("collapseHierarchy",false);
	pDDMetaElement->SetBool("Default", true);
	lSettings.push_back(pDDMetaElement);

	pDDMetaElement = new CDDObject();
	pDDMetaElement->SetString("ID","copyShaders");
	pDDMetaElement->SetString("Type","bool");
	pDDMetaElement->SetString("Group","Resources");
	pDDMetaElement->SetString("Caption","Copy Shaders");
	pDDMetaElement->SetString("Help","Copy Shaders To Output Folder");
	//pDDMetaElement->SetBool("collapseHierarchy",false);
	pDDMetaElement->SetBool("Default", true);
	lSettings.push_back(pDDMetaElement);

	pDDMetaDesc->SetDDList("MetaList", lSettings, false);

	AddAnimationMetaData(pDDMetaDesc);

	return pDDMetaDesc;
}

//
typedef std::map< Ogre::String, CIntermediateMaterial*> MAT_LIST;

bool CMeshExportObject::Export() const
{

	Ogre::LogManager::getSingleton().logMessage("Export: Starting..");

	CIntermediateBuilder::Get()->Clear();
	CIntermediateBuilder::Get()->SetConfig(m_pDDEditMeta);

	Ogre::LogManager::getSingleton().logMessage("Export: Creating Hierarchy...");

	Ogre::SceneNode* pNode = CIntermediateBuilder::Get()->CreateHierarchy(m_iID, true, false);

	if(pNode == NULL)
	{
		OutputProgress("ERROR!: No node with such ID", 1);
		return false;
	}

	Ogre::LogManager::getSingleton().logMessage("Export: Reading Config..");

	bool bCollaps = m_pDDEditMeta->GetBool("collapseHierarchy", false);
	bool bCopyTextures = m_pDDEditMeta->GetBool("copyTextureMaps", false);
	bool bCopyShaders = m_pDDEditMeta->GetBool("copyShaders", false);

	if(bCollaps)
	{
		std::list<std::string> clist;
		clist.push_back("position");
		clist.push_back("normal");
		clist.push_back("uv1");
		Ogre::SceneNode* pColNode = CIntermediateBuilder::Get()->CollapseHierarchy(pNode, clist, "Collapsed");
		delete pNode;
		pNode = pColNode;
	}

	CIntermediateMesh* iMesh = (CIntermediateMesh*)pNode->getAttachedObject(0);


	Ogre::String str = "Exporting Mesh: "+iMesh->getName();
	OutputProgress(str.c_str(), 1);

	Ogre::LogManager::getSingleton().logMessage("Export: Creating OgreMeshcompiler...");

	COgreMeshCompiler* pOgreMeshCompiler = new COgreMeshCompiler(iMesh, m_pDDEditMeta, m_sFilename);

	Ogre::String sBaseName;
	Ogre::String sPath;
	Ogre::StringUtil::splitFilename(m_sFilename, sBaseName, sPath);
	
	int n = sPath.find("/");
	while(n != Ogre::String::npos)
	{
		sPath.replace(n,1,"\\");
		sPath.insert(n,"\\");
		n = sPath.find("/");
	}

	if(!::MakeSureDirectoryPathExists(sPath.c_str()))
	{
		Ogre::String errorMsg = "Unknown error while attempting to create path: ";
		errorMsg += sPath;
		MessageBox(NULL,errorMsg.c_str(),"ERROR", MB_ICONERROR);

		delete pOgreMeshCompiler;
		delete iMesh;

		return false;
	}

	Ogre::LogManager::getSingleton().logMessage("Export: Writing Ogre Mesh...");


	pOgreMeshCompiler->WriteOgreMesh(m_sFilename);

	Ogre::LogManager::getSingleton().logMessage("Export: Preparing Material Export..");


	bool bInOneFile = true;
	MAT_LIST lMaterials;
	if ( CIntermediateBuilder::Get()->GetMaterials(lMaterials) )
	{
		Ogre::MaterialSerializer matWriter;
		MAT_LIST::iterator it = lMaterials.begin();

		while (it != lMaterials.end())
		{
			COgreMaterialCompiler matComp( it->second );
			//Ogre::MaterialPtr mat = 
			if(bInOneFile)
			{
				Ogre::LogManager::getSingleton().logMessage("Export: Queueing material...");
				matWriter.queueForExport( matComp.GetOgreMaterial() );
			}
			else
			{
				Ogre::LogManager::getSingleton().logMessage("Export: Exporting individual material...");

				try {
					matWriter.exportMaterial( matComp.GetOgreMaterial(), Ogre::String(m_sFilename+".material"));
				} catch (Ogre::Exception e) {
					MessageBox(NULL,e.getDescription().c_str(),"ERROR", MB_ICONERROR);
				}
			}

			if(bCopyTextures) matComp.CopyTextureMaps( sPath );
			if(bCopyShaders) matComp.CopyShaderSources( sPath );
			it++;
		}
		try	{
			Ogre::LogManager::getSingleton().logMessage("Export: Exporting Global material file...");
			matWriter.exportQueued( Ogre::String(m_sFilename+".material") );
		} catch (Ogre::Exception e) {
			MessageBox(NULL,e.getDescription().c_str(),"ERROR", MB_ICONERROR);
		}

	}

	Ogre::LogManager::getSingleton().logMessage("Export: Cleaning Up...");
	delete pOgreMeshCompiler;
	delete iMesh;

	OutputProgress("..Done", 1);
	Ogre::LogManager::getSingleton().logMessage("Export: Done!");

	return true;
}

//

const char* CMeshExportObject::GetDefaultFileExt() const
{
	return "mesh";
}

void CMeshExportObject::AddAnimationMetaData( CDDObject* pDDobj )
{
	CDDObject* AnimContainer = new CDDObject();

	fastvector< const CDDObject* > lAnimSettings;
	CDDObject* pDDAnimElement;

	// --- General Animation ---

	// Animation Name
	pDDAnimElement = new CDDObject();
	pDDAnimElement->SetString("ID","AnimationNameID");
	pDDAnimElement->SetString("Type","string");
	pDDAnimElement->SetString("Group","Animation");
	pDDAnimElement->SetString("Caption","Name");
	pDDAnimElement->SetString("Help","The Name of the Animation.");
	pDDAnimElement->SetBool("Default", "AnimName");
	lAnimSettings.push_back(pDDAnimElement);

	// Animation Type
	pDDAnimElement = new CDDObject();
	pDDAnimElement->SetString("ID","AnimationTypeID");
	pDDAnimElement->SetString("Type","selection");
	pDDAnimElement->SetString("Group","Animation");
	vector< faststring > lTypes;
	lTypes.push_back("Bone");	//0
	//lTypes.push_back("Morph");//1
	//lTypes.push_back("Pose");	//2
	pDDAnimElement->SetStringList("Strings",lTypes);
	pDDAnimElement->SetString("Caption","Type");
	pDDAnimElement->SetString("Help","The Type of the Animation (Bone, Morph or Pose).");
	pDDAnimElement->SetInt("Default", 0);
	lAnimSettings.push_back(pDDAnimElement);

	pDDAnimElement = new CDDObject();
	pDDAnimElement->SetString("ID","AnimationOptimizeID");
	pDDAnimElement->SetString("Type","Bool");
	pDDAnimElement->SetString("Group","Animation");
	pDDAnimElement->SetString("Caption","Optimize");
	pDDAnimElement->SetString("Help","Reduces the amount of KeyFrames by removing redundant data.");
	pDDAnimElement->SetBool("Default", true);
	lAnimSettings.push_back(pDDAnimElement);

	// --- Bone Animation ---

	pDDAnimElement = new CDDObject();
	pDDAnimElement->SetString("ID","AnimationStartID");
	pDDAnimElement->SetString("Type","Int");
	pDDAnimElement->SetBool("EnableSlider", false);
	pDDAnimElement->SetString("Group","Bone");
	pDDAnimElement->SetString("Caption","Start Frame");
	pDDAnimElement->SetString("Help","Frame which the animation begins");
	pDDAnimElement->SetString("Condition", "$AnimationTypeID=0");
	pDDAnimElement->SetInt("Default", 0);
	lAnimSettings.push_back(pDDAnimElement);

	pDDAnimElement = new CDDObject();
	pDDAnimElement->SetString("ID","AnimationEndID");
	pDDAnimElement->SetString("Type","Int");
	pDDAnimElement->SetBool("EnableSlider", false);
	pDDAnimElement->SetString("Group","Bone");
	pDDAnimElement->SetString("Caption","End Frame");
	pDDAnimElement->SetString("Help","Frame which the animation stops");
	pDDAnimElement->SetString("Condition", "$AnimationTypeID=0");
	pDDAnimElement->SetInt("Default", 100);
	lAnimSettings.push_back(pDDAnimElement);

	pDDAnimElement = new CDDObject();
	pDDAnimElement->SetString("ID","AnimationSampleRateID");
	pDDAnimElement->SetString("Type","Float");
	pDDAnimElement->SetBool("EnableSlider", false);
	pDDAnimElement->SetString("Group","Bone");
	pDDAnimElement->SetString("Caption","Samplerate");
	pDDAnimElement->SetString("Help","Rate at which samples should be done. e.g 2 yields every second frame in max.");
	pDDAnimElement->SetString("Condition", "$AnimationTypeID=0");
	pDDAnimElement->SetFloat("Default", 1.0);
	lAnimSettings.push_back(pDDAnimElement);

		// --- Pose Animation ---

	//pDDAnimElement = new CDDObject();
	//pDDAnimElement->SetString("ID","AnimationStartID");
	//pDDAnimElement->SetString("Type","Int");
	//pDDAnimElement->SetBool("EnableSlider", false);
	//pDDAnimElement->SetString("Group","Pose");
	//pDDAnimElement->SetString("Caption","Start Frame");
	//pDDAnimElement->SetString("Help","Frame which the animation begins");
	//pDDAnimElement->SetString("Condition", "$AnimationTypeID=2");
	//pDDAnimElement->SetInt("Default", 0);
	//lAnimSettings.push_back(pDDAnimElement);

	//pDDAnimElement = new CDDObject();
	//pDDAnimElement->SetString("ID","AnimationEndID");
	//pDDAnimElement->SetString("Type","Int");
	//pDDAnimElement->SetBool("EnableSlider", false);
	//pDDAnimElement->SetString("Group","Pose");
	//pDDAnimElement->SetString("Caption","End Frame");
	//pDDAnimElement->SetString("Help","Frame which the animation stops");
	//pDDAnimElement->SetString("Condition", "$AnimationTypeID=2");
	//pDDAnimElement->SetInt("Default", 100);
	//lAnimSettings.push_back(pDDAnimElement);

	//pDDAnimElement = new CDDObject();
	//pDDAnimElement->SetString("ID","AnimationSampleRateID");
	//pDDAnimElement->SetString("Type","Float");
	//pDDAnimElement->SetBool("EnableSlider", false);
	//pDDAnimElement->SetString("Group","Pose");
	//pDDAnimElement->SetString("Caption","Samplerate");
	//pDDAnimElement->SetString("Help","Rate at which samples should be done. e.g 2 yields every second frame in max.");
	//pDDAnimElement->SetString("Condition", "$AnimationTypeID=2");
	//pDDAnimElement->SetFloat("Default", 1.0);
	//lAnimSettings.push_back(pDDAnimElement);

	AnimContainer->SetDDList("MetaList", lAnimSettings, false);
	pDDobj->SetDDObject("AnimationContainer", AnimContainer);

}

//
