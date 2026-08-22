#include "termigine.h"
#include <stdio.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <print>
#include <vector>
#include <algorithm>
#include <cmath>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <iostream>

/* terminal methods */
void Engine::setCanonicalAndCursor(const int &cmd)
{
    /* canonical changes */
    tcgetattr(STDIN_FILENO, &tio); // get current terminal setting
    cmd== 0 ? 
	tio.c_lflag &= ~(ICANON | ECHO) : // disable canonical mode
	tio.c_lflag |= (ICANON | ECHO); // enable canonical mode
    tcsetattr(STDIN_FILENO,TCSANOW, &tio); // apply changes
    
    /* cursor changes */
    cmd == 0 ? std::print("\033[?25l") : std::print("\033[?25h");
}
float Engine::getTerminalHeight()
{
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return (float)size.ws_row;
}
float Engine::getTerminalWidth()
{
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    return (float)size.ws_col;
}

/* rendering methods */
void Engine::print(const std::string &input, const int &x, const int &y)
{
    std::print("\033[{};{}H", y, x); // reversed cuz the syntax is {ROW};{COL}
    std::print("{}", input);
    fflush(stdout);
}
void Engine::clearScreen()
{
    std::print("\033[2J"); // clear screen
    std::print("\033[1;1H"); // move cursor to (1,1)
}

Vec3 Engine::getCrossProduct(Vec3 v, Vec3 w)
{
    return Vec3(
	v.y * w.z - v.z * w.y,
	-(v.x * w.z - v.z * w.x),
	v.x * w.y- v.y * w.x
    );

}

float Engine::getDotProduct(Vec3 v, Vec3 w)
{
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

Vec3 Engine::getVectorBetweenTwoVertices(Vec3 v, Vec3 w)
{
    return Vec3(
	    w.x - v.x,
	    w.y - v.y,
	    w.z - v.z);
}


/* draws a straight horizontal line between two points, x0 and x1. */
void Engine::drawScanLine(int y, int x0, int x1, char symbol)
{
    // first we need to figure out the start and the end
    int left {x0};
    int right {x1};
    if (left > right)
    {
	left = x1; 
	right = x0;
    }
    // then draw the line
    for (int i = left; i <= right; i++)
    {
	std::string s(1, symbol); 
	print(s, i, y);
    }
}

void Engine::drawFlatBottomTriangle(Vec2 top, Vec2 bottom1, Vec2 bottom2, char symbol)
{
    Vec2 pStart = top;
    Vec2 pEnd = top;
    while (pStart.y != bottom1.y) {

	drawScanLine(pStart.y, pStart.x, pEnd.x, symbol);

	/* modify pStart */
	pStart.y += 1;
	pStart.x += (bottom1.x - top.x) / (bottom1.y - top.y);

	/* modify pEnd */
	pEnd.y += 1;
	pEnd.x += (bottom2.x - top.x) / (bottom2.y - top.y);
    }
    drawScanLine(pStart.y, pStart.x, pEnd.x, symbol);
}

void Engine::drawFlatTopTriangle(Vec2 bottom, Vec2 top1, Vec2 top2, char symbol)
{
    Vec2 pStart = bottom;
    Vec2 pEnd = bottom;
    while (pStart.y != top1.y) {

	drawScanLine(pStart.y, pStart.x, pEnd.x, symbol);

	/* modify pStart */
	pStart.y -= 1;
	pStart.x -= (bottom.x - top1.x) / (bottom.y - top1.y);

	/* modify pEnd */
	pEnd.y -= 1;
	pEnd.x -= (bottom.x - top2.x) / (bottom.y - top2.y);
    }
    drawScanLine(pStart.y, pStart.x, pEnd.x, symbol);
}

void Engine::drawTriangle(Vec2 inputV0, Vec2 inputV1, Vec2 inputV2, char symbol)
{
    /* first sort the vertices by ascending Y */
    std::vector<Vec2> sortedVertices = {inputV0, inputV1, inputV2};
    sort(sortedVertices.begin(), sortedVertices.end(), 
	[](const auto &L, const auto &R) {return L.y < R.y;}
    );
    Vec2 v0 = sortedVertices[0];
    Vec2 v1 = sortedVertices[1];
    Vec2 v2 = sortedVertices[2];

    /* compute the middle point using right angled triangles */
    Vec2 midpoint = {0, 0};
    midpoint.y = v1.y;

    midpoint.x = v0.x + (v2.x - v0.x) / (v2.y - v0.y) * (v1.y - v0.y);

    /* split the triangle into 2 and use both functions to draw it */
    drawFlatBottomTriangle(v0, midpoint, v1, symbol);
    drawFlatTopTriangle(v2, midpoint, v1, symbol);
}

std::vector<std::vector<char>> Engine::drawTerminalBorder(char symbol)
{
    const int TER_COLS = getTerminalWidth();
    const int TER_ROWS = getTerminalHeight();
    std::string s(1, symbol);
    std::vector<std::vector<char>> m {{}};

    for (int i=1; i<=TER_ROWS; i++)
    {
	m.push_back({' '});
	for (int j=1; j<=TER_COLS; j++)
	{
	    if (i==1 || i==TER_ROWS)
	    { // top or bottom row
		print(symbol == 'D' ? "-" : s, j, i);
		m[i].push_back(symbol=='D' ? '-' : symbol);
	    } else if (j==1 || j==TER_COLS)
	    { // side col
		print(symbol == 'D' ? "|" : s, j, i);
		m[i].push_back(symbol=='D' ? '|' : symbol);
	    } else {
		m[i].push_back(' ');
	    }
	}
    }
    return m;
}
