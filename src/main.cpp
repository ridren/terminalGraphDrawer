#include <ncurses.h>

#include <locale.h>

#include <vector>
#include <string>

#include <iostream>
#include <fstream>

#include <cstring> //strcmp
#include <cmath> //min

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

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

#define STRING_COMPARE_2(str, opt_a, opt_b) (0 == strcmp(str, opt_a) || 0 == strcmp(str, opt_a))

bool load_File(const std::string& file_to_read, 
               std::vector<Box>&  boxes, 
               std::vector<Line>& lines, 
               std::vector<Text>& texts);

void save_To_File(const std::string& file_to_write, 
                  std::vector<Box>&  boxes, 
                  std::vector<Line>& lines, 
                  std::vector<Text>& texts);

void load_Config();



void render_Box (const Box&  box,  const int max_x, const int max_y);
void render_Line(const Line& line, const int max_x, const int max_y);
void render_Text(const Text& text, const int max_x, const int max_y);


int main(int argc, char* argv[])
{
	//starting from one to ommit program name
	for(int i = 1; i < argc; i++)
	{
		if(STRING_COMPARE_2(argv[i], "-h", "--help"))
		{
			std::cout << "COMMAND LINE OPTIONS: \n"
			             "\t-h --help   \tprints this help \n"
			             "\t-c --config \tprints help about config file\n"
			
			             "IN PROGRAM CONTROL: \n"
			             "\tarrows      - move cursor and choosed part (if any)\n"

			             "\tspace/enter - (un)selects part (first searches for box, then for line then for text in cursor pos)\n"
		                 "\t              \tbox and text are selected by top left corner while line is selected wherever its points are\n"
		                 "\t              \t`space` doesnt work for unselecting text, enter has to be used\n"
			             "\tTAB         - removes selected part\n"
	
			             "\tb           - create box\n"
			             "\tl           - create line\n"
			             "\tt           - create text\n"
	
			             "\tp           - add point to a line and switch to it\n"
			             "\tr           - remove point from a line\n"
			             "\ta           - change line to an arrow and vice versa\n"
			             
						 "\t[ ]         - change box size on y axis\n"
			             "\t{ }         - change box size on x axis\n"
		
			             "\tc/'         - copy selected object\n"
			             "\tv/\\         - paste copied object\n\n"

			             "\t:           - enter command mode\n"
			             
	
			             "\tif text is selected, characters are added/removed via regular typing/using backspace\n"
			             "\t\tkeep in mind that cursor is not changing its position while chars are added/removed\n"	
		
			             "COMMANDS: \n"
			             "\tQ           - exits program, UPPERCASE, not lowercase\n"
			             "\tCLEAR       - deletes screen contents\n"


			             "\tsFILENAME   - saves current project to FILENAME so it can be loaded later\n"
			             "\tlFILENAME   - loads current project from FILENAME, discarding what is on the screen\n"
			             "\twFILENAME   - write contents of screen to FILENAME\n"
			             
						 "\tenter       - execute current command\n"
			             "\tescape      - discard current command\n"


			             "";
		
			return 0;
		}
		if(STRING_COMPARE_2(argv[i], "-c", "--config"))
		{
			std::cout << "config file is currently used to specify colors of each element\n"
			             "format of the file is as follows:\n"
			             "\tR G B\n"
			             "\tR G B\n"
			             "\tR G B\n"
			             "\tR G B\n"
			             "\tR G B\n"
			             "\n"
			             "R, G, B stand for numbers in the range [0, 1000]\n"
			             "lines, in order from top to botom denote:\n"
			             "\tcolor of the box\n"
			             "\tcolor of the line\n"
			             "\tcolor of the text\n"
			             "\tcolor of the choosed element\n"
			             "\tcolor of the command\n"
			             "\n"
			             "unless config file is specified exactly in the way described above, the behavior is undefined\n"
			             "\n"
						 "config file is located at $HOME/.config/TBDW/config.txt\n"
			             "";
			return 0;
		}
	}

	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	start_color();
	keypad(stdscr, true);
	mousemask(BUTTON1_PRESSED 
	        | BUTTON1_RELEASED
	        | BUTTON1_CLICKED
	        | REPORT_MOUSE_POSITION, NULL);

	// Makes the terminal report mouse movement events
	printf("\033[?1003h\n");

	load_Config();

	std::vector<Box> boxes;
	Box* choosed_box = nullptr;

	std::vector<Line> lines;
	Line* choosed_line = nullptr;
	int line_point = -1;

	std::vector<Text> texts;
	Text* choosed_text = nullptr;

	vec2* choosed = nullptr;

	vec2 cursor_pos(0, 0);

	
	Box copied_box;
	Line copied_line;
	int copied_line_point = -1;
	Text copied_text;
	int copied_type = 0;

	std::vector<vec2*> to_move;
	//0, 0 is TL of screen, hard to imagine someone having 1M x 1M screen
	vec2 to_move_tl(1'000'000, 1'000'000);
	
	std::string command;
	bool mode_command = false;
	MEVENT mevent;
	while(true)
	{	
		erase();
		
		const int max_x = getmaxx(stdscr) - 1;
		const int max_y = getmaxy(stdscr) - 1;
	
		for(const auto& box : boxes)
		{
			if(&box == choosed_box)
			{
				attron(COLOR_PAIR(PAIR_CHOS));
				render_Box(box, max_x, max_y);
				attroff(COLOR_PAIR(PAIR_CHOS));
			}
			else
			{
				attron(COLOR_PAIR(PAIR_BOX));
				render_Box(box, max_x, max_y);
				attroff(COLOR_PAIR(PAIR_CHOS));
			}
		} 
		for(const auto& line : lines)
		{
			if(&line == choosed_line)
			{
				attron(COLOR_PAIR(PAIR_CHOS));
				render_Line(line, max_x, max_y);
				attroff(COLOR_PAIR(PAIR_CHOS));
			}
			else
			{
				attron(COLOR_PAIR(PAIR_LINE));
				render_Line(line, max_x, max_y);
				attroff(COLOR_PAIR(PAIR_LINE));
			}
		} 
		for(const auto& text : texts)
		{
			if(text.pos.x + (int)text.content.length() >= max_x)
				continue;
			if(text.pos.y > max_y)
				continue;
			
			if(&text == choosed_text)
			{
				attron(COLOR_PAIR(PAIR_CHOS));
				render_Text(text, max_x, max_y);
				attroff(COLOR_PAIR(PAIR_CHOS));
			}
			else
			{
				attron(COLOR_PAIR(PAIR_TEXT));
				render_Text(text, max_x, max_y);
				attroff(COLOR_PAIR(PAIR_TEXT));
			}
		} 
		attron(COLOR_PAIR(PAIR_COMM));
		mvprintw(max_y, 0, command.c_str());
		attroff(COLOR_PAIR(PAIR_COMM));
		
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
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_UP:
			if(cursor_pos.y == 0) break;
			cursor_pos.y--;

			if(choosed)
				choosed->y--;

			break;
		case 'i':
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_DOWN:
			cursor_pos.y++;

			if(choosed)
				choosed->y++;
			
			break;
		case 'e':
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_LEFT:
			if(cursor_pos.x == 0) break;

			cursor_pos.x--;
			
			if(choosed)
				choosed->x--;
			
			break;
		case 'o':
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
		__attribute__((fallthrough));
		case KEY_RIGHT:

			cursor_pos.x++;
			
			if(choosed)
				choosed->x++;

			break;
		//=============
		//copy and paste
		case 'c':
			if(choosed_text)
				goto enter_text;
			__attribute__((fallthrough));
		case '\'':
			if(mode_command)
				goto enter_command;
			if(choosed_box)
			{
				copied_box = *choosed_box;
				copied_box.pos -= cursor_pos;
				copied_type = 1;
			}
			else if(choosed_line)
			{
				copied_line = *choosed_line;
				copied_line_point = line_point;
				for(vec2& p : copied_line.points)
					p -= cursor_pos;
				copied_type = 2;
			}
			else if(choosed_text)
			{
				copied_text = *choosed_text;
				copied_text.pos -= cursor_pos;
				copied_type = 3;
			}
			break;
		case 'v':
			if(choosed_text)
				goto enter_text;
			__attribute__((fallthrough));
		case '\\':
			if(mode_command)
				goto enter_command;
			switch(copied_type)
			{
			case 1:
				boxes.emplace_back(copied_box);
				boxes.back().pos += cursor_pos; 
				choosed_box = &boxes.back();
				choosed     = &boxes.back().pos;
				break;
			case 2:
				lines.emplace_back(copied_line);
				for(vec2& p : lines.back().points)
				{
					if(vec2{0, 0} == p)
						choosed = &p;

					p += cursor_pos;

				}
				choosed_line = &lines.back();
				line_point = copied_line_point;
				break;

			case 3:
				texts.emplace_back(copied_text);
				texts.back().pos += cursor_pos; 
				choosed_text = &texts.back();
				choosed      = &texts.back().pos;
				break;
				break;
			default:
				break;
			}
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
		if(mode_command)
			goto parse_command;
	choose:
		if(choosed == nullptr)
		{
			//first check for boxes
			if((choosed_box = find_Box_With_Pos(boxes, cursor_pos)) != nullptr)
			{
				choosed = &choosed_box->pos;
				break;
			}
			
			//then line
			if((choosed_line = find_Line_With_Pos(lines, cursor_pos, line_point)) != nullptr)
			{
				choosed = &choosed_line->points[line_point];
				break;
			}
			
			//finally texts
			//if not needed since if it returns nullptr it is fine 
			choosed_text = find_Text_With_Pos(texts, cursor_pos);
			if(choosed_text)
			{
				choosed = &choosed_text->pos;
				break;
			}
			
			//nothing to select
			break;
		}
		else
		{
			choosed      = nullptr;
			choosed_box  = nullptr;
			choosed_line = nullptr;
			choosed_text = nullptr;
			
			line_point = -1;
			break;
		}
		//add currently choosed to list of to move
		case '`':
			if(choosed)
			{
				to_move.emplace_back(choosed);
				if(choosed->x < to_move_tl.x)
					to_move_tl.x = choosed->x;
				if(choosed->y < to_move_tl.y)
					to_move_tl.y = choosed->y;
			}
			break;
delete_element:
		case '\t':
			choosed = nullptr;
			if(choosed_box)
			{
				const int index = find<Box>(boxes, choosed_box);
				boxes.erase(boxes.begin() + index);
				choosed_box = nullptr;
				break;
			}
			else if(choosed_line)
			{
				const int index = find<Line>(lines, choosed_line);
				lines.erase(lines.begin() + index);
				choosed_line = nullptr;
				line_point = -1;
				break;
			}
			else if(choosed_text)
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
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
				
			if(choosed_box || choosed_line)
				break;

			boxes.emplace_back(cursor_pos, 4, 3);
			
			choosed_box = &boxes.back();
			choosed = &choosed_box->pos; 
			break;
		//-------------
		//line
		case 'l':
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
			if(choosed_box || choosed_line)
				break;
			
			lines.emplace_back(cursor_pos, cursor_pos + vec2(2, 0));

			choosed_line = &lines.back();
			line_point = 0;
			choosed = &choosed_line->points[0];
			break;
		//-------------
		//text
		case 't':
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
			if(choosed_box || choosed_line)
				break;

			texts.emplace_back(cursor_pos, "text");

			choosed_text = &texts.back();
			choosed = &choosed_text->pos;
			break;
		
		//=============
		//specific
		//-------------
		//changing box size
		case '[':
			if(mode_command)
				goto enter_command;
			if(choosed_box)
			{
				if(choosed_box->height > 2)
					choosed_box->height--;
			
				break;
			}
			else goto def;
		case ']':
			if(mode_command)
				goto enter_command;
			if(choosed_box)
			{
				choosed_box->height++;
				
				break;
			}
			else goto def;
		case '{':
			if(mode_command)
				goto enter_command;
			if(choosed_box)
			{
				if(choosed_box->width > 2)
					choosed_box->width--;
				
				break;
			}
			else goto def;
		case '}':
			if(mode_command)
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
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;

			if(!choosed_line) break;
			
			line_point++;
			choosed_line->points.insert(choosed_line->points.begin() + line_point, cursor_pos);
			choosed = &choosed_line->points[line_point];

			break;
		case 'r':
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;
			if(!choosed_line) break;

			//less than two points so delete
			if(choosed_line->points.size() < 3) goto delete_element;
		
			choosed_line->points.erase(choosed_line->points.begin() + line_point);
			
			choosed_line = nullptr;
			choosed = nullptr;
			line_point = -1;
			
			break;
		//-------------
		//changing if line is an arrow
		case 'a':
			if(mode_command)
				goto enter_command;
			if(choosed_text)
				goto enter_text;

			if(!choosed_line) break;
			
			choosed_line->ends_with_arrow ^= 1;

			break;
		//-------------
		//removing text
		case KEY_BACKSPACE:
		case '\b':
			if(choosed_text && choosed_text->content.size() > 0)
				choosed_text->content.pop_back();
			if(mode_command && command.size() > 0)
				command.pop_back();
			break;
		
		//-------------
		//commands
		case ':':
			if(choosed_text)
				goto enter_text;
			
			mode_command = true;
			break;
		case 0x1B: //KEY_ESC
			mode_command = false;
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
				break;
			case 'C':
				if("CLEAR" == command)
				{
					boxes.clear();
					lines.clear();
					texts.clear();
					choosed_box  = nullptr;
					choosed_line = nullptr;
					choosed_text = nullptr;
				}

				break;
			case 'M':
				if("MMV" == command)
				{
					//everything is placed relative to the cursor the way it is relative to the to_move_tl

					for(vec2* vp : to_move)
					{
						vec2 const offset = *vp - to_move_tl;
						*vp = offset + cursor_pos;
					}
					to_move.clear();
					to_move_tl.x = 1'000'000;
					to_move_tl.y = 1'000'000;
				}
				if("MCL" == command)
				{
					to_move.clear();
					to_move_tl.x = 1'000'000;
					to_move_tl.y = 1'000'000;
				}


				if("MAL" == command)
				{
					vec2 tl = {1'000'000, 1'000'000};

					for(Box const& box : boxes)
					{
						if(box.pos.x < tl.x)
							tl.x = box.pos.x;
						if(box.pos.y < tl.y)
							tl.y = box.pos.y;
					}
					for(Text const& text : texts)
					{
						if(text.pos.x < tl.x)
							tl.x = text.pos.x;
						if(text.pos.y < tl.y)
							tl.y = text.pos.y;
					}
					for(Line const& line : lines)
						for(vec2 const& point : line.points)
						{
							if(point.x < tl.x)
								tl.x = point.x;
							if(point.y < tl.y)
								tl.y = point.y;
						}


					for(Box& box : boxes)
						box.pos += cursor_pos - tl;
					for(Text& text : texts)
						text.pos += cursor_pos - tl;
					for(Line& line : lines)
						for(vec2& point : line.points)
							point += cursor_pos - tl;
				}
			}
			
			mode_command = false;
			command.clear();
			break;
		//-------------
		//moving by mouse
		case KEY_MOUSE:
			if(getmouse(&mevent) != OK)
				break;

			if(mevent.bstate & BUTTON1_CLICKED)
				goto choose;

			if(mevent.bstate & BUTTON1_PRESSED)
			{
				choosed_box  = nullptr;
				choosed_line = nullptr;
				choosed_text = nullptr;
				choosed      = nullptr;
				goto choose;
			}

			if(mevent.bstate & BUTTON1_RELEASED)
			{
				choosed_box  = nullptr;
				choosed_line = nullptr;
				choosed_text = nullptr;
				choosed      = nullptr;
			}

			if(mevent.bstate & REPORT_MOUSE_POSITION)
			{
				cursor_pos = {mevent.x, mevent.y};

				if(choosed)
					*choosed = cursor_pos;
			}

			break;

		//-------------
		//adding text
def:
		default:
			if(mode_command)
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
	// disable reporting of mouse movement 
	printf("\033[?1003l\n");

	return 0;
}
