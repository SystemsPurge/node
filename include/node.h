/** Nodes
 *
 * The S2SS server connects multiple nodes.
 * There are multiple types of nodes:
 *  - simulators
 *  - servers
 *  - workstations
 *
 * @author Steffen Vogel <stvogel@eonerc.rwth-aachen.de>
 * @copyright 2014, Institute for Automation of Complex Power Systems, EONERC
 * @file node.h
 */

#ifndef _NODE_H_
#define _NODE_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <libconfig.h>

/** The type of a node.
 *
 * This type is used to determine the message format of the remote node
 */
enum node_type
{
	NODE_INVALID,
	NODE_SERVER,
	NODE_WORKSTATION,
	NODE_SIM_OPAL,
	NODE_SIM_RTDS,
	NODE_SIM_DSP
};

/** The datastructure for a node.
 *
 * Every entity which exchanges messages is represented by a node.
 */
struct node
{
	/** A system-wide unique id per node */
	int id;

	/** The socket descriptor */
	int sd;

	/** A short identifier of the node */
	const char *name;

	/** The type of this node */
	enum node_type type;

	/** Local address of the socket */
	struct sockaddr_in local;
	/** Remote address of the socket */
	struct sockaddr_in remote;

	/** The egress interface */
	struct interface *interface;

	/** Socket mark for netem, routing and filtering */
	int mark;

	/** A pointer to the libconfig object which instantiated this node */
	config_setting_t *cfg;

	/** Linked list pointer */
	struct node *next;
};

/** Connect and bind the UDP socket of this node.
 *
 * @param n A pointer to the node structure
 * @return
 *  - 0 on success
 *  - otherwise on error occured
 */
int node_connect(struct node *n);

/** Disconnect the UDP socket of this node.
 *
 * @param n A pointer to the node structure
 * @return
 *  - 0 on success
 *  - otherwise on error occured
 */
int node_disconnect(struct node *n);

/** Lookup node type from string.
 *
 * @param str The string containing the node type
 * @return The node type enumeration value
 */
enum node_type node_lookup_type(const char *str);

/** Search list of nodes for a name.
 *
 * @param str The name of the wanted node
 * @param nodes A linked list of all nodes
 * @return A pointer to the node or NULL if not found
 */
struct node* node_lookup_name(const char *str, struct node *nodes);

#endif /* _NODE_H_ */
