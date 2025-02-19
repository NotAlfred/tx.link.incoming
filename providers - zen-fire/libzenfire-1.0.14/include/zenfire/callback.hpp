
/******************************************************************************
 ******************************************************************************/

/** \file callback.hpp
 *  function callback
 *
 * $Id: callback.hpp 1411 2009-09-23 02:07:43Z grizz $
 */

#ifndef ZENFIRE_CALLBACK_HPP__
#define ZENFIRE_CALLBACK_HPP__

//############################################################################//
// L I C E N S E #############################################################//
//############################################################################//

/*
 * (C) 2008 BigWells Technology (Zen-Fire). All rights reserved. 
 * Confidential and proprietary information of BigWells Technology.
 */

//############################################################################//
// I N C L U D E S ###########################################################//
//############################################################################//

#include "platform.hpp"

namespace zenfire {

template<typename T>
struct callback
  {
  typedef sys::function<void (const T&)> function;
  };

} // namespace zenfire
//############################################################################//

#endif // end ZENFIRE_CALLBACK_HPP__

/******************************************************************************
 ******************************************************************************/

