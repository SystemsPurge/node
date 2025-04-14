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
#include<villas/hooks/base_stats.hpp>
#include <villas/timing.hpp>

using namespace villas;
using namespace node;

  StatsWriteHook::StatsWriteHook(BaseStatsHook *pa, Path *p, Node *n, int fl, int prio,
                 bool en)
      : Hook(p, n, fl, prio, en), parent(pa) {
    // This hook has no config. We never call parse() for it
    state = State::PARSED;
  }



  StatsReadHook::StatsReadHook(BaseStatsHook *pa, Path *p, Node *n, int fl, int prio,
                bool en)
      : Hook(p, n, fl, prio, en), last(nullptr), parent(pa) {
    // This hook has no config. We never call parse() for it
    state = State::PARSED;
  }

  void StatsReadHook::start() {
    assert(state == State::PREPARED);

    last = nullptr;

    state = State::STARTED;
  }

  void StatsReadHook::stop() {
    assert(state == State::STARTED);

    if (last)
      sample_decref(last);

    state = State::STOPPED;
  }

  BaseStatsHook::BaseStatsHook(Path *p, Node *n, int fl, int prio, bool en)
      : Hook(p, n, fl, prio, en),
        warmup(500), buckets(20){
    readHook = std::make_shared<StatsReadHook>(this, p, n, fl, prio, en);
    writeHook = std::make_shared<StatsWriteHook>(this, p, n, fl, prio, en);

    if (!readHook || !writeHook)
      throw MemoryAllocationError();

    // Add child hooks
    if (node) {
      node->in.hooks.push_back(readHook);
      node->out.hooks.push_back(writeHook);
    }
  }

  void BaseStatsHook::restart() {
    assert(state == State::STARTED);

    stats->reset();
  }

  Hook::Reason BaseStatsHook::process(struct Sample *smp) {
    if (!node)
      return readHook->process(smp);

    return Hook::Reason::OK;
  }

  void BaseStatsHook::prepare() {
    assert(state == State::CHECKED);
    stats = std::make_shared<villas::Stats>(buckets, warmup);

    if (node)
      node->setStats(stats);

    state = State::PREPARED;
  }


Hook::Reason StatsWriteHook::process(struct Sample *smp) {
  timespec now = time_now();

  parent->stats->update(Stats::Metric::AGE,
                        time_delta(&smp->ts.received, &now));

  return Reason::OK;
}

Hook::Reason StatsReadHook::process(struct Sample *smp) {
  if (last) {
    if (smp->flags & last->flags & (int)SampleFlags::HAS_TS_RECEIVED)
      parent->stats->update(Stats::Metric::GAP_RECEIVED,
                            time_delta(&last->ts.received, &smp->ts.received));

    if (smp->flags & last->flags & (int)SampleFlags::HAS_TS_ORIGIN)
      parent->stats->update(Stats::Metric::GAP_SAMPLE,
                            time_delta(&last->ts.origin, &smp->ts.origin));

    if ((smp->flags & (int)SampleFlags::HAS_TS_ORIGIN) &&
        (smp->flags & (int)SampleFlags::HAS_TS_RECEIVED))
      parent->stats->update(Stats::Metric::OWD,
                            time_delta(&smp->ts.origin, &smp->ts.received));

    if (smp->flags & last->flags & (int)SampleFlags::HAS_SEQUENCE) {
      int dist = smp->sequence - (int32_t)last->sequence;
      if (dist != 1)
        parent->stats->update(Stats::Metric::SMPS_REORDERED, dist);
    }
  }

  parent->stats->update(Stats::Metric::SIGNAL_COUNT, smp->length);

  sample_incref(smp);

  if (last)
    sample_decref(last);

  last = smp;

  return Reason::OK;
}

