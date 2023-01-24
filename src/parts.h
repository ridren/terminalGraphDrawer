#ifndef PARTS_H
#define PARTS_H

#include <vector>
#include <string>

struct vec2{
	int x;
	int y;

	vec2();
	vec2(const int n_x, const int n_y);

	vec2 operator+(const vec2& other)  const;
	vec2 operator-(const vec2& other)  const;

	void operator+=(const vec2& other);
	void operator-=(const vec2& other);

	bool operator==(const vec2& other) const;
	bool operator!=(const vec2& other) const;
};

struct Box{
	vec2 pos;
	int width;
	int height;

	constexpr static const char* corner_TL = "╔";
	constexpr static const char* corner_TR = "╗";
	constexpr static const char* corner_BL = "╚";
	constexpr static const char* corner_BR = "╝";
	constexpr static const char* straight  = "═";
	constexpr static const char* vertical  = "║";

	Box();
	Box(const vec2& n_pos, int n_width, int n_height);
};

struct Line{
	std::vector<vec2> points;

	bool ends_with_arrow;

	Line();
	Line(const vec2& start, const vec2& end);
};

struct Text{
	vec2 pos;
	std::string content;

	Text();
	Text(const vec2& n_pos, const std::string& n_content);
	Text(const vec2& n_pos, const char* n_content);
};

Box*  find_Box_With_Pos (std::vector<Box>& v,  const vec2& pos);
Line* find_Line_With_Pos(std::vector<Line>& v, const vec2& pos, int& index);
Text* find_Text_With_Pos(std::vector<Text>& v, const vec2& pos);

template<typename T>
int find(std::vector<T>& v, T* element)
{
	for(unsigned int i = 0; i < v.size(); i++)
	{
		if(&v[i] == element) return static_cast<int>(i);
	}

	return -1;
}


#endif
