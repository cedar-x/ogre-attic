/*
-----------------------------------------------------------------------------
This source file is part of OGRE 
	(Object-oriented Graphics Rendering Engine)
For the latest info, see http://ogre.sourceforge.net/

Copyright � 2000-2002 The OGRE Team
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
#ifndef __RotationAffector_H__
#define __RotationAffector_H__

#include "OgreParticleFXPrerequisites.h"
#include "OgreParticleAffector.h"
#include "OgreStringInterface.h"

namespace Ogre {


    /** This plugin subclass of ParticleAffector allows you to alter the rotation of particles.
    @remarks
        This class supplies the ParticleAffector implementation required to make the particle expand
		or contract in mid-flight.
    */
    class _OgreParticleFXExport RotationAffector : public ParticleAffector
    {
    public:
		/// Command object for particle emitter  - see ParamCommand 
        class CmdRotationSpeedRangeStart : public ParamCommand
        {
        public:
            String doGet(void* target);
            void doSet(void* target, const String& val);
        };

        /// Command object for particle emitter  - see ParamCommand 
        class CmdRotationSpeedRangeEnd : public ParamCommand
        {
        public:
            String doGet(void* target);
            void doSet(void* target, const String& val);
        };

		/// Command object for particle emitter  - see ParamCommand 
        class CmdRotationRangeStart : public ParamCommand
        {
        public:
            String doGet(void* target);
            void doSet(void* target, const String& val);
        };

        /// Command object for particle emitter  - see ParamCommand 
        class CmdRotationRangeEnd : public ParamCommand
        {
        public:
            String doGet(void* target);
            void doSet(void* target, const String& val);
        };

        /** Default constructor. */
        RotationAffector();

        /** See ParticleAffector. */
		void _initParticle(Particle* pParticle);

        /** See ParticleAffector. */
        void _affectParticles(ParticleSystem* pSystem, Real timeElapsed);



		/** Sets the minimum rotation speed of particles to be emitted. */
        void setRotationSpeedRangeStart(Real angle);
        /** Sets the maximum rotation speed of particles to be emitted. */
        void setRotationSpeedRangeEnd(Real angle);
        /** Gets the minimum rotation speed of particles to be emitted. */
        Real getRotationSpeedRangeStart(void);
        /** Gets the maximum rotation speed of particles to be emitted. */
        Real getRotationSpeedRangeEnd(void);

		
		/** Sets the minimum rotation angle of particles to be emitted. */
        void setRotationRangeStart(Real angle);
        /** Sets the maximum rotation angle of particles to be emitted. */
        void setRotationRangeEnd(Real angle);
        /** Gets the minimum rotation of particles to be emitted. */
        Real getRotationRangeStart(void);
        /** Gets the maximum rotation of particles to be emitted. */
        Real getRotationRangeEnd(void);

		static CmdRotationSpeedRangeStart	msRotationSpeedRangeStartCmd;
        static CmdRotationSpeedRangeEnd		msRotationSpeedRangeEndCmd;
        static CmdRotationRangeStart		msRotationRangeStartCmd;
        static CmdRotationRangeEnd			msRotationRangeEndCmd;
        
    protected:
        /// Initial rotation speed of particles (range start)
        Real mRotationSpeedRangeStart;
        /// Initial rotation speed of particles (range end)
        Real mRotationSpeedRangeEnd;
        /// Initial rotation angle of particles (range start)
        Real mRotationRangeStart;
        /// Initial rotation angle of particles (range end)
        Real mRotationRangeEnd;

    };


}


#endif

