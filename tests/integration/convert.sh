#!/bin/bash
#
# Integration test for villas convert tool
#
# @author Steffen Vogel <stvogel@eonerc.rwth-aachen.de>
# @copyright 2014-2022, Institute for Automation of Complex Power Systems, EONERC
# @license GNU General Public License (version 3)
#
# VILLASnode
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##################################################################################

set -e

DIR=$(mktemp -d)
pushd ${DIR}

function finish {
	popd
	rm -rf ${DIR}
}
trap finish EXIT

FORMATS="villas.human csv tsv json opal.asyncip"

villas signal -v5 -n -l20 mixed > input.dat

for FORMAT in ${FORMATS}; do
	villas convert -o ${FORMAT} < input.dat | tee ${TEMP} | \
	villas convert -i ${FORMAT} > output.dat

	CMP_FLAGS=""
	if [ ${FORMAT} = "opal.asyncip" ]; then
		CMP_FLAGS+=-T
	fi

	villas compare ${CMP_FLAGS} input.dat output.dat
done
