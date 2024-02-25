/////////////////////////////////////////////////////////////////////////////////////////
//
// gro is protected by the UW OPEN SOURCE LICENSE, which is summarized here.
// Please see the file LICENSE.txt for the complete license.
//
// THE SOFTWARE (AS DEFINED BELOW) AND HARDWARE DESIGNS (AS DEFINED BELOW) IS PROVIDED
// UNDER THE TERMS OF THIS OPEN SOURCE LICENSE (“LICENSE”).  THE SOFTWARE IS PROTECTED
// BY COPYRIGHT AND/OR OTHER APPLICABLE LAW.  ANY USE OF THIS SOFTWARE OTHER THAN AS
// AUTHORIZED UNDER THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
//
// BY EXERCISING ANY RIGHTS TO THE SOFTWARE AND/OR HARDWARE PROVIDED HERE, YOU ACCEPT AND
// AGREE TO BE BOUND BY THE TERMS OF THIS LICENSE.  TO THE EXTENT THIS LICENSE MAY BE
// CONSIDERED A CONTRACT, THE UNIVERSITY OF WASHINGTON (“UW”) GRANTS YOU THE RIGHTS
// CONTAINED HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF SUCH TERMS AND CONDITIONS.
//
// TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION
//
//
#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "ut/defines.hpp"


namespace rsc
{
  //resources
	constexpr const char* RESOURCE_ERRORIMAGE = ":/images/images/error.tiff";
	constexpr const char* RESOURCE_START_ICON = ":/icons/icons/start.png";
	constexpr const char* RESOURCE_STOP_ICON = ":/icons/icons/stop.png";
  //files
	constexpr const char* IO_DF_WORKINGDIR = "../Files/";
  //messages
	constexpr const char* IO_SPECERROR_MSG = "Please edit the program to fix the error and try opening it again.";

	constexpr const char* IO_ABOUT_TITLE = "<h3>gro 3.0: The flexible and friendly multicellular simulator</h3>Version alpha.3.0<br />";
	constexpr const char* IO_ABOUT_AUTHOR = "Programmed by Elena Núñez Berrueco, Universidad Politécnica de Madrid, Spain<br />";
	constexpr const char* IO_ABOUT_ATTRIBUTIONS = "Based on previous versions from Eric Klavins, University of Washington, Seattle, WA, USA and Gutiérrez et al, Universidad Politécnica de Madrid, Spain<br />";
	constexpr const char* IO_ABOUT_LICENSE = "Copyright © 2011-2012, University of Washington (GNU V. 2)";
	constexpr const char* IO_ABOUT_LINK = "<br />";
	constexpr const char* IO_OPEN_DOC_LINK = "open http://depts.washington.edu/soslab/gro/docview.html";
}

  
//debugging
#define DBG {                                             \
    printf ( "Debug statement: reached %s, line %d\n",    \
                    __FILE__, __LINE__);                  \
    fflush ( stdout );                                    \
}

#define ASSERT(_cond_) {                                                          \
  if ( ! ( _cond_ ) ) {                                                           \
    fprintf (stderr, "Internal error: assertion '%s' failed at %s, line %d\n",    \
                   #_cond_ , __FILE__, __LINE__);                                 \
    exit ( -1 );                                                                  \
  }                                                                               \
}

#define ASSERT_MSG(_cond_,_msg_...) {                                             \
  if ( ! ( _cond_ ) ) {                                                           \
    fprintf ( stderr, "Internal error: assertion '%s' failed at %s, line %d\n",   \
                   #_cond_ , __FILE__, __LINE__);                                 \
    fprintf ( stderr, _msg_ );                                                    \
    exit ( -1 );                                                                  \
  }                                                                               \
}

#endif //_DEFINES_H_
