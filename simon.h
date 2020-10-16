#ifndef SIMON_H
#define SIMON_H

enum square_pattern
{
	SQUARE_YELLOW,
	SQUARE_RED,
	SQUARE_GREEN,
	SQUARE_BLUE,
};

enum game_event
{
	EVENT_DISPLAY_PATTERNS,
	EVENT_DISPLAY_DELAY,
	EVENT_PLAYER,
};

struct game_state
{
	rect32 Squares[4];
	v4 SquareColors[4];
	int PressedIndex;
	
	std::vector<square_pattern> Patterns;
	int PatternLength;
	int CurrentPattern;
	
	timer PatternDisplayTimer;
	timer DisplayDelayTimer;
	
	game_event Event;
	
	uint32 Score;
};

void InitializeGame(game_state* State);
void GeneratePattern(game_state* State);

bool IsCorrectPattern(game_state* State, int Index);

void RenderPatterns(game_state* State, uint32 Renderer);
#endif