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
octree.cpp  -  Slightly modified version of Octree.cpp from TerrainSM. 

-----------------------------------------------------------------------------
begin                : Mon Sep 30 2002
copyright            : (C) 2002 by Jon Anderson
email                : janders@users.sf.net

Enhancements 2003 - 2004 (C) The OGRE Team
Modified to use with PCZones 2007 by Eric Cha
-----------------------------------------------------------------------------
*/

#include <OgreOctree.h>
#include <OgrePCZSceneNode.h>
#include "OgreOctreeZone.h"

namespace Ogre
{

    /* INTERSECTION UTILITY FUNCTIONS ***/

    enum Intersection
    {
        OUTSIDE=0,
        INSIDE=1,
        INTERSECT=2
    };

    Intersection intersect( const Ray &one, const AxisAlignedBox &two )
    {
        // Null box?
        if (two.isNull()) return OUTSIDE;
	    // Infinite box?
	    if (two.isInfinite()) return INTERSECT;

        bool inside = true;
        const Vector3& twoMin = two.getMinimum();
        const Vector3& twoMax = two.getMaximum();
        Vector3 origin = one.getOrigin();
        Vector3 dir = one.getDirection();

        Vector3 maxT(-1, -1, -1);

        int i = 0;
        for(i=0; i<3; i++ )
        {
            if( origin[i] < twoMin[i] )
            {
                inside = false;
                if( dir[i] > 0 )
                {
                    maxT[i] = (twoMin[i] - origin[i])/ dir[i];
                }
            }
            else if( origin[i] > twoMax[i] )
            {
                inside = false;
                if( dir[i] < 0 )
                {
                    maxT[i] = (twoMax[i] - origin[i]) / dir[i];
                }
            }
        }

        if( inside )
        {
            return INTERSECT;
        }
        int whichPlane = 0;
        if( maxT[1] > maxT[whichPlane])
            whichPlane = 1;
        if( maxT[2] > maxT[whichPlane])
            whichPlane = 2;

        if( ((int)maxT[whichPlane]) & 0x80000000 )
        {
            return OUTSIDE;
        }
        for(i=0; i<3; i++ )
        {
            if( i!= whichPlane )
            {
                float f = origin[i] + maxT[whichPlane] * dir[i];
                if ( f < (twoMin[i] - 0.00001f) ||
                        f > (twoMax[i] +0.00001f ) )
                {
                    return OUTSIDE;
                }
            }
        }

        return INTERSECT;

    }


    /** Checks how the axis aligned box intersects with the plane bounded volume
    */
    Intersection intersect( const PlaneBoundedVolume &one, const AxisAlignedBox &two )
    {
        // Null box?
        if (two.isNull()) return OUTSIDE;
	    // Infinite box?
	    if (two.isInfinite()) return INTERSECT;

        // Get centre of the box
        Vector3 centre = two.getCenter();
        // Get the half-size of the box
        Vector3 halfSize = two.getHalfSize();

        // For each plane, see if all points are on the negative side
        // If so, object is not visible.
        // If one or more are, it's partial.
        // If all aren't, full
        bool all_inside = true;
        PlaneList::const_iterator i, iend;
        iend = one.planes.end();
        for (i = one.planes.begin(); i != iend; ++i)
        {
            const Plane& plane = *i;

            Plane::Side side = plane.getSide(centre, halfSize);
            if(side == one.outside)
                    return OUTSIDE;
            if(side == Plane::BOTH_SIDE)
                    all_inside = false; 
        }

        if ( all_inside )
            return INSIDE;
        else
            return INTERSECT;

    }


    /** Checks how the second box intersects with the first.
    */
    Intersection intersect( const AxisAlignedBox &one, const AxisAlignedBox &two )
    {
        // Null box?
        if (one.isNull() || two.isNull()) return OUTSIDE;
	    if (one.isInfinite()) return INSIDE;
	    if (two.isInfinite()) return INTERSECT;


        const Vector3& insideMin = two.getMinimum();
        const Vector3& insideMax = two.getMaximum();

        const Vector3& outsideMin = one.getMinimum();
        const Vector3& outsideMax = one.getMaximum();

        if (    insideMax.x < outsideMin.x ||
                insideMax.y < outsideMin.y ||
                insideMax.z < outsideMin.z ||
                insideMin.x > outsideMax.x ||
                insideMin.y > outsideMax.y ||
                insideMin.z > outsideMax.z )
        {
            return OUTSIDE;
        }

        bool full = ( insideMin.x > outsideMin.x &&
                    insideMin.y > outsideMin.y &&
                    insideMin.z > outsideMin.z &&
                    insideMax.x < outsideMax.x &&
                    insideMax.y < outsideMax.y &&
                    insideMax.z < outsideMax.z );

        if ( full )
            return INSIDE;
        else
            return INTERSECT;

    }

    /** Checks how the box intersects with the sphere.
    */
    Intersection intersect( const Sphere &one, const AxisAlignedBox &two )
    {
        // Null box?
        if (two.isNull()) return OUTSIDE;
	    if (two.isInfinite()) return INTERSECT;

        float sradius = one.getRadius();

        sradius *= sradius;

        Vector3 scenter = one.getCenter();

        const Vector3& twoMin = two.getMinimum();
        const Vector3& twoMax = two.getMaximum();

        float s, d = 0;

        Vector3 mndistance = ( twoMin - scenter );
        Vector3 mxdistance = ( twoMax - scenter );

        if ( mndistance.squaredLength() < sradius &&
                mxdistance.squaredLength() < sradius )
        {
            return INSIDE;
        }

        //find the square of the distance
        //from the sphere to the box
        for ( int i = 0 ; i < 3 ; i++ )
        {
            if ( scenter[ i ] < twoMin[ i ] )
            {
                s = scenter[ i ] - twoMin[ i ];
                d += s * s;
            }

            else if ( scenter[ i ] > twoMax[ i ] )
            {
                s = scenter[ i ] - twoMax[ i ];
                d += s * s;
            }

        }

        bool partial = ( d <= sradius );

        if ( !partial )
        {
            return OUTSIDE;
        }

        else
        {
            return INTERSECT;
        }


    }
    /***************************************************/

    /** Returns true is the box will fit in a child.
    */
    bool Octree::_isTwiceSize( const AxisAlignedBox &box ) const
    {
	    // infinite boxes never fit in a child - always root node
	    if (box.isInfinite())
		    return false;

        Vector3 halfMBoxSize = mBox.getHalfSize();
        Vector3 boxSize = box.getSize();
        return ((boxSize.x <= halfMBoxSize.x) && (boxSize.y <= halfMBoxSize.y) && (boxSize.z <= halfMBoxSize.z));

    }

    /** It's assumed the the given box has already been proven to fit into
    * a child.  Since it's a loose octree, only the centers need to be
    * compared to find the appropriate node.
    */
    void Octree::_getChildIndexes( const AxisAlignedBox &box, int *x, int *y, int *z ) const
    {
        Vector3 max = mBox.getMaximum();
        Vector3 min = box.getMinimum();

        Vector3 center = mBox.getMaximum().midPoint( mBox.getMinimum() );

        Vector3 ncenter = box.getMaximum().midPoint( box.getMinimum() );

        if ( ncenter.x > center.x )
            * x = 1;
        else
            *x = 0;

        if ( ncenter.y > center.y )
            * y = 1;
        else
            *y = 0;

        if ( ncenter.z > center.z )
            * z = 1;
        else
            *z = 0;

    }

    Octree::Octree(PCZone * oz, Octree * parent ) 
        : mWireBoundingBox(0),
        mHalfSize( 0, 0, 0 )
    {
        //initialize all children to null.
        for ( int i = 0; i < 2; i++ )
        {
            for ( int j = 0; j < 2; j++ )
            {
                for ( int k = 0; k < 2; k++ )
                {
                    mChildren[ i ][ j ][ k ] = 0;
                }
            }
        }
	    mZone = oz;
        mParent = parent;
        mNumNodes = 0;
    }

    Octree::~Octree()
    {
        //initialize all children to null.
        for ( int i = 0; i < 2; i++ )
        {
            for ( int j = 0; j < 2; j++ )
            {
                for ( int k = 0; k < 2; k++ )
                {
                    if ( mChildren[ i ][ j ][ k ] != 0 )
                        delete mChildren[ i ][ j ][ k ];
                }
            }
        }

        if(mWireBoundingBox)
            delete mWireBoundingBox;

        mParent = 0;
    }

    void Octree::_addNode( PCZSceneNode * n )
    {
        mNodes.push_back( n );
        ((OctreeZoneData*)n ->getZoneData(mZone))->setOctant( this );

        //update total counts.
        _ref();

    }

    void Octree::_removeNode( PCZSceneNode * n )
    {
        mNodes.erase( std::find( mNodes.begin(), mNodes.end(), n ) );
        ((OctreeZoneData*)n ->getZoneData(mZone))->setOctant( 0 );

        //update total counts.
        _unref();
    }

    void Octree::_getCullBounds( AxisAlignedBox *b ) const
    {
        b -> setExtents( mBox.getMinimum() - mHalfSize, mBox.getMaximum() + mHalfSize );
    }

    WireBoundingBox* Octree::getWireBoundingBox()
    {
        // Create a WireBoundingBox if needed
        if(mWireBoundingBox == 0)
            mWireBoundingBox = new WireBoundingBox();

        mWireBoundingBox->setupBoundingBox(mBox);
        return mWireBoundingBox;
    }

    void Octree::_findNodes(const AxisAlignedBox &t, 
                            PCZSceneNodeList &list, 
                            PCZSceneNode *exclude, 
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AxisAlignedBox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersect( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSceneNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSceneNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
					if (it == list.end())
					{
						list.push_back( on );
					}
			    }

			    else
			    {
				    Intersection nsect = intersect( t, on -> _getWorldAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
						if (it == list.end())
						{
							list.push_back( on );
						}
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }

    void Octree::_findNodes(const Ray &t, 
                            PCZSceneNodeList &list, 
                            PCZSceneNode *exclude, 
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AxisAlignedBox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersect( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSceneNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSceneNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
					if (it == list.end())
					{
						list.push_back( on );
					}
			    }

			    else
			    {
				    Intersection nsect = intersect( t, on -> _getWorldAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
						if (it == list.end())
						{
							list.push_back( on );
						}
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }

    void Octree::_findNodes(const Sphere &t, 
                            PCZSceneNodeList &list, 
                            PCZSceneNode *exclude, 
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AxisAlignedBox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersect( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSceneNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSceneNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
					if (it == list.end())
					{
						list.push_back( on );
					}
			    }

			    else
			    {
				    Intersection nsect = intersect( t, on -> _getWorldAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
						if (it == list.end())
						{
							list.push_back( on );
						}
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }


    void Octree::_findNodes(const PlaneBoundedVolume &t, 
                            PCZSceneNodeList &list, 
                            PCZSceneNode *exclude, 
							bool includeVisitors,
                            bool full )
    {
	    if ( !full )
	    {
		    AxisAlignedBox obox;
		    _getCullBounds( &obox );

		    Intersection isect = intersect( t, obox );

		    if ( isect == OUTSIDE )
			    return ;

		    full = ( isect == INSIDE );
	    }


	    PCZSceneNodeList::iterator it = mNodes.begin();

	    while ( it != mNodes.end() )
	    {
		    PCZSceneNode * on = ( *it );

		    if ( on != exclude && (on->getHomeZone() == mZone || includeVisitors ))
		    {
			    if ( full )
			    {
					// make sure the node isn't already on the list
					PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
					if (it == list.end())
					{
						list.push_back( on );
					}
			    }

			    else
			    {
				    Intersection nsect = intersect( t, on -> _getWorldAABB() );

				    if ( nsect != OUTSIDE )
				    {
						// make sure the node isn't already on the list
						PCZSceneNodeList::iterator it = std::find( list.begin(), list.end(), on );
						if (it == list.end())
						{
							list.push_back( on );
						}
				    }
			    }

		    }
		    ++it;
	    }

	    Octree* child;

	    if ( (child=mChildren[ 0 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 0 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 0 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 0 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

	    if ( (child=mChildren[ 1 ][ 1 ][ 1 ]) != 0 )
		    child->_findNodes( t, list, exclude, includeVisitors, full );

    }

}