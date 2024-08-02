#include "DebugClass.hxx"

#include <string>

#include "SDLApp.hxx"

DebugClass::DebugClass()
	:Singleton(UPDATE_ORDER::LAST_UPDATE)
{

}

DebugClass::~DebugClass()
{

}

bool DebugClass::Init()
{
	return true;
}

void DebugClass::Update()
{
	unsigned int deltaTime = SDLApp::GetInstance().DeltaTime();
	float fps = 1000.0f / (float)deltaTime;
	SDL_Log(std::to_string( fps).c_str());
}