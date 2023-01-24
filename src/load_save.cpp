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
