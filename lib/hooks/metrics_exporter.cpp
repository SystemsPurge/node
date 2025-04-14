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
#include <villas/node/exceptions.hpp>
#include<villas/metrics_exporter.hpp>
#include<villas/hooks/metrics_exporter.hpp>
#include <villas/timing.hpp>

namespace villas{

  namespace node{
    
  StatsExporterHook::StatsExporterHook(Path *p, Node *n, int fl, int prio, bool en)
      : BaseStatsHook(p, n, fl, prio, en),metrics_exporter(MetricsExporter()){
  }

  void StatsExporterHook::start() {
    assert(state == State::PREPARED);
    metrics_exporter.check_family(port);
    metrics_exporter.register_node(node);
    state = State::STARTED;
  }

  void StatsExporterHook::stop() {
    assert(state == State::STARTED);
    metrics_exporter.shutdown();

    state = State::STOPPED;
  }

  void StatsExporterHook::parse(json_t *json) {
    int ret;
    json_error_t err;

    assert(state != State::STARTED);
  
    Hook::parse(json);

    ret = json_unpack_ex(json, &err, 0, "{ s?: i }","port", &port);
    if (ret)
      throw ConfigError(json, err, "node-config-hook-stats");
    state = State::PARSED;
  }

  void StatsExporterHook::periodic(){
    
    assert(state == State::STARTED);
    metrics_exporter.loop_instruction(stats);
  }

  }
}

