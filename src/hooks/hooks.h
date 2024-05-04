#pragma once

#include "../std.h"

struct hooks {
	void initialize();
	void undo();
};

extern hooks g_hooks;