#include "simon.h"

void InitializeGame(game_state* State)
{
	State->PressedIndex = -1;
	
	State->SquareColors[0] = Color(1, 1, 0);
	State->SquareColors[1] = Color(1, 0, 0);
	State->SquareColors[2] = Color(0, 1, 0);
	State->SquareColors[3] = Color(0, 0, 1);
	
	v2 SquareDim = V2(350, 350);
	for(int y = 0; y < 2; ++y)
	{
		for(int x = 0; x < 2; ++x)
		{
			State->Squares[y * 2 + x] = 
			Rect32(V2(25, 25) + V2(50.0f*x, 50.0f*y) + 
				   V2(SquareDim.x * x, SquareDim.y * y),
				   SquareDim);
		}
	}
	
	State->PatternLength = 4;
	State->Patterns.reserve(State->PatternLength);
	State->PatternDisplayTimer = CreateTimer(1.0f);
	State->DisplayDelayTimer = CreateTimer(0.5f);
}

void GeneratePattern(game_state* State)
{
	for(int i = 0; i < State->PatternLength; ++i)
	{
		State->Patterns[i] = (square_pattern)Rand(0, 3);
	}
}

bool IsCorrectPattern(game_state* State, int Index)
{
	return Index == State->Patterns[State->CurrentPattern];
}

void ResetPatterns(game_state* State)
{
	State->CurrentPattern = 0;
	GeneratePattern(State);
	State->Event = EVENT_DISPLAY_DELAY;
}

void IncreaseDifficulty(game_state* State)
{
	State->Score += State->PatternLength;
	++State->PatternLength;
	ResetPatterns(State);
}

void ResetGame(game_state* State)
{
	State->PatternLength = 4;
	State->Score = 0;
	ResetPatterns(State);
}

void RenderPatterns(game_state* State, simple_renderer* Renderer)
{
	for(int y = 0; y < 2; ++y)
	{
		for(int x = 0; x < 2; ++x)
		{
			int Index = y * 2 + x;
			real32 Brightness = State->PressedIndex == Index ? 1.0f : 0.5f;
			
			BasicDraw(Renderer->Shader, &Renderer->Rect, 
					  V3(State->Squares[Index].Pos + State->Squares[Index].Dim*0.5f),
					  V3(State->Squares[Index].Dim),
					  Hadamard(State->SquareColors[Index], Brightness),
					  0, {}, 
					  true, true);
		}
	}
}

void RenderDisplayPatterns(game_state* State, simple_renderer* Renderer)
{
	for(int y = 0; y < 2; ++y)
	{
		for(int x = 0; x < 2; ++x)
		{
			int Index = y * 2 + x;
			real32 Brightness = 
			Index == State->Patterns[State->CurrentPattern] ? 1.0f : 0.5f;
			
			BasicDraw(Renderer->Shader, &Renderer->Rect, 
					  V3(State->Squares[Index].Pos + State->Squares[Index].Dim*0.5f),
					  V3(State->Squares[Index].Dim),
					  Hadamard(State->SquareColors[Index], Brightness),
					  0, {}, 
					  true, true);
		}
	}
}