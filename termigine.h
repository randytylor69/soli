#ifndef TERMIGINE_H_
#define TERMIGINE_H_

#include <termios.h>
#include <string>
#include <vector>

struct Vec3{
    float x, y, z;
    Vec3() {}
    Vec3(float xi, float yi, float zi)
	:x(xi), y(yi), z(zi){}
};

struct Vec2 {
    float x, y;
    Vec2() {}
    Vec2(const float &xi, const float &yi)
	: x(xi), y(yi) {}
};

class Engine
{
    private:
	termios tio;
    public:
	/* terminal methods */
	void setCanonicalAndCursor(const int &cmd);
	float getTerminalHeight();
	float getTerminalWidth();
		
	/* rendering methods */
	void print(const std::string &input, const int &x, const int &y);
	void clearScreen();
	Vec3 getCrossProduct(Vec3 v, Vec3 w);
	float getDotProduct(Vec3 v, Vec3 w);
	Vec3 getVectorBetweenTwoVertices(Vec3 v, Vec3 w);

	/* drawing methods */
	void drawScanLine(int y, int x0, int x1, char symbol);
	void drawFlatBottomTriangle(Vec2 top, Vec2 bottom1, Vec2 bottom2, char symbol);
	void drawFlatTopTriangle(Vec2 bottom, Vec2 top1, Vec2 top2, char symbol);
	void drawTriangle(Vec2 inputV0, Vec2 inputV1, Vec2 inputV2, char symbol);
	std::vector<std::vector<char>> drawTerminalBorder(char symbol = 'D');

};


#endif
