#include <iostream>

#ifndef _TESTS

int main(int argc, char **argv)
{
	if (argc < 3)
	{

	}

	return EXIT_SUCCESS;
}

#else

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#endif // _TESTS