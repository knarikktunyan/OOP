#include "../include/compiler-pipeline.hpp"

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Error: Expecting a file for execution\n";
		return 1;
	}
	try
	{
		VirtualMachine vm;
		vm.loadExecutableFile(argv[1]);
		// vm.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}


// int main(int argc, char** argv)
// {
//     if (argc < 2) {
//         std::cerr << "Usage: " << argv[0] << " <exe-file> [--debug]\n";
//         return 1;
//     }
 
//     std::string exePath = argv[1];
//     bool debug = (argc >= 3 && std::string(argv[2]) == "--debug");
 
//     try {
//         VirtualMachine vm;
//         vm.loadAndRun(exePath, debug);
//     }
//     catch (const std::exception& e) {
//         std::cerr << "[VirtualMachine ERROR] " << e.what() << "\n";
//         return 1;
//     }
 
//     return 0;
// }
 
