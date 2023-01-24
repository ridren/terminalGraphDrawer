#include "parts.h"

vec2::vec2()
	: x(0), y(0) {}

vec2::vec2(const int n_x, const int n_y)
	: x(n_x), y(n_y) {}

vec2 vec2::operator+(const vec2& other) const
{
	return {x + other.x, y + other.y};
}
vec2 vec2::operator-(const vec2& other) const 
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

bool vec2::operator==(const vec2& other) const
{
	return x == other.x && y == other.y;
}
bool vec2::operator!=(const vec2& other) const 
{
	return x != other.x || y != other.y;
}
	
Box::Box() {}
Box::Box(const vec2& n_pos, int n_width, int n_height)
	: pos(n_pos), width(n_width), height(n_height) {}

Line::Line() {}
Line::Line(const vec2& start, const vec2& end)
{
	points.emplace_back(start);
	points.emplace_back(end);

	ends_with_arrow = false;
}
	
Text::Text() {}
Text::Text(const vec2& n_pos, const std::string& n_content)
	: pos(n_pos), content(n_content) {}
Text::Text(const vec2& n_pos, const char* n_content)
	: pos(n_pos), content(n_content) {}

Box* find_Box_With_Pos(std::vector<Box>& v, const vec2& pos)
{
	for(unsigned int i = 0; i < v.size(); i++)
	{
		if(v[i].pos == pos) return &v[i];
	}
	return nullptr;
}
Line* find_Line_With_Pos(std::vector<Line>& v, const vec2& pos, int& index)
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
Text* find_Text_With_Pos(std::vector<Text>& v, const vec2& pos)
{
	for(unsigned int i = 0; i < v.size(); i++)
	{
		if(v[i].pos == pos) return &v[i];
	}
	return nullptr;
}
