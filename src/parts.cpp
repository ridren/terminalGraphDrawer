#include "parts.h"

vec2::vec2()
	: x(0), y(0) {}

vec2::vec2(int nX, int nY)
	: x(nX), y(nY) {}

vec2 vec2::operator+(const vec2& other)
{
	return {x + other.x, y + other.y};
}
vec2 vec2::operator-(const vec2& other)
{
	return {x - other.x, y - other.y};
}

void vec2::operator+=(const vec2& other)
{
	x += other.x;
	y += other.y;
}
void vec2::operator-=(const vec2& other)
{
	x -= other.x;
	y -= other.y;
}

bool vec2::operator==(const vec2& other)
{
	return x == other.x && y == other.y;
}
bool vec2::operator!=(const vec2& other)
{
	return x != other.x || y != other.y;
}
	
Box::Box() {}
Box::Box(const vec2& nPos, int nWidth, int nHeight)
	: pos(nPos), width(nWidth), height(nHeight) {}

Line::Line() {}
Line::Line(const vec2& start, const vec2& end)
{
	points.emplace_back(start);
	points.emplace_back(end);

	endsWithArrow = false;
}
	
Text::Text() {}
Text::Text(vec2 nPos, const std::string& nContent)
	: pos(nPos), content(nContent) {}
Text::Text(vec2 nPos, const char* nContent)
	: pos(nPos), content(nContent) {}

Box* findBoxAtPos(std::vector<Box>& v, vec2 pos)
{
	for(Box& box : v)
	{
		if(box.pos.x <= pos.x && pos.x <= box.pos.x+box.width && 
		   box.pos.y <= pos.y && pos.y <= box.pos.y+box.height)
			return &box;
	}
	return nullptr;
}

Box* findBoxWithPos(std::vector<Box>& v, vec2 pos)
{
	for(unsigned int i = 0; i < v.size(); i++)
	{
		if(v[i].pos == pos) return &v[i];
	}
	return nullptr;
}
Line* findLineWithPos(std::vector<Line>& v, vec2 pos, int& index)
{
	for(unsigned int i = 0; i < v.size(); i++)
	{
		for(unsigned int j = 0; j < v[i].points.size(); j++)
		{
			if(v[i].points[j] == pos)
			{
				index = static_cast<int>(j);
				return &v[i];
			}
		}
	}	
	index = -1;
	return nullptr;
}
Text* findTextWithPos(std::vector<Text>& v, vec2 pos)
{
	for(unsigned int i = 0; i < v.size(); i++)
	{
		if(v[i].pos == pos) return &v[i];
	}
	return nullptr;
}
