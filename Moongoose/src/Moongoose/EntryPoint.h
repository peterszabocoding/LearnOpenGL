#pragma once

#ifdef MG_PLATFORM_WINDOWS

extern Moongoose::Application* Moongoose::CreateApplication();

int main(int argc, char** argv)
{
	printf("Moongoose Engine");

	Moongoose::Log::Init();

	auto application = Moongoose::CreateApplication();
	application->Run();
	delete application;
}

#endif