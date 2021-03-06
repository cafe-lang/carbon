//------------------------------------------------------------------------------
// MIT License
//------------------------------------------------------------------------------
// 
// Copyright (c) 2020-2021 Thakee Nathees
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

#define DOCTEST_CONFIG_IMPLEMENT
#include "carbon_tests.h"

#define CARBON_INCLUDE_CRASH_HANDLER_MAIN
#define CARBON_CRASH_HANDLER_IMPLEMENTATION
#include "../src/main/crash_handler.h"

int _main(int argc, char** argv) {

	carbon_initialize();

	// to run the tests directly from the bin/
	if (!Path("tests/test_files").exists() && Path("../tests/test_files").exists()) {
		OS::chdir("..");
	}

	doctest::Context context;
	// !!! THIS IS JUST AN EXAMPLE SHOWING HOW DEFAULTS/OVERRIDES ARE SET !!!

	// defaults
	context.addFilter("test-case-exclude", "[native_classes:dylib]");    // exclude test cases
	context.setOption("rand-seed", 324);     // if order-by is set to "rand" use this seed
	context.setOption("order-by", "file");   // sort the test cases by file and line

	context.applyCommandLine(argc, argv);

	// overrides
	context.setOption("no-breaks", true); // don't break in the debugger when assertions fail

	int test_result = context.run(); // run queries, or run tests unless --no-run is specified

	if (!context.shouldExit())   // important - query flags (and --exit) rely on the user doing this
		context.clearFilters();  // removes all filters added up to this point

	Logger::log("\n===============================================================================\n", Logger::VERBOSE, Console::Color::L_YELLOW);
	if (test_result) Logger::log("Tests failed, press enter to exit...\n", Logger::VERBOSE, Console::Color::L_RED);
	else Logger::log("Tests passed, press enter to exit...\n", Logger::VERBOSE, Console::Color::L_GREEN);
	Logger::log("===============================================================================\n", Logger::VERBOSE, Console::Color::L_YELLOW);

	carbon_cleanup();
	getchar(); // pause
	return test_result;
}

