/*
--------------------------------------------------------------------------------
This source file is part of Hydrax.
Visit ---

Copyright (C) 2008 Xavier Vergu�n Gonz�lez <xavierverguin@hotmail.com>
                                           <xavyiy@gmail.com>

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
--------------------------------------------------------------------------------
*/

#ifndef _Hydrax_Prerequisites_H_
#define _Hydrax_Prerequisites_H_

/// Include external headers
#include <Ogre.h>

/// Define the dll export qualifier if compiling for Windows
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   #ifdef HYDRAX_LIB
     #define DllExport __declspec (dllexport)
   #elif HYDRAX_LIB_CLIENT
     #define DllExport __declspec (dllimport)
    #else
	#define DllExport
   #endif
#else
   #define DllExport
#endif

/// Log macro
#define HydraxLOG(msg) Ogre::LogManager::getSingleton().logMessage("[Hydrax] " + Ogre::String(msg));

/// Hydrax defines
#define HYDRAX_VERSION_MAJOR 0
#define HYDRAX_VERSION_MINOR 5
#define HYDRAX_VERSION_PATCH 1

#define HYDRAX_RESOURCE_GROUP "Hydrax"

#define HYDRAX_IMAGE_CHECK_PIXELS 0 // See Image.cpp, 1 = Check pixels / 0 = No check pixels
                                    // Use it for debug mode only

#endif
