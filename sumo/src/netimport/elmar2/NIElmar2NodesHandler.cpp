/***************************************************************************
                          NIElmar2NodesHandler.cpp
             A LineHandler-derivate to load nodes form a elmar-nodes-file
                             -------------------
    project              : SUMO
    begin                : Sun, 16 May 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2006/09/18 10:11:38  dkrajzew
// changed the way geocoordinates are processed
//
// Revision 1.10  2006/04/18 08:05:45  dkrajzew
// beautifying: output consolidation
//
// Revision 1.9  2006/04/07 05:28:24  dkrajzew
// removed some warnings
//
// Revision 1.8  2006/03/27 07:30:20  dkrajzew
// added projection information to the network
//
// Revision 1.7  2006/03/17 11:03:06  dkrajzew
// made access to positions in Position2DVector c++ compliant
//
// Revision 1.6  2006/03/08 13:02:27  dkrajzew
// some further work on converting geo-coordinates
//
// Revision 1.5  2006/01/19 09:26:04  dkrajzew
// adapted to the current version
//
// Revision 1.4  2005/10/07 11:39:36  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/23 06:02:24  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.2  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/07/14 11:05:28  dkrajzew
// elmar unsplitted import added
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIElmar2NodesHandler.h"
#include <utils/geoconv/GeoConvHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NIElmar2NodesHandler::NIElmar2NodesHandler(NBNodeCont &nc,
                                           const std::string &file,
                                           std::map<std::string, Position2DVector> &geoms)
    : FileErrorReporter("elmar-nodes", file),
    myInitX(-1), myInitY(-1),
    myNodeCont(nc), myGeoms(geoms)
{
}


NIElmar2NodesHandler::~NIElmar2NodesHandler()
{
}


bool
NIElmar2NodesHandler::report(const std::string &result)
{
    if(result[0]=='#') {
        return true;
    }
    string id;
    SUMOReal x, y;
    int no_geoms, intermediate;
    StringTokenizer st(result, StringTokenizer::WHITECHARS);
    // check
    if(st.size()<5) {
        MsgHandler::getErrorInstance()->inform("Something is wrong with the following data line\n" + result);
        throw ProcessError();
    }
    // parse
        // id
    id = st.next();
        // intermediate?
    try {
        intermediate = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical value for internmediate y/n occured.");
        throw ProcessError();
    }
        // number of geometrical information
    try {
        no_geoms = TplConvert<char>::_2int(st.next().c_str());
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical value for number of nodes occured.");
        throw ProcessError();
    }
        // geometrical information
    Position2DVector geoms;
    for(int i=0; i<no_geoms; i++) {
        try {
            x = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("Non-numerical value for node-x-position occured.");
            throw ProcessError();
        }
        try {
            y = (SUMOReal) TplConvert<char>::_2SUMOReal(st.next().c_str());
        } catch (NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform("Non-numerical value for node-y-position occured.");
            throw ProcessError();
        }

        myNodeCont.addGeoreference(Position2D((SUMOReal) (x / 100000.0), (SUMOReal) (y / 100000.0)));
        Position2D pos(x, y);
        GeoConvHelper::remap(pos);
        geoms.push_back(pos);
    }

    if(intermediate==0) {
        NBNode *n = new NBNode(id, geoms[0]);
        if(!myNodeCont.insert(n)) {
            delete n;
            MsgHandler::getErrorInstance()->inform("Could not add node '" + id + "'.");
            throw ProcessError();
        }
    } else {
        myGeoms[id] = geoms;
    }
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


