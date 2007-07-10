/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

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

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/

#include "OgreStableHeaders.h"
#include "OgreMaterialScriptCompiler2.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"
#include "OgrePass.h"

namespace Ogre{

	// MaterialScriptCompilerListener
	MaterialScriptCompilerListener::MaterialScriptCompilerListener()
	{
	}

	bool MaterialScriptCompilerListener::processNode(ScriptNodeList::iterator &iter, ScriptNodeList::iterator &end, MaterialScriptCompiler2*)
	{
		return false;
	}

	Material *MaterialScriptCompilerListener::getMaterial(const Ogre::String &name, const Ogre::String &group)
	{
		Material *material = (Material*)MaterialManager::getSingleton().create(name, group).get();
		material->removeAllTechniques();

		return 0;
	}

	GpuProgram *MaterialScriptCompilerListener::getGpuProgram(const String &name, const String &group, GpuProgramType type, const String &syntax)
	{
		return 0;
	}

	HighLevelGpuProgram *MaterialScriptCompilerListener::getHighLevelGpuProgram(const String &name, const String &group, GpuProgramType type, const String &language)
	{
		return 0;
	}

	void MaterialScriptCompilerListener::preApplyTextureAliases(Ogre::AliasTextureNamePairList &aliases)
	{
	}

	// MaterialScriptCompiler2
	MaterialScriptCompiler2::MaterialScriptCompiler2()
		:mListener(0)
	{
		mAllowNontypedObjects = false; // All material objects must be typed
	}

	void MaterialScriptCompiler2::setListener(MaterialScriptCompilerListener *listener)
	{
		mListener = listener;
	}

	bool MaterialScriptCompiler2::compileImpl(ScriptNodeListPtr nodes)
	{
		ScriptNodeList::iterator i = nodes->begin();
		while(i != nodes->end())
		{
			// Delegate some processing to the listener
			if(!processNode(i, nodes->end()))
			{
				if((*i)->token == "abstract")
				{
					// Abstract is followed by the type then the name.
					// Jump past the abstract and the token for the object type.
					++i;
					++i;
				}
				else if((*i)->token == "material")
				{
					compileMaterial(i, nodes->end());
				}
				else if((*i)->token == "fragment_program")
				{
				}
				else if((*i)->token == "vertex_program")
				{
				}
				else
				{
					// Just move forward
					++i;
				}
			}
		}
		return mErrors.empty();
	}

	bool MaterialScriptCompiler2::processNode(ScriptNodeList::iterator &i, ScriptNodeList::iterator &end)
	{
		if(mListener)
			return mListener->processNode(i, end, this);
		return false;
	}

	ScriptNodeListPtr MaterialScriptCompiler2::loadImportPath(const Ogre::String &name)
	{
		ScriptNodeListPtr nodes;

		// Try the listener
		if(mListener)
			nodes = mListener->importFile(name);

		// Try the base version
		if(nodes.isNull())
			nodes = ScriptCompiler::loadImportPath(name);

		// If we got any AST loaded, do the necessary pre-processing steps
		if(!nodes.isNull())
		{
			// Expand all imports
			processImports(nodes);
		}

		return nodes;
	}

	void MaterialScriptCompiler2::compileMaterial(ScriptNodeList::iterator &i, ScriptNodeList::iterator &end)
	{
		// The next token needs to be the name of the material
		{
			ScriptNodeList::iterator j = i;
			++j;

			if(j == end)
			{
				addError(CE_OBJECTNAMEEXPECTED, (*j)->file, (*j)->line, (*j)->column);
				return;
			}
			i = j;
		}

		if((*i)->type != SNT_STRING)
		{
			addError(CE_OBJECTNAMEEXPECTED, (*i)->file, (*i)->line, (*i)->column);
			return;
		}

		// We have verified that a name is here, so get the material
		if(mListener)
		{
			mMaterial = mListener->getMaterial((*i)->token, mGroup);
		}
		else
		{
			mMaterial = (Material*)MaterialManager::getSingleton().create((*i)->token, mGroup).get();
			mMaterial->removeAllTechniques();
		}

		if(!mMaterial)
		{
			addError(CE_OBJECTALLOCATIONERROR, (*i)->file, (*i)->line, (*i)->column);
			return;
		}

		// The next token should be the '{'
		{
			ScriptNodeList::iterator j = i;
			j++;

			// We can't end here!
			if(j == end)
			{
				// Bail out!
				addError(CE_OPENBRACEEXPECTED, (*i)->file, (*i)->line, (*i)->column);
				mMaterial = 0;
				return;
			}

			// The next token must be the '{' starting the particle system object's body
			if((*j)->type != SNT_LBRACE)
			{
				addError(CE_OPENBRACEEXPECTED, (*j)->file, (*j)->line, (*j)->column);
				mMaterial = 0;
				return;
			}
			
			// We're ok, so continue on
			i = j;
		}

		// We hit the '{', so descend into it to continue compilation
		ScriptNodeList::iterator j = (*i)->children.begin();
		while(j != (*i)->children.end())
		{
			if(!processNode(j, (*i)->children.end()))
			{
				if((*j)->token == "lod_distances")
				{
					Ogre::Material::LodDistanceList lods;

					// Read any number tokens after this one
					++j;
					while(j != (*i)->children.end() && (*j)->type == SNT_NUMBER)
					{
						lods.push_back(StringConverter::parseReal((*j)->token));
						++j;
					}

					if(!lods.empty())
						mMaterial->setLodLevels(lods);
					else
						addError(ME_LODLISTEXPECTED, (*j)->file, (*j)->line, (*j)->column);
				}
				else if((*j)->token == "receive_shadows")
				{
					if(!nodeExists(j, (*i)->children.end(), 1))
					{
						addError(CE_TRUTHVALUEEXPECTED, (*j)->file, (*j)->line, (*j)->column);
						break;
					}

					// The next token needs to be a truth value
					if((*j)->token != "on" || (*j)->token != "off")
						addError(CE_TRUTHVALUEEXPECTED, (*j)->file, (*j)->line, (*j)->column);
					else
						mMaterial->setReceiveShadows(isTruthValue((*j)->token));
				}
				else if((*j)->token == "transparency_cast_shadows")
				{
					if(!nodeExists(j, (*i)->children.end(), 1))
					{
						addError(CE_TRUTHVALUEEXPECTED, (*j)->file, (*j)->line, (*j)->column);
						break;
					}

					if((*j)->token != "on" || (*j)->token != "off")
						addError(CE_TRUTHVALUEEXPECTED, (*j)->file, (*j)->line, (*j)->column);
					else
						mMaterial->setTransparencyCastsShadows(isTruthValue((*j)->token));
				}
				else if((*j)->token == "set_texture_alias" &&
					nodeExists(j, (*i)->children.end(), 1) &&
					nodeExists(j, (*i)->children.end(), 2))
				{
					String aliasName, textureName;

					if(!nodeExists(j, (*i)->children.end(), 1))
					{
						addError(CE_STRINGEXPECTED, (*j)->file, (*j)->line, (*j)->column);
						break;
					}
					++j;
					aliasName = (*j)->token;

					if(!nodeExists(j, (*i)->children.end(), 1))
					{
						addError(CE_STRINGEXPECTED, (*j)->file, (*j)->line, (*j)->column);
						break;
					}
					++j;
					textureName = (*j)->token;

					mTextureAliases.insert(std::make_pair(aliasName, textureName));
				}
				else if((*j)->token == "technique")
				{
					compileTechnique(j, (*i)->children.end());
				}
			}
		}

		// We are finished with the object, so consume the '}'
		++i; // '{'
		++i; // '}'

		// Apply the texture alises
		if(mListener)
			mListener->preApplyTextureAliases(mTextureAliases);
		mMaterial->applyTextureAliases(mTextureAliases);

		// Reset the pointer to the system
		mMaterial = 0;
		// Remove texture alises
		mTextureAliases.clear();
	}

	void MaterialScriptCompiler2::compileTechnique(ScriptNodeList::iterator &i, ScriptNodeList::iterator &end)
	{
		// Consume the "technique" node
		if(!nodeExists(i, end, 1))
		{
			addError(ME_TECHNIQUEBODYEXPECTED, (*i)->file, (*i)->line, (*i)->column);
			return;
		}
		++i;

		// Create the technique being compiled
		Technique *technique = mMaterial->createTechnique();

		// Consume the name if there is one
		if((*i)->type != SNT_LBRACE)
		{
			technique->setName((*i)->token);

			if(!nodeExists(i, end, 1))
			{
				addError(ME_TECHNIQUEBODYEXPECTED, (*i)->file, (*i)->line, (*i)->column);
				return;
			}
			++i;
		}

		if((*i)->type != SNT_LBRACE)
		{
			addError(CE_OPENBRACEEXPECTED, (*i)->file, (*i)->line, (*i)->column);
			return;
		}

		// Hit the '{', so compile the technique's parameters
		ScriptNodeList::iterator j = (*i)->children.begin();
		while(j != (*i)->children.end())
		{
			if(!processNode(j, (*i)->children.end()))
			{
				if((*j)->token == "scheme")
				{
					if(!nodeExists(j, (*i)->children.end(), 1))
					{
						addError(CE_STRINGEXPECTED, (*j)->file, (*j)->line, (*j)->column);
						break;
					}
					++j;

					technique->setSchemeName((*j)->token);
				}
				else if((*j)->token == "lod_index")
				{
					if(!nodeExists(j, (*i)->children.end(), 1))
					{
						addError(CE_NUMBEREXPECTED, (*j)->file, (*j)->line, (*j)->column);
						break;
					}
					++j;

					if((*j)->type != SNT_NUMBER)
					{
						addError(CE_NUMBEREXPECTED, (*j)->file, (*j)->line, (*j)->column);
						continue;
					}

					technique->setLodIndex(StringConverter::parseUnsignedInt((*j)->token));
				}
				else if((*j)->token == "pass")
				{

				}
			}

			// Consume the '{' and the '}'
			++i;
			++i;
		}
	}

	void MaterialScriptCompiler2::compilePass(ScriptNodeList::iterator &i, ScriptNodeList::iterator &end)
	{

	}

}