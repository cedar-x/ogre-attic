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
#include "EditorManager.h"

#include <wx/aui/auibook.h>

#include "Editor.h"

template<> EditorManager* Ogre::Singleton<EditorManager>::ms_Singleton = 0;

EditorManager& EditorManager::getSingleton(void)
{  
	assert( ms_Singleton );  return ( *ms_Singleton );  
}

EditorManager* EditorManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

EditorManager::EditorManager(wxAuiNotebook* notebook)
: mEditorNotebook(notebook), mActiveEditor(NULL)
{
	registerEvents();

	//Connect(mEditorNotebook->GetId(), EVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(EditorManager::OnPageChanged));
}

EditorManager::~EditorManager()
{

}

void EditorManager::openEditor(Editor* editor)
{
	
}

void EditorManager::closeEditor(Editor* editor)
{
}


Editor* EditorManager::getActiveEditor()
{
	return mActiveEditor;
}

void EditorManager::setActiveEditor(Editor* editor)
{
	if(mActiveEditor != NULL) mActiveEditor->deactivate();

	mActiveEditor = editor;

	if(mActiveEditor != NULL) mActiveEditor->activate();
}

void EditorManager::registerEvents()
{

}
void EditorManager::OnPageChanged(wxAuiNotebookEvent& event)
{
}