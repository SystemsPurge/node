#!/bin/bash
#
# Integration test for remote API
#
# @author Steffen Vogel <svogel2@eonerc.rwth-aachen.de>
# @copyright 2014-2022, Institute for Automation of Complex Power Systems, EONERC
# @license Apache 2.0
##################################################################################

set -e

DIR=$(mktemp -d)
pushd ${DIR}

function finish {
	popd
	rm -rf ${DIR}
}
trap finish EXIT

cat > config.json <<EOF
{
	"http": {
		"port": 8080
	}
}
EOF

# Start without a configuration
villas node config.json &

# Wait for node to complete init
sleep 1

# Fetch capabilities
curl -s http://localhost:8080/api/v2/capabilities > fetched.json

kill %%
wait %%

jq -e '.apis | index( "capabilities" ) != null' < fetched.json
