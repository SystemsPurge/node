/** Delete semaphores.
 *
 * @author Steffen Vogel <svogel2@eonerc.rwth-aachen.de>
 * @copyright 2014-2022, Institute for Automation of Complex Power Systems, EONERC
 * @license Apache 2.0
 *********************************************************************************/

#include <semaphore.h>
#include <iostream>

int main(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++) {
		int ret = sem_unlink(argv[i]);
		if (ret)
			std::cerr << "Failed to unlink: " << argv[i] << std::endl;
	}

	return 0;
}
