/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://ogre.sourceforge.net/

Copyright � 2000-2003 The OGRE Team
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
-----------------------------------------------------------------------------
*/
#ifndef __D3D9HARDWAREVERTEXBUFFER_H__
#define __D3D9HARDWAREVERTEXBUFFER_H__

#include "OgreD3D9Prerequisites.h"
#include "OgreHardwareVertexBuffer.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>


namespace Ogre {

    /// Specialisation of HardwareVertexBuffer for D3D9
    class D3D9HardwareVertexBuffer : public HardwareVertexBuffer 
    {
    protected:
        LPDIRECT3DVERTEXBUFFER9 mlpD3DBuffer;
    public:
		D3D9HardwareVertexBuffer(size_t vertexSize, size_t numVertices, 
            HardwareBuffer::Usage usage, LPDIRECT3DDEVICE9 pDev);
        ~D3D9HardwareVertexBuffer();
        /** See HardwareBuffer. */
        void* lock(size_t offset, size_t length, LockOptions options);
        /** See HardwareBuffer. */
		void unlock(void);
        /** See HardwareBuffer. */
        void readData(size_t offset, size_t length, void* pDest);
        /** See HardwareBuffer. */
        void writeData(size_t offset, size_t length, const void* pSource,
				bool discardWholeBuffer = false);

        /// Get D3D9-specific vertex buffer
        LPDIRECT3DVERTEXBUFFER9 getD3D9VertexBuffer(void) const { return mlpD3DBuffer; }


    };

}
#endif

