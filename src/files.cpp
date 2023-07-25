#include <ncurses.h>

#include <fstream>

#include <vector>
#include <string>

#include "parts.h"

bool load_File(const std::string& file_to_read, 
               std::vector<Box>&  boxes, 
               std::vector<Line>& lines, 
               std::vector<Text>& texts)
{
	std::ifstream read(file_to_read);

	if(!read.good())
	{
		return false;
	}

	std::string line;
	
	while(std::getline(read, line))
	{
		if(line.size() == 0) continue;
		
		switch(line[0])
		{
		case '#':
			continue;
		case 'B':
		{
			//x, y, width, height
			int vals[4];
			
			int index = 0;
			std::string to_convert = "";
			for(unsigned int i = 2; i < line.length(); i++)
			{
				if(line[i] == ',')
				{
					vals[index] = std::stoi(to_convert);
					to_convert.clear();
					index++;
					continue;
				}

				to_convert += line[i];
			}
			vals[index] = std::stoi(to_convert);
		
			boxes.emplace_back(vec2(vals[0], vals[1]), vals[2], vals[3]);
			break;
		}
		case 'L':
		{
			std::vector<int> vals;
			bool ends_with_arrow = ('t' == line[2] ? true : false);

			std::string to_convert = "";
			for(unsigned int i = 3; i < line.length(); i++)
			{
				if(line[i] == ',')
				{
					vals.emplace_back(std::stoi(to_convert));
					to_convert.clear();
					continue;
				}

				to_convert += line[i];
			}
			vals.emplace_back(std::stoi(to_convert));
		
			lines.emplace_back(vec2(vals[0], vals[1]), vec2(vals[2], vals[3]));

			for(unsigned int i = 4; i < vals.size(); i++)
			{
				lines.back().points.emplace_back(vec2(vals[i], vals[i + 1]));

				i++;
			}
			lines.back().ends_with_arrow = ends_with_arrow;
			break;
		}
		case 'T':
		{
			//x, y
			int vals[2];
			std::string to_convert = "";
			int index = 0;
			
			for(unsigned int i = 2; i < line.length(); i++)
			{
				if(line[i] == ',')
				{
					vals[index] = std::stoi(to_convert);
					to_convert.clear();
					index++;
					if(index == 2)
					{
						i++; // to ommit comma
						to_convert = line.substr(i);
						break;
					}

					continue;
				}

				to_convert += line[i];

			}

			texts.emplace_back(vec2(vals[0], vals[1]), to_convert);

			break;
		}
		}
	}
			
	return true;
}

void save_To_File(const std::string& file_to_write, 
                  std::vector<Box>&  boxes, 
                  std::vector<Line>& lines, 
                  std::vector<Text>& texts)
{
	std::ofstream write(file_to_write);

	for(auto& box : boxes)
	{
		write << "B ";
		write << box.pos.x  << ',';
		write << box.pos.y  << ',';
		write << box.width  << ',';
		write << box.height;
	
		write << '\n';
	}
	for(auto& line : lines)
	{
		write << "L ";
		write << (line.ends_with_arrow ? 't' : 'f');
		for(auto& point : line.points)
		{
			write << point.x << ',';
			write << point.y;

			if(&point != &line.points.back())
				write << ',';
		}

		write << '\n';
	}
	for(auto& text : texts)
	{
		write << "T ";
		write << text.pos.x  << ',';
		write << text.pos.y  << ',';
		write << text.content;

		write << '\n';
	}
}

//SHARED BETWEEN FILES 
#define COLOR_BOX  1
#define COLOR_LINE 2
#define COLOR_TEXT 3
#define COLOR_CHOS 4
#define COLOR_COMM 5
#define PAIR_BOX   1 
#define PAIR_LINE  2
#define PAIR_TEXT  3
#define PAIR_CHOS  4
#define PAIR_COMM  5

void set_Color(const std::string& str, const int col_id, const int pair_id);
void load_Config()
{
	std::string home_dir = std::string(getenv("HOME"))
	                     + "/.config/TBDW/config.txt";
	
	std::fstream config;
	config.open(home_dir, std::ios_base::in);

	
	//create default file in case it is empty
	if(!config.is_open())
	{
		config.open(home_dir, std::ios_base::out);
		
		config << "0 1000 0\n"
		          "1000 1000 0\n"
		          "1000 1000 1000\n"
		          "0 1000 1000\n"
		          "1000 0 0\n";
		
		config.close();
		config.open(home_dir, std::ios_base::in);
	}
	
	
	std::string line;
	std::getline(config, line);
	set_Color(line, COLOR_BOX,  PAIR_BOX);
	std::getline(config, line);
	set_Color(line, COLOR_LINE, PAIR_LINE);
	std::getline(config, line);
	set_Color(line, COLOR_TEXT, PAIR_TEXT);
	std::getline(config, line);
	set_Color(line, COLOR_CHOS, PAIR_CHOS);
	std::getline(config, line);
	set_Color(line, COLOR_COMM, PAIR_COMM);
	config.close();
}

void set_Color(const std::string& str, const int col_id, const int pair_id)
{
	int ind = 0;
	int col[] = {0, 0, 0};
	for(const char c : str)
	{
		if(c == ' ')
			ind++;
		else
		{
			col[ind] *= 10;
			col[ind] += c - '0';
		}
	}
	//std::cout << col[0] << ' ' << col[1] << ' ' << col[2] << '\n';

	
	init_color(col_id, col[0], col[1], col[2]);
	init_pair(pair_id, col_id, COLOR_BLACK);
}

