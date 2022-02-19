#include <fstream>

#include <vector>
#include <string>

#include "parts.h"

bool loadFile(const std::string& fileToRead, 
              std::vector<Box>& boxes, 
              std::vector<Line>& lines, 
              std::vector<Text>& texts)
{
	std::ifstream read(fileToRead);

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
			std::string toConvert = "";
			for(unsigned int i = 2; i < line.length(); i++)
			{
				if(line[i] == ',')
				{
					vals[index] = std::stoi(toConvert);
					toConvert.clear();
					index++;
					continue;
				}

				toConvert += line[i];
			}
			vals[index] = std::stoi(toConvert);
		
			boxes.emplace_back(vec2(vals[0], vals[1]), vals[2], vals[3]);
			break;
		}
		case 'L':
		{
			std::vector<int> vals;
			bool endsWithArrow = ('t' == line[2] ? true : false);

			std::string toConvert = "";
			for(unsigned int i = 3; i < line.length(); i++)
			{
				if(line[i] == ',')
				{
					vals.emplace_back(std::stoi(toConvert));
					toConvert.clear();
					continue;
				}

				toConvert += line[i];
			}
			vals.emplace_back(std::stoi(toConvert));
		
			lines.emplace_back(vec2(vals[0], vals[1]), vec2(vals[2], vals[3]));

			for(unsigned int i = 4; i < vals.size(); i++)
			{
				lines.back().points.emplace_back(vec2(vals[i], vals[i + 1]));

				i++;
			}
			lines.back().endsWithArrow = endsWithArrow;
			break;
		}
		case 'T':
		{
			//x, y
			int vals[2];
			std::string toConvert = "";
			int index = 0;
			
			for(unsigned int i = 2; i < line.length(); i++)
			{
				if(line[i] == ',')
				{
					vals[index] = std::stoi(toConvert);
					toConvert.clear();
					index++;
					if(index == 2)
					{
						i++; // to ommit comma
						toConvert = line.substr(i);
						break;
					}

					continue;
				}

				toConvert += line[i];

			}

			texts.emplace_back(vec2(vals[0], vals[1]), toConvert);

			break;
		}
		}
	}
			
	return true;
}

void saveToFile(const std::string& fileToWrite, 
              std::vector<Box>& boxes, 
              std::vector<Line>& lines, 
              std::vector<Text>& texts)
{
	std::ofstream write(fileToWrite);

	write << "# you probably should not edit this file\n";

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
		write << (line.endsWithArrow ? 't' : 'f');
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
