/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright � 2000-2002 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General  License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General  License for more details.

You should have received a copy of the GNU Lesser General  License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

-----------------------------------------------------------------------------
*/

/***************************************************************************
OgreActionEvent.h  -  
  A semantic event which indicates that a component-defined action occured.
  This high-level event is generated by a component (such as a Button) when
  the component-specific action occurs (such as being pressed).
  The event is passed to every every ActionListener object
  that registered to receive such events using the component's
  addActionListener method.

  The object that implements the ActionListener interface
  gets this ActionEvent when the event occurs. The listener
  is therefore spared the details of processing individual mouse movements
  and mouse clicks, and can instead process a "meaningful" (semantic)
  event like "button pressed".

-------------------
begin                : Nov 19 2002
copyright            : (C) 2002 by Kenny Sabir
email                : kenny@sparksuit.com
***************************************************************************/


#ifndef __ActionEvent_H__
#define __ActionEvent_H__

#include "OgrePrerequisites.h"
#include "OgreString.h"
#include "OgreInputEvent.h"

namespace Ogre {

/***
  A semantic event which indicates that a component-defined action occured.
  @remarks
	This high-level event is generated by a component (such as a Button) when
	the component-specific action occurs (such as being pressed).
	The event is passed to every every ActionListener object
	that registered to receive such events using the component's
	addActionListener method.
  @par
	The object that implements the ActionListener interface
	gets this ActionEvent when the event occurs. The listener
	is therefore spared the details of processing individual mouse movements
	and mouse clicks, and can instead process a "meaningful" (semantic)
	event like "button pressed".
 */
    class _OgreExport ActionEvent : public InputEvent
    {
    protected:
	/**
	 * The nonlocalized string that gives more details
	 * of what actually caused the event.
	 * This information is very specific to the component
	 * that fired it.

	 */
	String mActionCommand;



	public:
	enum
	{
	/**
	 * The first number in the range of ids used for action events.
	 */
		AE_ACTION_FIRST				= 1001,

	/**
	 * The last number in the range of ids used for action events.
	 */
		AE_ACTION_LAST		        = 1001
	};

	/**
	 * This event id indicates that a meaningful action occured.
	 */
	enum
	{
		AE_ACTION_PERFORMED	= AE_ACTION_FIRST 
	};


		/**
		 * Constructs a ActionEvent object with the specified source GuiElement,
		 * type, modifiers, coordinates, and click count.
		 *
		 * @param source       the GuiElement that originated the event
		 * @param id           the integer that identifies the event
		 * @param when         a long int that gives the time the event occurred
		 * @param modifiers    the modifier keys down during event
		 *                     (shift, ctrl, alt, meta)
		 * @param x            the horizontal x coordinate for the mouse location
		 * @param y            the vertical y coordinate for the mouse location
		 * @param actionCommand The nonlocalized string that gives more details
		 * of what actually caused the event.
		 */
		 ActionEvent(ActionTarget* source, int id, Real when, int modifiers,
			const String& actionCommand);

		/**
		 * Returns the command string associated with this action.
		 * This string allows a "modal" component to specify one of several 
		 * commands, depending on its state. For example, a single button might
		 * toggle between "show details" and "hide details". The source object
		 * and the event would be the same in each case, but the command string
		 * would identify the intended action.
		 *
		 *@return the string identifying the command for this event
		 */
		const String& getActionCommand() const;


		/**
		 * Returns a parameter string identifying this action event.
		 * This method is useful for event-logging and for debugging.
		 * 
		 * @return a string identifying the event and its associated command 
		 */
		String paramString() const;

    };


}


#endif 

