/* Statistic hooks.
 *
 * Author: Steffen Vogel <post@steffenvogel.de>
 * SPDX-FileCopyrightText: 2014-2023 Institute for Automation of Complex Power Systems, RWTH Aachen University
 * SPDX-License-Identifier: Apache-2.0
 */

#include <memory>

#include <villas/common.hpp>
#include <villas/hook.hpp>
#include<villas/hooks/base_stats.hpp>
#include <villas/node.hpp>
#include <villas/node/exceptions.hpp>
#include <villas/stats.hpp>
#include <villas/timing.hpp>

namespace villas {
namespace node {
class BaseStatsHook;
class StatsHook : public BaseStatsHook {


  enum Stats::Format format;
  int verbose;

  FILE *output;
  std::string uri;

public:
  StatsHook(Path *p, Node *n, int fl, int prio, bool en = true);
  StatsHook &operator=(const StatsHook &) = delete;
  StatsHook(const StatsHook &) = delete;

  virtual void start();

  virtual void stop();

  virtual void periodic();

  virtual void parse(json_t *json);
};


// Register hook
static char n[] = "stats";
static char d[] = "Collect statistics for the current node";
static HookPlugin<StatsHook, n, d, (int)Hook::Flags::NODE_READ> p;

} // namespace node
} // namespace villas
