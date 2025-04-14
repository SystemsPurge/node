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
#include <villas/stats.hpp>
#include<villas/hooks/stats.hpp>
#include <villas/timing.hpp>

using namespace villas;
using namespace node;

  StatsHook::StatsHook(Path *p, Node *n, int fl, int prio, bool en)
      : BaseStatsHook(p, n, fl, prio, en), format(Stats::Format::HUMAN), verbose(0)
      , output(nullptr), uri() {

  }


  void StatsHook::start() {
    assert(state == State::PREPARED);

    if (!uri.empty()) {
      output = fopen(uri.c_str(), "w+");
      if (!output)
        throw RuntimeError("Failed to open file '{}' for writing", uri);
    }

    state = State::STARTED;
  }

  void StatsHook::stop() {
    assert(state == State::STARTED);

    stats->print(uri.empty() ? stdout : output, format, verbose);

    if (!uri.empty())
      fclose(output);

    state = State::STOPPED;
  }

  void StatsHook::periodic() {
    assert(state == State::STARTED);

    stats->printPeriodic(uri.empty() ? stdout : output, format, node);
  }

  void StatsHook::parse(json_t *json) {
    int ret;
    json_error_t err;

    assert(state != State::STARTED);

    Hook::parse(json);

    const char *f = nullptr;
    const char *u = nullptr;

    ret = json_unpack_ex(json, &err, 0, "{ s?: s, s?: b, s?: i, s?: i, s?: s }",
                         "format", &f, "verbose", &verbose, "warmup", &warmup,
                         "buckets", &buckets, "output", &u);
    if (ret)
      throw ConfigError(json, err, "node-config-hook-stats");

    if (f) {
      try {
        format = Stats::lookupFormat(f);
      } catch (const std::invalid_argument &e) {
        throw ConfigError(json, "node-config-hook-stats",
                          "Invalid statistic output format: {}", f);
      }
    }

    if (u)
      uri = u;

    state = State::PARSED;
  }