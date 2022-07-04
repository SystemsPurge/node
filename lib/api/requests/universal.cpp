/** Universal Data-exchange API request
 *
 * @file
 * @author Steffen Vogel <svogel2@eonerc.rwth-aachen.de>
 * @copyright 2014-2022, Institute for Automation of Complex Power Systems, EONERC
 * @license Apache 2.0
 *********************************************************************************/

#include <villas/api/requests/universal.hpp>

using namespace villas::node;
using namespace villas::node::api;

void UniversalRequest::prepare()
{
	NodeRequest::prepare();

	api_node = dynamic_cast<APINode*>(node);
	if (!api_node)
		throw BadRequest("Node {} is not an univeral API node!", node->getNameShort());
}
