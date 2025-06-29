#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "minion.h"
#include <functional>

using function_handler = std::function<void(minion::MList *)>;
extern std::unordered_map<std::string, function_handler> function_map;

void do_commands(minion::MList *dolist);

#endif // FUNCTIONS_H
