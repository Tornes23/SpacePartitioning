#include "pch.hpp"
#include "demo_renderer.hpp"

int main(int argc, const char* argv[])
{
	cs350::demo_renderer demo;
	demo.create();
	while (demo.update()){}
	demo.destroy();
	return 0;
}