#include <ncurses.h>

#include <locale.h>

#include <vector>
#include <string>

#include <iostream>
#include <fstream>

#include <cmath> //min

#include "parts.h"

#define DEFAULT_COLOR 0
#define WHITE_BLACK   1
#define RED_BLACK     2
#define GREEN_BLACK   3
#define BLUE_BLACK    4  
#define YELLOW_BLACK  5
#define CYAN_BLACK    6
#define MAGENTA_BLACK 7

#define LEFT   1
#define RIGHT  2
#define TOP    3
#define BOTTOM 4

#define STRING_COMPARE_2(str, opt_a, opt_b) (opt_a == str || opt_b == str)

bool load_File(const std::string& file_to_read, 
               std::vector<Box>&  boxes, 
               std::vector<Line>& lines, 
               std::vector<Text>& texts);

void save_To_File(const std::string& file_to_write, 
                  std::vector<Box>&  boxes, 
                  std::vector<Line>& lines, 
                  std::vector<Text>& texts);

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



int main(int argc, char* argv[])
{
	std::string file_name = "diagram.txt";
	
	bool load_data = false;
	std::string file_for_data = "";

	//starting from one to ommit program name
	for(int i = 1; i < argc; i++)
	{
		const std::string arg = argv[i];
		if(STRING_COMPARE_2(arg, "-h", "--help"))
		{
			std::cout << "COMMAND LINE OPTIONS: \n"
			             "\t-h --help \tprints this help \n"
			
			             "IN PROGRAM CONTROL: \n"
			             "\tarrows      - move cursor and choosed part (if any)\n"

			             "\tspace/enter - (un)selects part (first searches for box, then for line then for text in cursor pos)\n"
		                 "\t              \tbox and text is selected by top left corner while line is selected wherever its points are\n"
		                 "\t              \t`space` doesnt work for unselecting text, enter has to be used\n"
			             "\td           - removes selected part\n"
	
			             "\tb           - create box\n"
			             "\tl           - create line\n"
			             "\tt           - create text\n"
	
			             "\tp           - add point to a line and switch to it\n"
			             "\tr           - remove point from a line\n"
			             "\ta           - change line to an arrow and vice versa\n"
			             
						 "\t[ ]         - change box size on y axis\n"
			             "\t{ }         - change box size on x axis\n\n"
		
			             "\t:           - enter command mode\n"
			             
	
			             "\tif text is selected you can type normally to add characters and remove them using backspace\n"
			             "\t\tkeep in mind that cursor is not changing its position when you are typing\n"	
		
			             "COMMANDS: \n"
			             "\tQ           - exits program, UPPERCASE, not lowercase\n"

			             "\tsFILENAME   - saves current project to FILENAME so it can be loaded later\n"
			             "\tlFILENAME   - loads current project from FILENAME, discarding what is on the screen\n"
			             "\twFILENAME   - write contents of screen to FILENAME\n"
			             
						 "\tenter       - execute current command\n"
			             "\tescape      - discard current command\n"


			             "";
		
			return 0;
		}
	}
	
	std::vector<Box> boxes;
	Box* choosed_box = nullptr;

	std::vector<Line> lines;
	Line* choosed_line = nullptr;
	int line_point = -1;

	std::vector<Text> texts;
	Text* choosed_text = nullptr;

	vec2 cursor_pos(0, 0);

	if(load_data)
	{
		if(load_File(file_for_data, boxes, lines, texts) == false)
		{
			std::cout << "Could not read file\n";
			return -1;
		}
	}


	setlocale(LC_ALL, "");

	initscr();
	cbreak();
	noecho();
	start_color();
	keypad(stdscr, true);
	
	init_pair(WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(RED_BLACK, COLOR_RED, COLOR_BLACK);
	init_pair(GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);
	init_pair(BLUE_BLACK, COLOR_BLUE, COLOR_BLACK);
	init_pair(YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);
	init_pair(CYAN_BLACK, COLOR_CYAN, COLOR_BLACK);
	init_pair(MAGENTA_BLACK, COLOR_MAGENTA, COLOR_BLACK);


	std::string command;
	bool command_mode = false;
	attron(COLOR_PAIR(WHITE_BLACK));
	while(true)
	{	
		erase();
		
		const int max_x = getmaxx(stdscr) - 1;
		const int max_y = getmaxy(stdscr) - 1;
	
		for(auto& box : boxes)
		{
			if(&box == choosed_box)
				attron(COLOR_PAIR(CYAN_BLACK));
			else
				attron(COLOR_PAIR(GREEN_BLACK));

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
			
			if(&box == choosed_box)
				attroff(COLOR_PAIR(CYAN_BLACK));
			else
				attroff(COLOR_PAIR(GREEN_BLACK));
		} // for box in boxes
		for(auto& line : lines)
		{
			if(&line == choosed_line)
				attron(COLOR_PAIR(CYAN_BLACK));
			else
				attron(COLOR_PAIR(YELLOW_BLACK));

			int prev = 0;

			//- 1 because there is no next from last one
			for(unsigned int i = 0; i < line.points.size() - 1; i++)
			{
				vec2& current = line.points[i];
				vec2& next    = line.points[i + 1];
				
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

			if(&line == choosed_line)
				attroff(COLOR_PAIR(CYAN_BLACK));
			else
				attroff(COLOR_PAIR(YELLOW_BLACK));

		} // for line in lines
		for(auto& text : texts)
		{
			if(text.pos.x + (int)text.content.length() >= max_x)
				continue;
			if(text.pos.y > max_y)
				continue;
			
			if(&text == choosed_text)
				attron(COLOR_PAIR(CYAN_BLACK));
			else
				attron(COLOR_PAIR(WHITE_BLACK));
			
			move(text.pos.y, text.pos.x);
			printw(text.content.c_str());


			if(&text == choosed_text)
				attroff(COLOR_PAIR(CYAN_BLACK));
			else
				attroff(COLOR_PAIR(WHITE_BLACK));

		} // for text in texts
		attron(COLOR_PAIR(RED_BLACK));
		mvprintw(max_y, 0, command.c_str());
		attroff(COLOR_PAIR(RED_BLACK));
		
		if(cursor_pos.y < max_y && cursor_pos.x < max_x)
		{
			curs_set(1);
			move(cursor_pos.y, cursor_pos.x);
		}
		else
		{
			curs_set(0);
		}
		
		refresh();
		
		int c = getch();
		switch(c)
		{
		//=============
		//MOVING
		case 'y':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_UP:
			if(cursor_pos.y == 0) break;

			if(choosed_box)       choosed_box->pos.y--;
			else if(choosed_line) choosed_line->points[line_point].y--;
			else if(choosed_text) choosed_text->pos.y--;

			cursor_pos.y--;
			break;
		case 'i':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_DOWN:
			if(choosed_box)       choosed_box->pos.y++;
			else if(choosed_line) choosed_line->points[line_point].y++;
			else if(choosed_text) choosed_text->pos.y++;
			
			cursor_pos.y++;
			break;
		case 'e':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_LEFT:
			if(cursor_pos.x == 0) break;
			
			if(choosed_box)       choosed_box->pos.x--;
			else if(choosed_line) choosed_line->points[line_point].x--;
			else if(choosed_text) choosed_text->pos.x--;
			
			cursor_pos.x--;
			break;
		case 'o':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_RIGHT:
			if(choosed_box)       choosed_box->pos.x++;
			else if(choosed_line) choosed_line->points[line_point].x++;
			else if(choosed_text) choosed_text->pos.x++;
			
			cursor_pos.x++;
			break;
		//=============
		//choosing
		//if nothing choosed, choose
		//if something choosed, unchoose
		case ' ':
		if(choosed_text)
			goto enter_text;
		goto choose;
		case '\n':
		if(command_mode)
			goto parse_command;
	choose:
		if(choosed_box  == nullptr
		&& choosed_line == nullptr
		&& choosed_text == nullptr)
		{
			//first check for boxes
			if((choosed_box = find_Box_With_Pos(boxes, cursor_pos)) != nullptr)
				break;
			
			//then line
			if((choosed_line = find_Line_With_Pos(lines, cursor_pos, line_point)) != nullptr)
				break;
			
			//finally texts
			//if not needed since if it returns nullptr we are fine 
			choosed_text = find_Text_With_Pos(texts, cursor_pos);
			
			//nothing to select
			break;
		}
		else
		{
			choosed_box  = nullptr;
			choosed_line = nullptr;
			choosed_text = nullptr;
			
			line_point = -1;
			break;
		}
delete_element:
		case '\t':
			if(choosed_box)
			{
				const int index = find<Box>(boxes, choosed_box);
				boxes.erase(boxes.begin() + index);
				choosed_box = nullptr;
				break;
			}
			if(choosed_line)
			{
				const int index = find<Line>(lines, choosed_line);
				lines.erase(lines.begin() + index);
				choosed_line = nullptr;
				line_point = -1;
				break;
			}
			if(choosed_text)
			{
				const int index = find<Text>(texts, choosed_text);
				texts.erase(texts.begin() + index);
				choosed_text = nullptr;
				break;
			}
			break;
		//=============
		//creating
		//-------------
		//box
		case 'b':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
				
			if(choosed_box || choosed_line)
				break;

			boxes.emplace_back(cursor_pos, 4, 3);

			choosed_box = &boxes.back();
			break;
		//-------------
		//line
		case 'l':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
			if(choosed_box || choosed_line)
				break;
			
			lines.emplace_back(cursor_pos, cursor_pos + vec2(2, 0));

			choosed_line = &lines.back();
			line_point = 0;
			break;
		//-------------
		//text
		case 't':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
			if(choosed_box || choosed_line)
				break;

			texts.emplace_back(cursor_pos, "text");

			choosed_text = &texts.back();
			break;
		
		//=============
		//specific
		//-------------
		//changing box size
		case '[':
			if(command_mode)
				goto enter_command;
			if(choosed_box)
			{
				if(choosed_box->height > 2)
					choosed_box->height--;
			
				break;
			}
			else goto def;
		case ']':
			if(command_mode)
				goto enter_command;
			if(choosed_box)
			{
				choosed_box->height++;
				
				break;
			}
			else goto def;
		case '{':
			if(command_mode)
				goto enter_command;
			if(choosed_box)
			{
				if(choosed_box->width > 2)
					choosed_box->width--;
				
				break;
			}
			else goto def;
		case '}':
			if(command_mode)
				goto enter_command;
			if(choosed_box)
			{
				choosed_box->width++;
				
				break;
			}
			else goto def;
		//-------------
		//adding and removing line points
		case 'p':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;

			if(!choosed_line) break;
			
			line_point++;
			choosed_line->points.insert(choosed_line->points.begin() + line_point, cursor_pos);

			break;
		case 'r':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
			if(!choosed_line) break;

			//less than two points so delete
			if(choosed_line->points.size() < 3) goto delete_element;
		
			choosed_line->points.erase(choosed_line->points.begin() + line_point);
			
			choosed_line = nullptr;
			line_point = -1;
			
			break;
		//-------------
		//changing if line is an arrow
		case 'a':
			if(command_mode)
				goto enter_command;
			if(choosed_text)
				goto enter_text;

			if(!choosed_line) break;
			
			choosed_line->ends_with_arrow ^= 1;

			break;
		//-------------
		//removing text
		case KEY_BACKSPACE:
			if(choosed_text && choosed_text->content.size() > 0)
				choosed_text->content.pop_back();
			if(command_mode && command.size() > 0)
				command.pop_back();
			break;
		
		//-------------
		//commands
		case ':':
			if(choosed_text)
				goto enter_text;
			
			command_mode = true;
			break;
		case 0x1B: //KEY_ESC
			command_mode = false;
			command.clear();
			break;

parse_command:
			if(command.size() == 0)
				break;
			switch(command[0])
			{
			case 'Q':
				goto end;		
		
			case 'w':
			{
				if(command.size() == 1)
					break;

				std::ofstream write(command.substr(1));
	
				//at most 500 chars per line
				char arr[500];
				//at most 100 y lines
				for(int i = 0; i < max_y; i++)
				{
					move(i, 0);
					innstr(arr, 500);

					write << arr << '\n';	
				}
			}
			break;
			case 's':
				if(command.size() > 1)
					save_To_File(command.substr(1), boxes, lines, texts);
				break;
			case 'l':
				if(command.size() > 1)
				{
					boxes.clear();
					lines.clear();
					texts.clear();
					choosed_box  = nullptr;
					choosed_line = nullptr;
					choosed_text = nullptr;
					load_File(command.substr(1),
					          boxes, lines, texts);
				}
			}

			command.clear();
			break;

		//-------------
		//adding text
def:
		default:
			if(command_mode)
			{
enter_command:
				if(c > 31 && c < 127)
			 		command += static_cast<char>(c);

				break;
			}
			if(choosed_text)
			{ 
enter_text:
				if(c > 31 && c < 127)
			 		choosed_text->content += static_cast<char>(c);

				break;
			}
		}
	}
end:
	endwin();

	return 0;
}
