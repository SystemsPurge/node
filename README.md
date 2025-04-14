# Usage.
Build with -DWITH_METRICS for a prometheus exporter hook.
Alternatively use image soullessblob/villas-node:prometheus the same way one would use the official image.
Metrics follow the prometheus best practices, and has the form `<metric_name>{node=<node_short_name>, accumulator=<stat_type_name>}`, with stat_type_name originating from Stats::types.
Only a subset of accumulators/metrics for now.

# Example docker compose.

```
services:
  node:
    image: soullessblob/villas-node:prometheus
    hostname: node-prom
    container_name: node-prom
    command: ["node","-d","debug","/configs/test-conf.conf"]
    volumes:
      - ./test-conf.conf:/configs/test-conf.conf
    ports:
      - 9096:9096
      - 80:80

  prom:
    image: prom/prometheus
    hostname: prom
    container_name: prom
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml
    ports:
      - 9090:9090
```

With node config:
```
{
  "http": {
    "port": 8080
  },
  "hugepages": 0,
  "nodes": {
    "siggen": {
      "type": "signal",
      "signal": [
        "sine",
        "sine"
      ],
      "rate": 10,
      "values": 2
    },
    "relay": {
      "type": "websocket",
      "destinations": [
        "https://s-dev.k8s.eonerc.rwth-aachen.de/ws/relay/hyperride-test"
      ],
       "in":{
          "hooks":[
            {
              "type":"metrics",
              "port":9096
            }
          ]
            
        }
    }
  },
  "paths": [
    {
      "in": "siggen",
      "out": "relay"
    }
  ],
  "logging": {
    "level": "info"
  },
  "uuid": "b3b45caf-c560-4d9c-9f66-4b9538a0a07a"
}
```

And a static prometheus config for testing: 
```
global:
  scrape_interval:     15s

  external_labels:
    monitor: 'codelab-monitor'

scrape_configs:
  - job_name: 'prometheus'

    scrape_interval: 5s

    static_configs:
      - targets: ['node-prom:9096']
```
