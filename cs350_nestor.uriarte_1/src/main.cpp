#include "pch.hpp"
#include "demo_bvh.hpp"

int main(int argc, const char* argv[])
{
	cs350::demo_bvh demo;
	demo.create();
	while (demo.update()){}
	demo.destroy();
	return 0;
}