/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.stevestreeting.com/ogre/

Copyright � 2000-2001 Steven J. Streeting
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/gpl.html.
-----------------------------------------------------------------------------
*/
#ifndef _String_H__
#define _String_H__

#include "OgrePrerequisites.h"

namespace Ogre {
    typedef std::basic_string< Char > _StringBase;
       
};

#ifdef GCC_3_1
#include <ext/hash_map>
namespace __gnu_cxx
{
  template <> struct hash<Ogre::_StringBase>
  {
    size_t operator()(const Ogre::_StringBase _stringBase) const 
    { return __stl_hash_string(_stringBase.c_str()); }
  };
};
#endif

namespace Ogre {

#ifdef GCC_3_1
    typedef ::__gnu_cxx::hash<_StringBase> _StringHash;    
#else
    typedef std::hash<_StringBase> _StringHash;
#endif

    /** Wrapper object for String to allow DLL export.
        @note
            <br>Derived from std::basic_string<char>.
    */
    class _OgreExport String : public _StringBase
    {
    public:
        /** Default constructor.
        */
        String() : 
          _StringBase() {}

        /** Normal copy constructor.
        */
        String(const String& rhs) :
            _StringBase( static_cast<std::string>( rhs ) ) {}

        /** Copy constructor for std::string's.
        */
        String(const _StringBase& rhs) : _StringBase( rhs ) {}

        /** Copy-constructor for C-style strings.
        */
        String(const char* rhs) : _StringBase( rhs ) {}

        /** Used for interaction with functions that require the old C-style
            strings.
        */
        operator const char* () const { return c_str(); }

        /** Removes any whitespace characters, be it standard space or
            TABs and so on.
            @remarks
                The user may specify wether they want to trim only the
                beginning or the end of the String ( the default action is
                to trim both).
        */
        void trim(bool left = true, bool right = true);

        /** Returns a StringVector that contains all the substrings delimited
            by the characters in the passed <code>delims</code> argument.
        @param delims A list of delimiter characters to split by
        @param maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
            parameters is > 0, the splitting process will stop after this many splits, left to right.
        */
        std::vector<String> split( const String& delims = "\t\n ", unsigned int maxSplits = 0) const;

        /** Upper-cases all the characters in the string.
        */
        String toLowerCase( void );

        /** Lower-cases all the characters in the string.
        */
        String toUpperCase( void );

        /** Converts the contents of the string to a Real.
        @remarks
            Assumes the only contents of the string are a valid parsable Real. Defaults to  a
            value of 0.0 if conversion is not possible.
        */
        Real toReal(void);
    };

} // namespace Ogre

#endif // _String_H__
