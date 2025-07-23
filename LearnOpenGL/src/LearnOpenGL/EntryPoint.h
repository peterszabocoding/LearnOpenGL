#pragma once

#ifdef MG_PLATFORM_WINDOWS

extern LearnOpenGL::Application* LearnOpenGL::CreateApplication();

int main(int argc, char** argv)
{
	printf("LearnOpenGL Engine");

	LearnOpenGL::Log::Init();

	auto application = LearnOpenGL::CreateApplication();
	application->Run();
	delete application;
}

#endif
