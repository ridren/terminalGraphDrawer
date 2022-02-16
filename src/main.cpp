#include <ncurses.h>

#include <locale.h>

#include <vector>
#include <string>

#include <iostream>
#include <fstream>

#include "parts.h"

#define DEFAULT_COLOR 0
#define WHITE_BLACK 1
#define RED_BLACK 2
#define GREEN_BLACK 3
#define BLUE_BLACK 4
#define YELLOW_BLACK 5
#define CYAN_BLACK 6
#define MAGENTA_BLACK 7

#define LEFT 1
#define RIGHT 2
#define TOP 3
#define BOTTOM 4

int min(int a, int b)
{
	return a <= b ? a : b;
}

void drawLine(const char* left, const char* middle, const char* right, int amount)
{
	if(left) printw(left);
	
	for(int x = 0; x < amount; x++)
	{
		if(middle) printw(middle);
		else printw(" ");
	}

	if(right) printw(right);
}

void drawVerticalLineFromTop(const char* top, const char* middle, const char* bottom, int height)
{
	int curX = getcurx(stdscr);
	int curY = getcury(stdscr);

	if(top) printw(top);
	for(int y = 1; y < height; y++)
	{
		if(middle)
		{
			move(curY + y, curX);
			printw(middle);
		}
	}
	move(curY + height, curX);
	if(bottom) printw(bottom);
}

int main(int argc, char* argv[])
{
	std::string fileName = "graph.txt";
	if(argc > 1)
	{
		std::string arg1 = argv[1];
		if("-h" == arg1 || "--help" == arg1)
		{
			std::cout << "COMMAND LINE OPTIONS: \n"
			             "\t-h --help \tprints this help \n"
			             "\t-w --write\tspecifies file to write \n\n"
			
			             "IN PROGRAM CONTROL: \n"
			             "\tarrows - move cursor and choosed part (if any)\n"

			             "\tF1     - selects part (first searches for box, then for line then for text in cursor pos)\n"
		                 "\t         \tbox and text is selected by top left corner while line is selected wherever its points are\n"
			             "\tF2     - unselects part\n"
			             "\tF3     - removes selected part\n"
	
			             "\tF4     - create box\n"
			             "\tF5     - create line\n"
			             "\tF6     - create text\n"
	
			             "\tF7     - add point to a line and switch to it\n"
			             "\tF8     - remove point from a line\n"
	
			             "\tQ      - exits program\n"
			             "\tw      - writes graph to \"graph.txt\" or file specified by user (for safety reasons, it should be empty file)\n"
			             "\t         \tto be more specific it writes whatever is in 200x by 100y rectangle\n"
		
			             "\t[ ]    - change box size on y axis\n"
			             "\t{ }    - change box size on x axis\n\n"
		
	
			             "\tif text is selected you can type normally to add characters and remove them using backspace\n"
			             "\t\tkeep in mind that cursor is not changing its position when you are typing\n";	
		
			return 0;
		}
		if("-w" == arg1 || "--write" == arg1)
		{
			if(argc > 2)
			{
				fileName = argv[2];
			}
			else
			{
				std::cout << "Not enough arguments\n";
				return -1;
			}
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

	std::vector<Box> boxes;
	Box* choosedBox = nullptr;

	std::vector<Line> lines;
	Line* choosedLine = nullptr;
	int linePoint = -1;

	std::vector<Text> texts;
	Text* choosedText = nullptr;

	vec2 cursorPos(0, 0);


	attron(COLOR_PAIR(WHITE_BLACK));
	while(true)
	{	
		int maxX = getmaxx(stdscr) - 1;
		int maxY = getmaxy(stdscr) - 1;

		erase();
		
		for(auto& box : boxes)
		{
			if(&box == choosedBox)
				attron(COLOR_PAIR(CYAN_BLACK));
			else
				attron(COLOR_PAIR(GREEN_BLACK));

			int relativeBoxHeight = box.height + box.pos.y;
			int relativeBoxWidth = box.width + box.pos.x;
			for(int y = box.pos.y; y < relativeBoxHeight; y++)
			{
				if(y >= maxY)
					break;
			
				if(box.pos.x >= maxX)
					break;;
				
				move(y, box.pos.x);
					
				//minus two because we are omiiting corners
				int distBetCorners = relativeBoxWidth - box.pos.x - 2; 
				int distToEnd = maxX - box.pos.x;
				bool firstSmaller = distBetCorners < distToEnd;
				int smallerDist = firstSmaller ? distBetCorners : distToEnd;
				
				if(y == box.pos.y)
				{
					drawLine(box.TLCorner, box.straight, firstSmaller ? box.TRCorner : nullptr, smallerDist);
				}
				else if(y == relativeBoxHeight - 1)
				{
					drawLine(box.BLCorner, box.straight, firstSmaller ? box.BRCorner : nullptr, smallerDist);
				}
				else
				{
					drawLine(box.vertical, nullptr, firstSmaller ? box.vertical : nullptr, smallerDist);
				}
			}
			
			if(&box == choosedBox)
				attroff(COLOR_PAIR(CYAN_BLACK));
			else
				attroff(COLOR_PAIR(GREEN_BLACK));
		} // for box in boxes
		for(auto& line : lines)
		{
			if(&line == choosedLine)
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
					if(current.x >= maxX) continue;

					int distToEnd = maxX - current.x;
					int distToPrint = next.x - 1 - current.x;

					move(current.y, current.x);
					if(current.y < maxY)
					{
						switch(prev)
						{
						case TOP:
							drawLine("└", "─", nullptr, min(distToEnd, distToPrint));
							break;
						case BOTTOM:
							drawLine("┌", "─", nullptr, min(distToEnd, distToPrint));
							break;
						default:
							drawLine("─", "─", nullptr, min(distToEnd, distToPrint));
						}
					}
					prev = LEFT;
				}
				else if(current.x > next.x)
				{
					if(next.x >= maxX) continue;
					

					int distToEnd = maxX - next.x;
					int distToPrint = current.x - 1 - next.x;

					if(current.y < maxY)
					{
						move(current.y, next.x);
						//if(current.x <= maxX)
						switch(prev)
						{
						case TOP:
							drawLine(nullptr, "─", "┘", min(distToEnd, distToPrint) + 1);
							break;
						case BOTTOM:
							drawLine(nullptr, "─", "┐", min(distToEnd, distToPrint) + 1);
							break;
						default:
							drawLine(nullptr, "─", "─", min(distToEnd, distToPrint) + 1);
						}
				
					
					}
					prev = RIGHT;
					if(next.x >= maxX) continue;
				}

				
				//current is lower
				if(current.y > next.y)
				{
					int topToBot  = current.y - next.y;
					int distToBot = maxY - next.y; 
					
					if(distToBot < 1) continue;
						
					move(next.y, next.x);
					switch(prev)
					{
					case LEFT:
						drawVerticalLineFromTop(nullptr, "│", current.y < maxY ? "┘" : nullptr, min(topToBot, distToBot));
						break;
					case RIGHT:
						drawVerticalLineFromTop(nullptr, "│", current.y < maxY ? "└" : nullptr, min(topToBot, distToBot));
						break;
					default:
						drawVerticalLineFromTop(nullptr, "│", current.y < maxY ? "│" : nullptr, min(topToBot, distToBot));
					}

					prev = BOTTOM;
				}	
				//current is higher
				else if(current.y < next.y)
				{
					int topToBot  = next.y - current.y;
					int distToBot = maxY - current.y; 
				
					if(distToBot < 1) continue;
				
					move(current.y, next.x);
					switch(prev)
					{
					case LEFT:
						drawVerticalLineFromTop("┐", "│", nullptr, min(topToBot, distToBot));
						break;
					case RIGHT:
						drawVerticalLineFromTop("┌", "│", nullptr, min(topToBot, distToBot));
						break;
					default:
						drawVerticalLineFromTop("│", "│", nullptr, min(topToBot, distToBot));
					}
			
					prev = TOP;
				}
				

				if(i == line.points.size() - 2)
				{
					move(next.y, next.x);
					if(next.y >= maxY - 1) break;

					if(prev == BOTTOM || prev == TOP)
					{
						printw("│");
					}
					else
					{
						printw("─");
					}
				}

			}// for points in line

			if(&line == choosedLine)
				attroff(COLOR_PAIR(CYAN_BLACK));
			else
				attroff(COLOR_PAIR(YELLOW_BLACK));
		} // for line in lines
		for(auto& text : texts)
		{
			if(text.pos.x + (int)text.content.length() >= maxX)
				continue;
			if(text.pos.y > maxY)
				continue;
			
			if(&text == choosedText)
				attron(COLOR_PAIR(CYAN_BLACK));
			else
				attron(COLOR_PAIR(WHITE_BLACK));
			
			move(text.pos.y, text.pos.x);
			printw(text.content.c_str());


			if(&text == choosedText)
				attroff(COLOR_PAIR(CYAN_BLACK));
			else
				attroff(COLOR_PAIR(WHITE_BLACK));

		} // for text in texts
		if(cursorPos.y < maxY && cursorPos.x < maxX)
		{
			curs_set(1);
			move(cursorPos.y, cursorPos.x);
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
		case KEY_UP:
			if(cursorPos.y == 0) break;

			if(choosedBox)       choosedBox->pos.y--;
			else if(choosedLine) choosedLine->points[linePoint].y--;
			else if(choosedText) choosedText->pos.y--;

			cursorPos.y--;
			break;
		case KEY_DOWN:
			if(choosedBox)       choosedBox->pos.y++;
			else if(choosedLine) choosedLine->points[linePoint].y++;
			else if(choosedText) choosedText->pos.y++;
			
			cursorPos.y++;
			break;
		case KEY_LEFT:
			if(cursorPos.x == 0) break;
			
			if(choosedBox)       choosedBox->pos.x--;
			else if(choosedLine) choosedLine->points[linePoint].x--;
			else if(choosedText) choosedText->pos.x--;
			
			cursorPos.x--;
			break;
		case KEY_RIGHT:
			if(choosedBox)       choosedBox->pos.x++;
			else if(choosedLine) choosedLine->points[linePoint].x++;
			else if(choosedText) choosedText->pos.x++;
			
			cursorPos.x++;
			break;
		//=============
		//choosing 
		case KEY_F(1):
			//null everything
			choosedBox  = nullptr;
			choosedLine = nullptr;
			choosedText = nullptr;

			//first check for boxes
			if((choosedBox = findBoxWithPos(boxes, cursorPos)) != nullptr)
				break;
			
			//then line
			if((choosedLine = findLineWithPos(lines, cursorPos, linePoint)) != nullptr)
				break;
			
			//finally texts
			choosedText = findTextWithPos(texts, cursorPos);
			//if not needed
			break;
		//-------------
		//unchoosing
		case KEY_F(2):
			choosedBox  = nullptr;
			choosedLine = nullptr;
			choosedText = nullptr;
			
			linePoint = -1;
			
			break;
		//-------------
		//destroying
F3:
		case KEY_F(3):
			if(choosedBox)
			{
				int index = find<Box>(boxes, choosedBox);
				boxes.erase(boxes.begin() + index);
				choosedBox = nullptr;
				break;
			}
			if(choosedLine)
			{
				int index = find<Line>(lines, choosedLine);
				lines.erase(lines.begin() + index);
				choosedLine = nullptr;
				linePoint = -1;
				break;
			}
			if(choosedText)
			{
				int index = find<Text>(texts, choosedText);
				texts.erase(texts.begin() + index);
				choosedText = nullptr;
				break;
			}

			break;
		//=============
		//creating
		//-------------
		//box
		case KEY_F(4):
			if(choosedBox || choosedLine || choosedText)
				break;

			boxes.emplace_back(cursorPos, 4, 3);

			choosedBox = &boxes.back();
			break;
		//-------------
		//line
		case KEY_F(5):
			if(choosedBox || choosedLine || choosedText)
				break;
			
			lines.emplace_back(cursorPos, cursorPos + vec2(2, 0));

			choosedLine = &lines.back();
			linePoint = 0;
			break;
		//-------------
		//text
		case KEY_F(6):
			if(choosedBox || choosedLine || choosedText)
				break;

			texts.emplace_back(cursorPos, "text");

			choosedText = &texts.back();
			break;
		
		//=============
		//specific
		//-------------
		//changing box size
		case '[':
			if(choosedBox)
			{
				if(choosedBox->height > 2)
					choosedBox->height--;
			
				break;
			}
			else goto def;
		case ']':
			if(choosedBox)
			{
				choosedBox->height++;
				
				break;
			}
			else goto def;
		case '{':
			if(choosedBox)
			{
				if(choosedBox->width > 2)
					choosedBox->width--;
				
				break;
			}
			else goto def;
		case '}':
			if(choosedBox)
			{
				choosedBox->width++;
				
				break;
			}
			else goto def;
		//-------------
		//adding and removing line points
		case KEY_F(7):
			if(!choosedLine) break;
			
			linePoint++;
			choosedLine->points.insert(choosedLine->points.begin() + linePoint, cursorPos);

			break;
		case KEY_F(8):
			if(!choosedLine) break;

			//less than two points so delete
			if(choosedLine->points.size() < 3) goto F3;
		
			choosedLine->points.erase(choosedLine->points.begin() + linePoint);
			
			choosedLine = nullptr;
			linePoint = -1;
			
			break;
		//-------------
		//removing text
		case KEY_BACKSPACE:
			if(choosedText && choosedText->content.size() > 0)
				choosedText->content.pop_back();
			
			break;
		//-------------
		//adding text
def:
		default:
			if(choosedText && 
			   c > 31 && c < 127)
			   choosedText->content += static_cast<char>(c);
			else if(c == 'Q')
				goto end;	
			else if(c == 'w')
			{
				std::ofstream write(fileName);

				//at most 200 chars per line
				char arr[200];
				//at most 100 y lines
				for(int i = 0; i < 100; i++)
				{
					move(i, 0);
					innstr(arr, 200);

					write << arr << '\n';	
				}
			}
		}
	}
end:
	endwin();

	return 0;
}
