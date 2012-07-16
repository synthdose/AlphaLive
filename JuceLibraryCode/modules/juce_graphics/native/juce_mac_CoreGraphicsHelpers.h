/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_MAC_COREGRAPHICSHELPERS_JUCEHEADER__
#define __JUCE_MAC_COREGRAPHICSHELPERS_JUCEHEADER__


//==============================================================================
namespace
{
    template <class RectType>
    Rectangle<int> convertToRectInt (const RectType& r) noexcept
    {
        return Rectangle<int> ((int) r.origin.x, (int) r.origin.y, (int) r.size.width, (int) r.size.height);
    }

    template <class RectType>
    Rectangle<float> convertToRectFloat (const RectType& r) noexcept
    {
        return Rectangle<float> (r.origin.x, r.origin.y, r.size.width, r.size.height);
    }

    template <class RectType>
    CGRect convertToCGRect (const RectType& r) noexcept
    {
        return CGRectMake ((CGFloat) r.getX(), (CGFloat) r.getY(), (CGFloat) r.getWidth(), (CGFloat) r.getHeight());
    }
}

extern CGImageRef juce_createCoreGraphicsImage (const Image&, const bool forAlpha, CGColorSpaceRef, const bool mustOutliveSource);

extern CGContextRef juce_getImageContext (const Image&);

#endif   // __JUCE_MAC_COREGRAPHICSHELPERS_JUCEHEADER__
