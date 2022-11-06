#include "cbWebserver.hpp"

int main(int argc, char *argv[])
{
	const char* root = "/home/exia/c-workshop/covalentBond/bin";
	cb::utils::cbWebserver server(8088, root);
	server.execMain();

	return 0;
}
