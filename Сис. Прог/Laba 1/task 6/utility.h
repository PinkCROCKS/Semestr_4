#pragma once

#include <libgen.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "../task 1/errors.h"

errors init(int* msg_id_1, int* msg_id_2);
