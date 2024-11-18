#pragma once

#include "pointers.h"
#include "spawning.h"
#include "constants.h"
#include <string>


void function_inject_into_chat();
void set_up_hook();
void our_chat_function(char* userinput, char* playerObject);