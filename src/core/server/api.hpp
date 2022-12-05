/**
 * @file api.hpp
 * @author chenghua Wang (chenghua.wang.edu@gmail.com)
 * @brief The C plus plus API section.
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __SERVER_API_HPP_
#define __SERVER_API_HPP_

#include "wfrest/HttpMsg.h"
#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif
#endif  //! _WIN32

#if defined(__unix__) && defined(__clang__)
#pragma once
#endif  //! defined(__unix__) && defined(__clang__)

#include "../pch.hpp"

#include <wfrest/json.hpp>

#include "task/cbComputeGraph.hpp"
#include "trivial/cbVirtualDevice.hpp"
using namespace wfrest;
namespace cb {
namespace apiCPP {

cb::graph::cbComputeGraph* addGraph(int32_t idx, const std::string& cmd,
                                    trivial::cbVirtualDeviceManager* vdm);

}  // namespace apiCPP
}  // namespace cb

namespace cb {
namespace trans {
void outbase(HttpResp* resp);
void outDeviceNode(HttpResp* resp, trivial::cbMySqlDevice* deviceNode, int node_num,
                   int device_num);
void outOpNode(int nodenum, int inputnum, HttpResp* resp, int op_num);
void outFinNode(int device_num, int op_num, HttpResp* resp);
void Node_leaf_connect(int nodeleftnum, int noderightnum, int nowinputsnum, HttpResp* resp);
void Node_op_connect(int nodeleftnum, int nowinputsnum, HttpResp* resp);
void outbaseo(HttpResp* resp);
}  // namespace trans
}  // namespace cb
#endif  //! __SERVER_API_HPP_