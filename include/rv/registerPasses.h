//===- rv/PlatformInfo.h - target info&function environment --*- C++ -*-===//
//
// Part of the RV Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Callback into libRV for the RVPLUG passplug in populate the new PM.
//
//===----------------------------------------------------------------------===//

#include "llvm/Passes/PassPlugin.h"

#ifndef RV_REGISTERPASSES_H
#define RV_REGISTERPASSES_H

namespace rv {
  void addRVPasses(llvm::PassBuilder & PB);
}

#endif // RV_REGISTERPASSES_H
