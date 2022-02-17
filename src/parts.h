#ifndef PARTS_H
#define PARTS_H

#include <vector>
#include <string>

struct vec2{
	int x;
	int y;

	vec2();
	vec2(int nX, int nY);

	vec2 operator+(const vec2& other);
	vec2 operator-(const vec2& other);

	void operator+=(const vec2& other);
	void operator-=(const vec2& other);

	bool operator==(const vec2& other);
	bool operator!=(const vec2& other);
};

struct Box{
	vec2 pos;
	int width;
	int height;

	const char* TLCorner = "╔";
	const char* TRCorner = "╗";
	const char* BLCorner = "╚";
	const char* BRCorner = "╝";
	const char* straight = "═";
	const char* vertical = "║";

	Box();
	Box(const vec2& nPos, int nWidth, int nHeight);
};

struct Line{
	std::vector<vec2> points;

	bool endsWithArrow;

	Line();
	Line(const vec2& start, const vec2& end);
};

struct Text{
	vec2 pos;
	std::string content;

	Text();
	Text(vec2 nPos, const std::string& nContent);
	Text(vec2 nPos, const char* nContent);
};

Box* findBoxWithPos(std::vector<Box>& v, vec2 pos);
Line* findLineWithPos(std::vector<Line>& v, vec2 pos, int& index);
Text* findTextWithPos(std::vector<Text>& v, vec2 pos);

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
