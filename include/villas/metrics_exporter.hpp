/* Statistic collection.
 *
 * Author: Steffen Vogel <post@steffenvogel.de>
 * SPDX-FileCopyrightText: 2014-2023 Institute for Automation of Complex Power Systems, RWTH Aachen University
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include<unordered_map>
#include<villas/stats.hpp>
#include<villas/log.hpp>
#include<prometheus/exposer.h>
#include<prometheus/registry.h>
#include<prometheus/gauge.h>
namespace villas {

  class Stats;
  namespace node {

  struct Sample;
  class Node;
  
  } 


  class MetricsExporter {

    private:
      Logger logger;
      static std::unordered_map<Stats::Metric, std::string> metrics_subset;
      static std::unordered_map<Stats::Type,std::string> type_subset;
      static prometheus::Exposer* exposer;
      static std::shared_ptr<prometheus::Registry> metrics_registry;
      static std::unordered_map<std::string, prometheus::Family<prometheus::Gauge>&> families;
      std::unordered_map<std::string, prometheus::Gauge&>gauges;
    public:
      MetricsExporter();
      void reset();
      void register_node(int port,node::Node *n);
      void loop_instruction(std::shared_ptr<Stats> stats);
      void shutdown();
  };

} // namespace villas
