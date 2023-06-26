#pragma once

#include "LoFox/Core/Core.h"
#include "LoFox/Core/Application.h"

#ifdef LF_PLATFORM_WINDOWS

extern LoFox::Application* LoFox::CreateApplication();

int main(int argc, char** argv) {

	LoFox::Log::Init();

	auto app = LoFox::CreateApplication();

	app->Run();

	delete app;
}

#endif
