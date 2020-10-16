#define USE_LEGACY_MATH
#include <AE/AutoEngine.h>
#include "simon.cpp"

enum renderer_type
{
	RENDERER_PRIMITIVE,
	RENDERER_TEXTURE,
	RENDERER_MAX
};

int wmain()
{
	InitLib(LIB_OPENGL | LIB_SDL_FULL);
	render Graphics = InitGraphics("Simon", {800, 800}, SDL_WINDOW_OPENGL);
	
	if(!gladLoadGL())
	{
		printf("Failed to load glad: Unknown Error\n");
		return -1;
	}

	glViewport(0, 0, Graphics.Dim.x, Graphics.Dim.y);
	
	if(SDL_GL_SetSwapInterval(0) == -1)
	{
		OutputDebugString(SDL_GetError());
	}
	
	SYSTEMTIME SystemTime = {};
	GetSystemTime(&SystemTime);
	srand(SystemTime.wSecond);
	
	game_input Input = {};
	Input.KeyState = SDL_GetKeyboardState(0);
	
	SDL_Event Event;
	uint64 LastCount = SDL_GetPerformanceCounter();
	real32 t = 0;
	
	bool Running = true;
	
	simple_renderer Renderer[RENDERER_MAX] = {};
	
	Renderer[RENDERER_PRIMITIVE] = 
	InitSimpleRenderer("primitive_vert.glsl", "primitive_frag.glsl");
	
	Renderer[RENDERER_TEXTURE] = 
	InitSimpleRenderer("textured_vert.glsl", "textured_frag.glsl");
	
	game_state GameState = {}; 
	InitializeGame(&GameState);
	GeneratePattern(&GameState);
	
	while(Running)
	{
		ResetKeyState(&Input.Keyboard, KEY_MAX);
		ResetKeyState(&Input.Mouse, BUTTON_MAX);
		while(SDL_PollEvent(&Event) != 0)
		{
			if(HandleEvents(&Event, &Input, &Graphics))
			{
				Running = false;
			}
		}
		
		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		GameState.PressedIndex = -1;
		
		switch(GameState.Event)
		{
			case EVENT_DISPLAY_PATTERNS:
			{
				ProcessTimer(&GameState.PatternDisplayTimer, t);
				if(GameState.PatternDisplayTimer.Complete)
				{
					StartTimer(&GameState.PatternDisplayTimer);
					++GameState.CurrentPattern;
					GameState.Event = EVENT_DISPLAY_DELAY;
				}
				
				if(GameState.CurrentPattern == GameState.PatternLength)
				{
					GameState.CurrentPattern = 0;
					GameState.Event = EVENT_PLAYER;
				}
			} break;
			case EVENT_DISPLAY_DELAY:
			{
				ProcessTimer(&GameState.DisplayDelayTimer, t);
				if(GameState.DisplayDelayTimer.Complete)
				{
					StartTimer(&GameState.DisplayDelayTimer);
					GameState.Event = EVENT_DISPLAY_PATTERNS;
				}
			} break;
			case EVENT_PLAYER:
			{
				for(int Index = 0; Index < 4; ++Index)
				{
					if(Input.Mouse.Keys[BUTTON_LEFT].IsDown)
					{
						if(IsPointInRect(V2(Input.MouseState), 
						   &GameState.Squares[Index]))
					   {
						   GameState.PressedIndex = Index;
					   }
					}
					if(Input.Mouse.Keys[BUTTON_LEFT].EndedDown)
					{
						if(IsPointInRect(V2(Input.MouseState), 
						   &GameState.Squares[Index]))
						{
							if(IsCorrectPattern(&GameState, Index))
							{
								GameState.CurrentPattern++;
								if(GameState.CurrentPattern == 
								   GameState.PatternLength)
								{
									IncreaseDifficulty(&GameState);
								}
							}
							else
							{
								ResetGame(&GameState);
							}
						}
					}
					
				}
			} break;
		}
		
		printf("Score: %d\n", GameState.Score);
		
		real32 FrameCompleteTime = 
		Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter());
		
		if(FrameCompleteTime < Graphics.Display.TargetSecPerFrame)
		{
			int SleepTime = (int)((Graphics.Display.TargetSecPerFrame - 
								  FrameCompleteTime) * 1000) - 1;
			if(SleepTime > 0)
			{
				Sleep(SleepTime);
			}
			//assert(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) < TargetSecPerFrame);
			while(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) <
				  Graphics.Display.TargetSecPerFrame);
		}
		
		uint64 EndCount = SDL_GetPerformanceCounter();
		
		//Render
		mat4 Projection = Ortho(0, 800, 800, 0, -1, 1);
		SetGlobalUniforms(Renderer[RENDERER_PRIMITIVE].Shader, Projection, Mat4());
		SetGlobalUniforms(Renderer[RENDERER_TEXTURE].Shader, Projection, Mat4());
		
		switch(GameState.Event)
		{
			case EVENT_DISPLAY_PATTERNS:
			{
				RenderDisplayPatterns(&GameState, &Renderer[RENDERER_PRIMITIVE]);
			} break;
			case EVENT_DISPLAY_DELAY:
			{
				RenderPatterns(&GameState, &Renderer[RENDERER_PRIMITIVE]);
			} break;
			case EVENT_PLAYER:
			{
				RenderPatterns(&GameState, &Renderer[RENDERER_PRIMITIVE]);
			} break;
		}
		
		SDL_GL_SwapWindow(Graphics.Window);
		
		real32 ElapsedTime = Win32GetSecondElapsed(LastCount, EndCount);
		t = ElapsedTime;

		uint64 CounterElapsed = EndCount - LastCount;
		real32 FPS = (real32)PerformanceFrequency / (real32)CounterElapsed;
		
		//PrintFPS(FPS);
		
		LastCount = EndCount;
	}
	
	return 0;
}