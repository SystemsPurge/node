/* Statistic hooks.
 *
 * Author: Steffen Vogel <post@steffenvogel.de>
 * SPDX-FileCopyrightText: 2014-2023 Institute for Automation of Complex Power Systems, RWTH Aachen University
 * SPDX-License-Identifier: Apache-2.0
 */

#include<villas/metrics_exporter.hpp>
#include<villas/node.hpp>
#include<villas/utils.hpp>
#include <villas/log.hpp>
using namespace villas ;
using namespace node ;


prometheus::Exposer* MetricsExporter::exposer;

std::shared_ptr<prometheus::Registry> MetricsExporter::metrics_registry;

prometheus::Family<prometheus::Gauge>* MetricsExporter::family;

std::unordered_map<Stats::Metric,std::string> MetricsExporter::metrics_subset = {
    {Stats::Metric::SMPS_SKIPPED,"skipped"},
    {Stats::Metric::OWD,"owd"},
    {Stats::Metric::AGE,"age"},
    {Stats::Metric::SIGNAL_COUNT,"signalcnt"},
    {Stats::Metric::RTP_LOSS_FRACTION,"rtplossfraction"}
};

std::unordered_map<Stats::Type, std::string> MetricsExporter::type_subset = {
    {Stats::Type::MEAN, "mean"},
    {Stats::Type::STDDEV, "stddev"},
    {Stats::Type::TOTAL, "total"}
};

MetricsExporter::MetricsExporter(){
    logger = Log::get("metrics-exporter");
    logger->debug("Creating metrics exporter.");
}


void MetricsExporter::reset(){

}


void MetricsExporter::check_family(int port){
    logger->debug("Checking family existence.");
    if(!MetricsExporter::exposer){
        MetricsExporter::exposer = new prometheus::Exposer{"0.0.0.0:"+std::to_string(port)};
        MetricsExporter::metrics_registry = std::make_shared<prometheus::Registry>();
        exposer->RegisterCollectable(metrics_registry);
        MetricsExporter::family = &prometheus::BuildGauge().Name("message_metrics").Register(*MetricsExporter::metrics_registry);
    }
}


void MetricsExporter::loop_instruction(std::shared_ptr<Stats> stats){
    for(auto& metric:metrics_subset){
        Hist hist = stats->getHistogram(metric.first);
        std::string name = metric.second;
        gauges.at(name+"_total").Set(hist.getTotal());
        gauges.at(name+"_mean").Set(hist.getMean());
        gauges.at(name+"_stddev").Set(hist.getStddev());
    }
}


void MetricsExporter::shutdown(){ 
    logger->debug("Shutting down metrics exporter.");
    if(MetricsExporter::exposer){
        for(auto& gauge: gauges){
            MetricsExporter::family->Remove(&gauge.second);
        }
        if(MetricsExporter::family->GetConstantLabels().size() == 0){ 
            logger->debug("Family empty, destroying.");
            MetricsExporter::metrics_registry->Remove(*family);
        }
        //destroy explicitely, until figure out how to STOP LISTENING
        MetricsExporter::exposer->~Exposer();
    }
}


void MetricsExporter::register_node(node::Node *n){
    std::string node_name = n->getName();
    node_name = node_name.substr(0,node_name.find('('));
    logger->debug("Registering node "+node_name);
    for(auto& metric: metrics_subset){
        for (auto& type_name:type_subset){
            std::string full_metric_name = metric.second+"_"+type_name.second;
            logger->debug("Registered "+full_metric_name);
            gauges.emplace(full_metric_name,MetricsExporter::family->Add({{full_metric_name,node_name}}));
        }
    }
}
