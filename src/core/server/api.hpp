#ifndef __SERVER_API_HPP_
#define __SERVER_API_HPP_

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../pch.hpp"

#include "task/cbComputeGraph.hpp"
#include "trivial/cbVirtualDevice.hpp"

namespace cb {
namespace apiCPP {

cb::graph::cbComputeGraph* addGraph(int32_t idx, const std::string& cmd,
                                    trivial::cbVirtualDeviceManager* vdm);

}  // namespace apiCPP
}  // namespace cb

#endif  //! __SERVER_API_HPP_