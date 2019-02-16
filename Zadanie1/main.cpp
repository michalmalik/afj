#include "interpreter.hpp"

#include <iostream>


#ifdef _TESTS

int main()
{
	Interpreter i1("tests/1.txt");
	std::cout << "Load: " << i1.execute() << "\n";

	/*
	Interpreter i2("tests/2.txt");
	std::cout << i2.execute() << "\n";

	Interpreter i3("tests/3.txt");
	std::cout << i3.execute() << "\n";

	Interpreter i4("tests/4.txt");
	std::cout << i4.execute() << "\n";

	Interpreter i5("tests/5.txt");
	std::cout << i5.execute() << "\n";
	*/

	system("pause");
	return 0;
}

#elif

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <instruction_file>\n";
		return EXIT_FAILURE;
	}

	std::string filename(argv[1], 1024);
	Interpreter interp(filename);
	
	std::cout << "Load: " << interp.load() << "\n";

	return EXIT_SUCCESS;
}

#endif // _TESTS