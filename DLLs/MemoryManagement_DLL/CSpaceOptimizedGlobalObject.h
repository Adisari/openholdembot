//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: base class for global objects 
//   that use memory-pools
//
//******************************************************************************

#pragma once

#include <assert.h>
#include "CMemoryPool.h"

class CSpaceOptimizedGlobalObject {
 public:
  CSpaceOptimizedGlobalObject()  {}
  ~CSpaceOptimizedGlobalObject() {}
 public:
  void* operator new(size_t size) {
    assert(MemoryPoolGlobal() != NULL);
    return MemoryPoolGlobal()->Allocate(size);
  }
 public:
  void operator delete(void* ptr) {}
};