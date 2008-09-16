/* $Id$ */
#ifndef __RO_H
#define __RO_H
/** \file ro.h
 * This is the main header for the Interface Module.
 * All projects that needs this module, should include this file, and this file only.
 */

/** \mainpage Open-Ragnarok Project
 *
 * \section intro_sec Introduction
 *
 * Welcome to the Open-Ragnarok project. This project has a goal to be a full replacement for the official Ragnarok Client.
 * It consists in 4 main projects. The interface, the GL, the Network and the Client.
 *
 * \section interface_sec The Interface
 *
 * \subsection step1 Step 1: Opening the box
 *  
 * \section gl_sec The Graphics Library
 *
 * \section net_sec The Network
 *
 * \section client_sec The Client
 */


#define GRF_HEADER "Master of Magic"
#define SPR_HEADER "SP"
#define ACT_HEADER "AC"
#define RSM_HEADER "GRSM"
#define RSW_HEADER "GRSW"
#define GND_HEADER "GRGN"
#define GAT_HEADER "GRAT"

#include "impdecl.h"

/** \namespace RO Main namespace for the Interface.
 * 
 */
namespace RO {
	MYLIB_DLLAPI unsigned short Version();
	MYLIB_DLLAPI void printVersion();
}

#include "ro_strings.h"
#include "constants.h"
#include "object.h"
#include "grf.h"
#include "spr.h"
#include "act.h"
#include "rsm.h"
#include "rsw.h"
#include "gnd.h"
#include "gat.h"

/**
 * \defgroup ROInterface Interface
 */


#endif // __RO_H
