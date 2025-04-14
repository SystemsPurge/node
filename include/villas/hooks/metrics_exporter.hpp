/* Statistic hooks.
 *
 * Author: Steffen Vogel <post@steffenvogel.de>
 * SPDX-FileCopyrightText: 2014-2023 Institute for Automation of Complex Power Systems, RWTH Aachen University
 * SPDX-License-Identifier: Apache-2.0
 */

#include <memory>

#include <villas/common.hpp>
#include <villas/hook.hpp>
#include <villas/node.hpp>
#include<villas/metrics_exporter.hpp>
#include <villas/node/exceptions.hpp>
#include <villas/hooks/base_stats.hpp>
#include <villas/timing.hpp>

namespace villas {
namespace node {
class BaseStatsHook;
class StatsExporterHook :public BaseStatsHook{

private:
    MetricsExporter metrics_exporter;
    int port;
public:
  
  StatsExporterHook(Path *p, Node *n, int fl, int prio, bool en = true);

  void start();

  void stop();

  void parse(json_t *json);

  void periodic();
};
  char n[] = "metrics";
  char d[] = "Collect statistics for the current node";
  HookPlugin<StatsExporterHook, n, d, (int)Hook::Flags::NODE_READ> p;
} // namespace node
} // namespace villas
