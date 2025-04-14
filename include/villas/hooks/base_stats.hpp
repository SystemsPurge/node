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
#include <villas/timing.hpp>

namespace villas {
namespace node {
class BaseStatsHook;
class StatsWriteHook : public Hook {

protected:
  BaseStatsHook *parent;
public:
  StatsWriteHook(BaseStatsHook *pa, Path *p, Node *n, int fl, int prio, bool en = true);

  virtual Hook::Reason process(struct Sample *smp);
};

class StatsReadHook : public Hook {

protected:
  struct Sample *last;

  BaseStatsHook *parent;

public:
  StatsReadHook(BaseStatsHook *pa, Path *p, Node *n, int fl, int prio,bool en = true);

  virtual void start();

  virtual void stop();

  virtual Hook::Reason process(struct Sample *smp);
};

class BaseStatsHook:public Hook{
  
  friend StatsReadHook;
  friend StatsWriteHook;

protected:
  std::shared_ptr<Stats> stats;
  int warmup;
  int buckets;
  std::shared_ptr<StatsReadHook> readHook;
  std::shared_ptr<StatsWriteHook> writeHook;
public:
  BaseStatsHook(Path *p, Node *n, int fl, int prio, bool en = true);
  virtual void restart();

  virtual Hook::Reason process(struct Sample *smp);
  
  virtual void prepare();
};
} // namespace node
} // namespace villas
