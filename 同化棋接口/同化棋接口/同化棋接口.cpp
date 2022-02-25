#include <cmath>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

int currBotColor; // ����ִ����ɫ��1Ϊ�ڣ�-1Ϊ�ף�����״̬��ͬ��
int gridInfo[7][7] = { 0 }; // ��x��y����¼����״̬
int blackPieceCount = 2, whitePieceCount = 2;
int startX, startY, resultX, resultY;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };

struct coord {
	int x1 = -1;
	int y1 = -1;
	int x2 = -1;
	int y2 = -1;
};
double weight[7][7] = {
	{ 1.0, 0.965051, 0.849347, 0.803681, 0.779659, 0.789668, 1.0, },
	{ 0.965051, 0.87366, 0.835621, 0.743914, 0.715635, 0.719026, 0.789668, },
	{ 0.849347, 0.835621, 0.788361, 0.701912, 0.677423, 0.715635, 0.779659, },
	{ 0.803681, 0.743914, 0.701912, 0.70752, 0.701912, 0.743914, 0.803681, },
	{ 0.779659, 0.715635, 0.677423, 0.701912, 0.788361, 0.835621, 0.849347, },
	{ 0.789668, 0.719026, 0.715635, 0.743914, 0.835621, 0.87366, 0.965051, },
	{ 1.0, 0.789668, 0.779659, 0.803681, 0.849347, 0.965051, 1.0, }
};

int dx[24] = { -1,0,1,1,1,0,-1,-1 ,-2,-1,0,1,2,2,2,2,2,1,0,-1,-2,-2,-2,-2 };
int dy[24] = { -1,-1,-1,0,1,1,1,0,-2,-2,-2,-2,-2,-1,0,1,2,2,2,2,2,1,0,-1 };

struct Move {
	int s1;
	int s2;
	int r1;
	int r2;
	int board[7][7] = { 0 };
};//��¼ÿһ���ƶ������״̬ 
Move makemove[5];

// �ж��Ƿ��ڵ�ͼ��
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

// ��Direction����Ķ����꣬�������Ƿ�Խ��
inline bool MoveStep(int& x, int& y, int Direction)
{
	x = x + delta[Direction][0];
	y = y + delta[Direction][1];
	return inMap(x, y);
}

// �����괦���ӣ�����Ƿ�Ϸ���ģ������
bool ProcStep(int x0, int y0, int x1, int y1, int color)
{
	if (color == 0)
		return false;
	if (x1 == -1) // ��·���ߣ������˻غ�
		return true;
	if (!inMap(x0, y0) || !inMap(x1, y1)) // �����߽�
		return false;
	if (gridInfo[x0][y0] != color)
		return false;
	int dx, dy, x, y, currCount = 0, dir;
	int effectivePoints[8][2];
	dx = abs((x0 - x1)), dy = abs((y0 - y1));
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2) // ��֤�����ƶ���ԭ��λ�ã������ƶ�ʼ����5��5������
		return false;
	if (gridInfo[x1][y1] != 0) // ��֤�ƶ�����λ��Ϊ��
		return false;
	if (dx == 2 || dy == 2) // ����ߵ���5��5����Χ�����Ǹ���ճ��
		gridInfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackPieceCount++;
		else
			whitePieceCount++;
	}

	gridInfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++) // Ӱ���ڽ�8��λ��
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inMap(x, y))
			continue;
		if (gridInfo[x][y] == -color)
		{
			effectivePoints[currCount][0] = x;
			effectivePoints[currCount][1] = y;
			currCount++;
			gridInfo[x][y] = color;
		}
	}
	if (currCount != 0)
	{
		if (color == 1)
		{
			blackPieceCount += currCount;
			whitePieceCount -= currCount;
		}
		else
		{
			whitePieceCount += currCount;
			blackPieceCount -= currCount;
		}
	}
	return true;
}



int value(int player, int depth) {
	int sum = 0;
	for (int i = 0; i <= 6; i++) {
		for (int j = 0; j <= 6; j++) {
			sum += makemove[depth].board[i][j];
		}
	}
	return player * sum;
}

bool CanMove(int player, int depth)//�ж��ܷ��ƶ� 
{
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (makemove[depth].board[i][j] == player) {
				for (int k = 0; k < 24; k++) {
					if (inMap(i + dy[k], j + dx[k])) {
						if (makemove[depth].board[i + dy[k]][j + dx[k]] == 0) {
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void MoveTry(int s1, int r1, int s2, int r2, int player, int depth)//��(s1,r1)�䵽(s2,r2)�� 
{
	makemove[depth].board[r2][s2] = player; 	//�ж������� 	
	if (abs(r1 - r2) == 2 || abs(s1 - s2) == 2) {
		makemove[depth].board[r1][s1] = 0;
	}
	//ͬ�� 	
	for (int i = 0; i < 8; i++) {
		if (inMap(s2 + dx[i], r2 + dy[i])) {
			if (makemove[depth].board[r2 + dy[i]][s2 + dx[i]] == -player) {
				makemove[depth].board[r2 + dy[i]][s2 + dx[i]] = player;
			}
		}
	}
	return;
}









int MoveValue(int player, int depth, int down, int up)//������ǰ״̬������ 
{
	int max = -10000;
	if (depth == 5)//��������������+1 		
		return value(player, depth - 1);
	if (depth == 0)//��ʼ���ݴ����� 	
	{
		for (int m = 0; m < 7; m++) {
			for (int n = 0; n < 7; n++) {
				makemove[0].board[m][n] = gridInfo[m][n];
				MoveValue(player, depth + 1, down, up);
			}
		}
		return 0;
	} 	//����� 	
	for (int m = 0; m < 7; m++) {
		for (int n = 0; n < 7; n++) {
			makemove[depth].board[m][n] = makemove[depth - 1].board[m][n];
		}
	}
	if (!CanMove(player, depth))//����������� 	
	{
		if (!CanMove(-player, depth))
			return value(player, depth);
		return -MoveValue(-player, depth + 1, -up, -down);
	} 	//��λ������ 	
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (makemove[depth].board[i][j] == player) {
				for (int k = 0; k < 24; k++) {
					if (inMap(i + dy[k], j + dx[k])) {
						if (makemove[depth].board[i + dy[k]][j + dx[k]] == 0) {
							MoveTry(j, i, j + dx[k], i + dy[k], player, depth);
							int val = -MoveValue(-player, depth + 1, -up, -down);
							//���С��һʱ���м�֦ 							
							if (depth > 1) {
								if (val >= up) {//��֦
									return up;
								}
								if (val > down)
								{
									down = val;
								}
							}
							if (val > max) {
								max = val;
								if (depth == 1) {
									startX = i;
									startY = j;
									resultX = i + dy[k];
									resultY = j + dx[k];
								}
							}
							//��ԭ���̣�����
							for (int m = 0; m < 7; m++) {
								for (int n = 0; n < 7; n++) {
									makemove[depth].board[m][n] = makemove[depth - 1].board[m][n];
								}
							}
						}
					}
				}
			}
		}
	}
	return max;
}

int main()
{
	istream::sync_with_stdio(false);

	int x0, y0, x1, y1;

	// ��ʼ������
	gridInfo[0][0] = gridInfo[6][6] = 1;  //|��|��|
	gridInfo[6][0] = gridInfo[0][6] = -1; //|��|��|

										  // �����Լ��յ���������Լ���������������ָ�״̬
	int turnID;
	currBotColor = -1; // ��һ�غ��յ�������-1, -1��˵�����Ǻڷ�
	cin >> turnID;
	for (int i = 0; i < turnID - 1; i++)
	{
		// ������Щ��������𽥻ָ�״̬����ǰ�غ�
		cin >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, -currBotColor); // ģ��Է�����
		else
			currBotColor = 1;
		cin >> x0 >> y0 >> x1 >> y1;
		if (x1 >= 0)
			ProcStep(x0, y0, x1, y1, currBotColor); // ģ�⼺������
	}

	// �����Լ����غ�����
	cin >> x0 >> y0 >> x1 >> y1;
	if (x1 >= 0)
		ProcStep(x0, y0, x1, y1, -currBotColor); // ģ��Է�����
	else
		currBotColor = 1;

	// �������ߣ���ֻ���޸����²��֣�
	startX = -1;
	startY = -1;
	resultX = -1;
	resultY = -1;
	MoveValue(currBotColor, 0, -10000, 10000);
	// ���߽���������������ֻ���޸����ϲ��֣�

	cout << startX << " " << startY << " " << resultX << " " << resultY;
	return 0;
}