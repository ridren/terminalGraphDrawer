#include <ncurses.h>

#include "parts.h"

//SHARED BETWEEN FILES
#define COLOR_BOX  1
#define COLOR_LINE 2
#define COLOR_TEXT 3
#define COLOR_CHOS 4
#define COLOR_COMM 5

//SHARED BETWEEN FILES 
#define PAIR_BOX   1 
#define PAIR_LINE  2
#define PAIR_TEXT  3
#define PAIR_CHOS  4
#define PAIR_COMM  5

//SHARED BETWEEN FILES 
#define LEFT   1
#define RIGHT  2
#define TOP    3
#define BOTTOM 4

void draw_Line(const char* left, const char* middle, const char* right, const int amount)
{
	if(left) printw(left);
	
	for(int x = 0; x < amount; x++)
	{
		if(middle) printw(middle);
		else printw(" ");
	}

	if(right) printw(right);
}

void draw_Vertical_Line_From_Top(const char* top, const char* middle, const char* bottom, const int height)
{
	const int cur_x = getcurx(stdscr);
	const int cur_y = getcury(stdscr);

	if(top) printw(top);
	for(int y = 1; y < height; y++)
	{
		if(middle)
		{
			move(cur_y + y, cur_x);
			printw(middle);
		}
	}
	move(cur_y + height, cur_x);
	if(bottom) printw(bottom);
}


void render_Box(const Box& box, const int max_x, const int max_y)
{
	const int relative_box_height = box.height + box.pos.y;
	const int relative_box_width = box.width + box.pos.x;
	for(int y = box.pos.y; y < relative_box_height; y++)
	{
		if(y >= max_y)
			break;
	
		if(box.pos.x >= max_x)
			break;;
		
		move(y, box.pos.x);
			
		//minus two because we are omiiting corners
		const int dist_bet_corners = relative_box_width - box.pos.x - 2; 
		const int dist_to_end = max_x - box.pos.x;
		const bool first_smaller = dist_bet_corners < dist_to_end;
		const int smaller_gist = first_smaller ? dist_bet_corners : dist_to_end;
		
		if(y == box.pos.y)
		{
			draw_Line(box.corner_TL, box.straight, first_smaller ? box.corner_TR : nullptr, smaller_gist);
		}
		else if(y == relative_box_height - 1)
		{
			draw_Line(box.corner_BL, box.straight, first_smaller ? box.corner_BR : nullptr, smaller_gist);
		}
		else
		{
			draw_Line(box.vertical, nullptr, first_smaller ? box.vertical : nullptr, smaller_gist);
		}
	}
}

void render_Line(const Line& line, const int max_x, const int max_y)
{
	int prev = 0;

	//- 1 because there is no next from last one
	for(unsigned int i = 0; i < line.points.size() - 1; i++)
	{
		const vec2& current = line.points[i];
		const vec2& next    = line.points[i + 1];
		
		if(current == next) continue;
		
		if(current.x < next.x)
		{
			if(current.x >= max_x) continue;

			const int dist_to_end = max_x - current.x;
			const int dist_to_print = next.x - 1 - current.x;

			move(current.y, current.x);
			if(current.y < max_y)
			{
				switch(prev)
				{
				case TOP:
					draw_Line("└", "─", nullptr, std::min(dist_to_end, dist_to_print));
					break;
				case BOTTOM:
					draw_Line("┌", "─", nullptr, std::min(dist_to_end, dist_to_print));
					break;
				default:
					draw_Line("─", "─", nullptr, std::min(dist_to_end, dist_to_print));
				}
			}
			prev = LEFT;
		}
		else if(current.x > next.x)
		{
			if(next.x >= max_x) continue;
			
			const int dist_to_end = max_x - next.x;
			const int dist_to_print = current.x - 1 - next.x;

			if(current.y < max_y)
			{
				move(current.y, next.x);
				//if(current.x <= max_x)
				switch(prev)
				{
				case TOP:
					draw_Line(nullptr, "─", "┘", std::min(dist_to_end, dist_to_print) + 1);
					break;
				case BOTTOM:
					draw_Line(nullptr, "─", "┐", std::min(dist_to_end, dist_to_print) + 1);
					break;
				default:
					draw_Line(nullptr, "─", "─", std::min(dist_to_end, dist_to_print) + 1);
				}
		
			
			}
			prev = RIGHT;
			if(next.x >= max_x) continue;
		}

		
		//current is lower
		if(current.y > next.y)
		{
			int top_to_bot  = current.y - next.y;
			int dist_to_bot = max_y - next.y; 
			
			if(dist_to_bot < 1) continue;
				
			move(next.y, next.x);
			switch(prev)
			{
			case LEFT:
				draw_Vertical_Line_From_Top(nullptr, "│", current.y < max_y ? "┘" : nullptr, std::min(top_to_bot, dist_to_bot));
				break;
			case RIGHT:
				draw_Vertical_Line_From_Top(nullptr, "│", current.y < max_y ? "└" : nullptr, std::min(top_to_bot, dist_to_bot));
				break;
			default:
				draw_Vertical_Line_From_Top(nullptr, "│", current.y < max_y ? "│" : nullptr, std::min(top_to_bot, dist_to_bot));
			}

			prev = BOTTOM;
		}	
		//current is higher
		else if(current.y < next.y)
		{
			int top_to_bot  = next.y - current.y;
			int dist_to_bot = max_y - current.y; 
		
			if(dist_to_bot < 1) continue;
		
			move(current.y, next.x);
			switch(prev)
			{
			case LEFT:
				draw_Vertical_Line_From_Top("┐", "│", nullptr, std::min(top_to_bot, dist_to_bot));
				break;
			case RIGHT:
				draw_Vertical_Line_From_Top("┌", "│", nullptr, std::min(top_to_bot, dist_to_bot));
				break;
			default:
				draw_Vertical_Line_From_Top("│", "│", nullptr, std::min(top_to_bot, dist_to_bot));
			}
	
			prev = TOP;
		}
		

		if(i == line.points.size() - 2)
		{
			move(next.y, next.x);
			if(next.y >= max_y - 1) break;

			if(line.ends_with_arrow)
			{
				switch(prev)
				{
				case BOTTOM:
					printw("↑");
					break;
				case TOP:
					printw("↓");
					break;
				case LEFT:
					printw("→");
					break;
				case RIGHT:
					printw("←");
					break;
				}
			}
			else
			{
				if(prev == BOTTOM || prev == TOP)
				{
					printw("│");
				}
				else
				{
					printw("─");
				}
			}
		}

	}// for points in line
}

void render_Text(const Text& text, [[maybe_unused]] const int max_x, [[maybe_unused]] const int max_y)
{
	move(text.pos.y, text.pos.x);
	printw(text.content.c_str());
}
