#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
//玩家/玩家1棋子为@，电脑棋子/玩家2为*

#define NUM2 29
#define NUM1 15

using namespace std;

ofstream outFile;
ifstream inFile;

char board[NUM1][NUM2];// 棋盘
int gridInfo[7][7];//算法中的棋盘
int startX, startY, resultX, resultY;//AI移动方式
//权值矩阵
double weight[7][7] = {
	{ 1.00, 0.95, 0.85, 0.80, 0.80, 0.90, 1.00, },
	{ 0.95, 0.90, 0.83, 0.75, 0.72, 0.72, 0.79, },
	{ 0.85, 0.85, 0.80, 0.70, 0.67, 0.71, 0.78, },
	{ 0.80, 0.75, 0.72, 0.70, 0.70, 0.74, 0.80, },
	{ 0.75, 0.70, 0.67, 0.70, 0.78, 0.84, 0.85, },
	{ 0.80, 0.75, 0.75, 0.75, 0.84, 0.87, 0.97, },
	{ 1.00, 0.80, 0.78, 0.80, 0.85, 0.96, 1.00, }
};
//移动向量
int dx[24] = { -1, 0, 1, 1, 1, 0, -1, -1, -2, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2 };
int dy[24] = { -1, -1, -1, 0, 1, 1, 1, 0, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1 };

//记录每一次移动的相关状态 
struct Move {
	int s1;
	int s2;
	int r1;
	int r2;
	int board[7][7] = { 0 };
};

Move makemove[5];//记录深度搜索时的一条路径上的节点

void ShowMenu();//展示初始菜单
void NewBoard(char board[NUM1][NUM2]);//清空棋盘
void ShowBoard(char board[NUM1][NUM2]);//展示棋盘
bool UserPlay(char board[NUM1][NUM2]);//玩家落子的操作
bool Law(int x1, int y1, int x2, int y2, char board[NUM1][NUM2]);//判断玩家User移动是否符合规则
bool Law2(int x1, int y1, int x2, int y2, char board[NUM1][NUM2]);//判断玩家User2移动是否符合规则
bool ComLaw(int x2, int y2, char board[NUM1][NUM2]);//判断某位置是否在棋盘内
void Assimi(char board[NUM1][NUM2], char piece, int x2, int y2);//同化对方的棋子
void Play(char board[NUM1][NUM2]);//人机对战（人先手）游戏过程函数
void Play2(char board[NUM1][NUM2]);//人机对战（机先手）函数
void Playpvp(char board[NUM1][NUM2]);//人人对战
void ComPlay(char board[NUM1][NUM2]);//电脑落子
bool JudgeStep(char board[NUM1][NUM2], char piece);//判断能否再走
void Count(char board[NUM1][NUM2]);//计算棋子数并显示
void Countpvp(char board[NUM1][NUM2]);//统计pvp模式棋子数并显示
bool Result(char board[NUM1][NUM2]);//判断结果
bool Resultpvp(char board[NUM1][NUM2]);//判断人人对战结果
void Save(char board[NUM1][NUM2], char mode);//存档
void Read(char board[NUM1][NUM2], char &mode);//读档
//以下为Minmax及α-β剪枝与辅助函数
inline bool inMap(int x, int y);//判断是否在地图内
double value(int player, int depth);//评估当前局面价值
bool CanMove(int player, int depth);//能否移动
void MoveTry(int s1, int r1, int s2, int r2, int player, int depth);//尝试移动一步
void ChangeBoard(char board[NUM1][NUM2]);//将棋盘情况转化为算法中的情况
double MoveValue(int player, int depth, double alpha, double beta);//minmax剪枝

int main() {
	cout << endl;
	cout << "\t\t欢迎来到CarefreeSun的同化棋！\\(^o^)/" << endl;
	cout << "\t\t祝你玩的愉快！" << endl;
	while (true) {
		ShowMenu();
		char select;
		cin >> select;
		system("cls");
		switch (select) {
		case '1':
			cout << "\t\t请选择你想进行的对局模式:" << endl;
			cout << "\t\t1.人机对战（人先手）" << endl;
			cout << "\t\t2.人机对战（机先手）" << endl;
			cout << "\t\t3.人人对战" << endl;
			char postselect;
			cin >> postselect;
			switch (postselect) {
			case '1':
				NewBoard(board);
				Play(board);
				break;
			case '2':
				NewBoard(board);
				ComPlay(board);
				Play2(board);
				break;
			case '3':
				NewBoard(board);
				Playpvp(board);
				break;
			default:
				break;
			}
			break;
		//读取存档
		case '2':
			NewBoard(board);
			char mode;
			Read(board, mode);
			switch (mode) {
			case '1':
				Play(board);
				break;
			case '2':
				Play2(board);
				break;
			case '3':
				Playpvp(board);
				break;
			}
			break;
		//直接退出
		case '3':
			cout << endl;
			cout << "\t\t欢迎您下次再玩！再见！";
			return 0;
			break;
		default:
			cout << "\t\t您的输入我读不懂，请再次输入！" << endl;
			break;
		}
	}
	system("pause");
	return 0;
}

// 展示初始菜单
void ShowMenu() {
	cout << "\t\t选择你想进行的操作：" << endl;
	cout << "\t\t1.新的开始" << endl;
	cout << "\t\t2.读取存档" << endl;
	cout << "\t\t3.退出游戏" << endl;
	cout << "\t\t请输入您的选择：";
}

// 空棋盘
void NewBoard(char board[NUM1][NUM2]) {
	board[0][0] = board[0][NUM2 - 1] = board[NUM1 - 1][0] = board[NUM1 - 1][NUM2 - 1] = '#';
	for (int i = 1; i <= NUM1 - 2; i++) {
		board[i][0] = board[i][NUM2 - 1] = '|';
		if (i % 2 == 1) {
			for (int j = 1; j <= NUM2 - 2; j++) {
				if (j % 4 == 0) board[i][j] = '|';
				else board[i][j] = ' ';
			}
		}
		else {
			board[i][0] = board[i][NUM2 - 2] = '|';
			for (int j = 1; j <= NUM2 - 2; j++) {
				if (j % 4 == 0) board[i][j] = '+';
				else board[i][j] = '-';
			}
		}
	}
	for (int j = 1; j <= NUM2 - 2; j++) {
		board[0][j] = board[NUM1 - 1][j] = '-';
	}
	board[1][2] = board[13][26] = '@';
	board[1][26] = board[13][2] = '*';
}

// 显示棋盘
void ShowBoard(char board[NUM1][NUM2]) {
	cout << setw(32) << "y轴" << endl;
	cout << setw(19) << 1;
	for (int i = 2; i <= 7; i++) {
		cout << setw(4) << i;
	}
	cout << endl;
	for (int i = 0; i <= NUM1 - 1; i++) {
		if (i % 2 == 1) {
			if (i == 7) {
				cout << setw(14) << "x轴" << setw(2) << 4;
			}
			else {
				cout << setw(16) << (i + 1) / 2;
			}
		}
		else {
			cout << setw(17);
		}
		for (int j = 0; j <= NUM2 - 1; j++) {
			cout << board[i][j];
		}
		cout << endl;
	}
}

//判断移动是否合法
bool Law(int x1, int y1, int x2, int y2, char board[NUM1][NUM2]) {
	if (x1 < 1 || x2 < 1 || y1 < 1 || y2 < 1 || x1 > 7 || x2 > 7 || y1 > 7 || y2 > 7) return false;
	else {
		if (abs(x2 - x1) <= 2 && abs(y2 - y1) <= 2) {
			if (board[2 * x1 - 1][4 * y1 - 2] == '@' && board[2 * x2 - 1][4 * y2 - 2] == ' ') return true;
			else return false;
		}
		else return false;
	}
}

//判断玩家User移动是否符合规则
bool Law2(int x1, int y1, int x2, int y2, char board[NUM1][NUM2]) {
	if (x1 < 1 || x2 < 1 || y1 < 1 || y2 < 1 || x1 > 7 || x2 > 7 || y1 > 7 || y2 > 7) return false;
	else {
		if (abs(x2 - x1) <= 2 && abs(y2 - y1) <= 2) {
			if (board[2 * x1 - 1][4 * y1 - 2] == '*' && board[2 * x2 - 1][4 * y2 - 2] == ' ') return true;
			else return false;
		}
		else return false;
	}
}

// 玩家落子
bool UserPlay(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '@')) {
		cout << "\t\t您User已经走上绝路了！" << endl;
		string key;
		cout << "\t\t请输入任意字符以弃权：";
		cin >> key;
		return true;
	}
	else {
		string chx1, chy1;
		cout << "\t\t请输入您User想移动的棋子的坐标：" << endl;
		cout << "\t\tx：";
		cin >> chx1;
		if (chx1[0] == '0') return false;
		cout << "\t\ty：";
		cin >> chy1;
		if (chy1[0] == '0') return false;
		string chx2, chy2;
		cout << "\t\t请输入您User想移动到的坐标：" << endl;
		cout << "\t\tx：";
		cin >> chx2;
		if (chx2[0] == '0') return false;
		cout << "\t\ty：";
		cin >> chy2;
		if (chy2[0] == '0') return false;
		int x1 = (int)chx1[0] - 48;
		int y1 = (int)chy1[0] - 48;
		int x2 = (int)chx2[0] - 48;
		int y2 = (int)chy2[0] - 48;
		if (Law(x1, y1, x2, y2, board)) {
			if (abs(x2 - x1) == 2 || abs(y2 - y1) == 2) {
				board[2 * x1 - 1][4 * y1 - 2] = ' ';
				board[2 * x2 - 1][4 * y2 - 2] = '@';
			}
			else board[2 * x2 - 1][4 * y2 - 2] = '@';
			Assimi(board, '@', x2, y2);
		}
		else {
			cout << "\t\t您的移动不合规则,请重新输入！" << endl;
			UserPlay(board);
		}
		return true;
	}
}

//玩家User2落子(执*）
bool User2Play(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '*')) {
		cout << "\t\t您User2已经走上绝路了！" << endl;
		string key;
		cout << "\t\t请输入任意字符以弃权：";
		cin >> key;
		return true;
	}
	else {
		string chx1, chy1;
		cout << "\t\t请输入您User2想移动的棋子的坐标：" << endl;
		cout << "\t\tx：";
		cin >> chx1;
		if (chx1[0] == '0') return false;
		cout << "\t\ty：";
		cin >> chy1;
		if (chy1[0] == '0') return false;
		string chx2, chy2;
		cout << "\t\t请输入您User想移动到的坐标：" << endl;
		cout << "\t\tx：";
		cin >> chx2;
		if (chx2[0] == '0') return false;
		cout << "\t\ty：";
		cin >> chy2;
		if (chy2[0] == '0') return false;
		int x1 = (int)chx1[0] - 48;
		int y1 = (int)chy1[0] - 48;
		int x2 = (int)chx2[0] - 48;
		int y2 = (int)chy2[0] - 48;
		if (Law2(x1, y1, x2, y2, board)) {
			if (abs(x2 - x1) == 2 || abs(y2 - y1) == 2) {
				board[2 * x1 - 1][4 * y1 - 2] = ' ';
				board[2 * x2 - 1][4 * y2 - 2] = '*';
			}
			else board[2 * x2 - 1][4 * y2 - 2] = '*';
			Assimi(board, '*', x2, y2);
		}
		else {
			cout << "\t\t您的移动不合规则,请重新输入！" << endl;
			User2Play(board);
		}
		return true;
	}
}

//同化对方的棋子
void Assimi(char board[NUM1][NUM2], char piece, int x2, int y2) {
	if (piece == '@') {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (board[2 * (x2 + i) - 1][4 * (y2 + j) - 2] == '*') {
					board[2 * (x2 + i) - 1][4 * (y2 + j) - 2] = '@';
				}
			}
		}
	}
	else if (piece == '*') {
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (board[2 * (x2 + i) - 1][4 * (y2 + j) - 2] == '@') {
					board[2 * (x2 + i) - 1][4 * (y2 + j) - 2] = '*';
				}
			}
		}
	}
}

//电脑落子
void ComPlay(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '*')) {
		cout << "\t\t电脑已经快瘫痪了！" << endl;
		string key;
		cout << "\t\t请按任意键继续：";
		cin >> key;
		cout << endl;
	}
	else {
		ChangeBoard(board);
		MoveValue(1, 0, -10000, 10000);
		int x1 = startX + 1;//加一是为了转换坐标（算法中使用0~6）
		int y1 = startY + 1;
		int x2 = resultX + 1;
		int y2 = resultY + 1;
		cout << "\t\t电脑将（" << x1 << ',' << y1 << "）处的棋子移动到了（" << x2 << ',' << y2 << "）" << endl;
		if (abs(x2 - x1) == 2 || abs(y2 - y1) == 2) {
			board[2 * x1 - 1][4 * y1 - 2] = ' ';
			board[2 * x2 - 1][4 * y2 - 2] = '*';
		}
		else board[2 * x2 - 1][4 * y2 - 2] = '*';
		Assimi(board, '*', x2, y2);
	}
}

// 人机对战（人先手）游戏过程中的函数
void Play(char board[NUM1][NUM2]) {
	int flag = 1;
	while (flag == 1) {
		Count(board);
		ShowBoard(board);
		cout << "\t\t1.继续游戏" << endl;
		cout << "\t\t2.存档并返回主菜单" << endl;
		cout << "\t\t3.直接退出" << endl;
		cout << "\t\t请选择您想进行的操作：";
		char choice;
		cin >> choice;
		cout << endl;
		switch (choice) {
		case '1':
			system("cls");
			while (true) {
				cout << "\t\tMODE : P V C (P FIRST)" << endl;
				Count(board);
				ShowBoard(board);
				cout << "\t\t在任意坐标处键入0以唤出操作选择菜单" << endl;
				if (!UserPlay(board)) {
					system("cls");
					break;
				}
				system("cls");
				ComPlay(board);
				//判断对局是否结束
				if (Result(board)) {
					Count(board);
					ShowBoard(board);
					while (true) {
						cout << "\t\t请选择是否保留该对局结果（1-是；2-否）：";
						string ans;
						cin >> ans;
						cout << endl;
						if (ans == "1") {
							Save(board, '1');
							break;
						}
						else if (ans == "2") {
							NewBoard(board);
							break;
						}
						else {
							cout << "\t\t您的输入我看不懂，请重新输入！" << endl;
						}
					}
					flag = 0;
					cout << "\t\t请按任意键返回主菜单：";
					string key;
					cin >> key;
					system("cls");
					break;
				}
			}
			break;
		case '2':
			system("cls");
			Save(board, '1');
			flag = 0;
			break;
		case '3':
			NewBoard(board);
			system("cls");
			flag = 0;
			break;
		default:
			system("cls");
			cout << "\t\t您的输入我看不懂，请重新输入！" << endl;
			break;
		}
	}
}

//人机对战（机先手）函数
void Play2(char board[NUM1][NUM2]) {
	int flag = 1;
	while (flag == 1) {
		Count(board);
		ShowBoard(board);
		cout << "\t\t1.继续游戏" << endl;
		cout << "\t\t2.存档并返回主菜单" << endl;
		cout << "\t\t3.直接退出" << endl;
		cout << "\t\t请选择您想进行的操作：";
		char choice;
		cin >> choice;
		cout << endl;
		switch (choice) {
		case '1':
			system("cls");
			while (true) {
				cout << "\t\tMODE : P V C (C FIRST)" << endl;
				Count(board);
				ShowBoard(board);
				cout << "\t\t在任意坐标处键入0以唤出操作选择菜单" << endl;
				if (!UserPlay(board)) {
					system("cls");
					break;
				}
				system("cls");
				ComPlay(board);				
				//判断对局是否结束
				if (Result(board)) {
					Count(board);
					ShowBoard(board);
					while (true) {
						cout << "\t\t请选择是否保留该对局结果（1-是；2-否）：";
						string ans;
						cin >> ans;
						cout << endl;
						if (ans == "1") {
							Save(board, '2');
							break;
						}
						else if (ans == "2") {
							NewBoard(board);
							break;
						}
						else {
							cout << "\t\t您的输入我看不懂，请重新输入！" << endl;
						}
					}
					flag = 0;
					cout << "\t\t请按任意键返回主菜单：";
					string key;
					cin >> key;
					system("cls");
					break;
				}
			}
			break;
		case '2':
			system("cls");
			Save(board, '2');
			flag = 0;
			break;
		case '3':
			NewBoard(board);
			system("cls");
			flag = 0;
			break;
		default:
			system("cls");
			cout << "\t\t您的输入我看不懂，请重新输入！" << endl;
			break;
		}
	}
}

//人人对战
void Playpvp(char board[NUM1][NUM2]) {
	int flag = 1;
	while (flag == 1) {
		Count(board);
		ShowBoard(board);
		cout << "\t\t1.继续游戏" << endl;
		cout << "\t\t2.存档并返回主菜单" << endl;
		cout << "\t\t3.直接退出" << endl;
		cout << "\t\t请选择您想进行的操作：";
		char choice;
		cin >> choice;
		cout << endl;
		switch (choice) {
		case '1':
			system("cls");
			while (true) {
				cout << "\t\tMODE : P V P" << endl;
				Countpvp(board);
				ShowBoard(board);
				cout << "\t\t在任意坐标处键入0以唤出操作选择菜单" << endl;
				if (!UserPlay(board)) {
					system("cls");
					break;
				}
				system("cls");
				cout << "\t\tMODE : P V P" << endl;
				Countpvp(board);
				ShowBoard(board);
				cout << "\t\t在任意坐标处键入0以唤出操作选择菜单" << endl;
				if (!User2Play(board)) {
					system("cls");
					break;
				}
				system("cls");
				//判断对局是否结束
				if (Resultpvp(board)) {
					Count(board);
					ShowBoard(board);
					while (true) {
						cout << "\t\t请选择是否保留该对局结果（1-是；2-否）：";
						string ans;
						cin >> ans;
						cout << endl;
						if (ans == "1") {
							Save(board, '3');
							break;
						}
						else if (ans == "2") {
							NewBoard(board);
							break;
						}
						else {
							cout << "\t\t您的输入我看不懂，请重新输入！" << endl;
						}
					}
					flag = 0;
					cout << "\t\t请按任意键返回主菜单：";
					string key;
					cin >> key;
					system("cls");
					break;
				}
			}
			break;
		case '2':
			system("cls");
			Save(board, '3');
			flag = 0;
			break;
		case '3':
			NewBoard(board);
			system("cls");
			flag = 0;
			break;
		default:
			system("cls");
			cout << "\t\t您的输入我看不懂，请重新输入！" << endl;
			break;
		}
	}
}

//棋子数及显示（PVC）
void Count(char board[NUM1][NUM2]) {
	int users = 0;
	int coms = 0;
	for (int i = 1; i <= 7; i++) {
		for (int j = 1; j <= 7; j++) {
			if (board[2 * i - 1][4 * j - 2] == '@') users++;
			else if (board[2 * i - 1][4 * j - 2] == '*') coms++;
		}
	}
	cout << "\t\t现在双方棋子数如下：" << endl;
	cout << "\t\tUser(@)：" << users << "  |  " << "Computer(*)：" << coms << endl;
}

//棋子数及显示（PVP）
void Countpvp(char board[NUM1][NUM2]) {
	int users = 0;
	int coms = 0;
	for (int i = 1; i <= 7; i++) {
		for (int j = 1; j <= 7; j++) {
			if (board[2 * i - 1][4 * j - 2] == '@') users++;
			else if (board[2 * i - 1][4 * j - 2] == '*') coms++;
		}
	}
	cout << "\t\t现在双方棋子数如下：" << endl;
	cout << "\t\tUser(@)：" << users << "  |  " << "User2(*)：" << coms << endl;
}

// 判断能否再走(true表示不能再走)
bool JudgeStep(char board[NUM1][NUM2], char piece) {
	for (int r = 1; r <= 7; r++) {
		for (int s = 1; s <= 7; s++) {
			if (board[2 * r - 1][4 * s - 2] == piece) {
				for (int i = -2; i <= 2; i++) {
					for (int j = -2; j <= 2; j++) {
						if (ComLaw( r + i, s + j, board) && board[2 * (r + i) - 1][4 * (s + j) - 2] == ' ') {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

//判断棋子位置是否在棋盘内
bool ComLaw(int x2, int y2, char board[NUM1][NUM2]) {
	if (x2 < 1 || y2 < 1 || x2 > 7 || y2 > 7) return false;
	return true;
}

//判断结果
bool Result(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '@') && JudgeStep(board, '*')) {
		cout << "\t\t游戏结束，让我们看看谁是胜利者！" << endl;
		int users = 0;
		int coms = 0;
		for (int i = 1; i <= 7; i++) {
			for (int j = 1; j <= 7; j++) {
				if (board[2 * i - 1][4 * j - 2] == '@') users++;
				else if (board[2 * i - 1][4 * j - 2] == '*') coms++;
			}
		}
		if (users > coms) {
			cout << "\t\t您获胜了！您已经超越人工智能的巅峰！" << endl;
		}
		else if (users < coms) {
			cout << "\t\t您竟然输了！您成为新一代人工智障！" << endl;
		}
		else {
			cout << "\t\t您和电脑平分秋色！下次再加把劲吧！" << endl;
		}
		return true;
	}
	return false;
}

//判断人人对战结果
bool Resultpvp(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '@') && JudgeStep(board, '*')) {
		cout << "\t\t游戏结束，让我们看看谁是胜利者！" << endl;
		int users = 0;
		int users2 = 0;
		for (int i = 1; i <= 7; i++) {
			for (int j = 1; j <= 7; j++) {
				if (board[2 * i - 1][4 * j - 2] == '@') users++;
				else if (board[2 * i - 1][4 * j - 2] == '*') users2++;
			}
		}
		if (users > users2) {
			cout << "\t\t恭喜user获胜！" << endl;
		}
		else if (users < users2) {
			cout << "\t\t恭喜user2获胜！" << endl;
		}
		return true;
	}
	return false;
}

//存档
void Save(char board[NUM1][NUM2], char mode) {
	outFile.open("Board.txt");
	outFile << mode;
	for (int i = 1; i <= 7; i++) {
		for (int j = 1; j <= 7; j++) {
			if (board[2 * i - 1][4 * j - 2] == ' ') {
				outFile << '0';
			}
			else {
				outFile << board[2 * i - 1][4 * j - 2];
			}
		}
	}
	outFile.close();
}

//读档
void Read(char board[NUM1][NUM2], char &mode) {
	inFile.open("Board.txt");
	inFile >> mode;
	for (int i = 1; i <= 7; i++) {
		for (int j = 1; j <= 7; j++) {
			char ch;
			inFile >> ch;
			if (ch == '0') {
				board[2 * i - 1][4 * j - 2] = ' ';
			}
			else {
				board[2 * i - 1][4 * j - 2] = ch;
			}
		}
	}
	inFile.close();
}

//判断是否在棋盘上
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

//评估当前局面价值
double value(int player, int depth) {
	double sum = 0;
	for (int i = 0; i <= 6; i++) {
		for (int j = 0; j <= 6; j++) {
			sum += makemove[depth].board[i][j] * weight[i][j];
		}
	}
	return player * sum;
}

//判断能否移动
bool CanMove(int player, int depth){
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

//搜索
void MoveTry(int s1, int r1, int s2, int r2, int player, int depth)//从(s1,r1)落到(s2,r2)处 
{
	makemove[depth].board[r2][s2] = player; 	//判断落点情况 	
	if (abs(r1 - r2) == 2 || abs(s1 - s2) == 2) {
		makemove[depth].board[r1][s1] = 0;
	}
	//同化 	
	for (int i = 0; i < 8; i++) {
		if (inMap(s2 + dx[i], r2 + dy[i])) {
			if (makemove[depth].board[r2 + dy[i]][s2 + dx[i]] == -player) {
				makemove[depth].board[r2 + dy[i]][s2 + dx[i]] = player;
			}
		}
	}
	return;
}

//将棋盘中的坐标转化为算法中的坐标
void ChangeBoard(char board[NUM1][NUM2]) {
	for (int i = 1; i <= 7; i++) {
		for (int j = 1; j <= 7; j++) {
			switch (board[2 * i - 1][4 * j - 2]) {
			case '*':
				gridInfo[i - 1][j - 1] = 1;
				break;
			case '@':
				gridInfo[i - 1][j - 1] = -1;
				break;
			case ' ':
				gridInfo[i - 1][j - 1] = 0;
				break;
			default:
				break;
			}

		}
	}
}

//AI核心算法minmax剪枝
double MoveValue(int player, int depth, double alpha, double beta) {//评估当前状态最佳落点 
	double max = -10000;
	if (depth == 4) {//到达最大搜索深度+1（5层会超时） 		
		return value(player, depth - 1);
	}
	if (depth == 0) {//初始化暂存棋盘 	
		for (int m = 0; m < 7; m++) {
			for (int n = 0; n < 7; n++) {
				makemove[0].board[m][n] = gridInfo[m][n];
				MoveValue(player, depth + 1, alpha, beta);
			}
		}
		return 0;
	} 	//存棋局 	
	for (int m = 0; m < 7; m++) {
		for (int n = 0; n < 7; n++) {
			makemove[depth].board[m][n] = makemove[depth - 1].board[m][n];
		}
	}
	if (!CanMove(player, depth)) {//如果不能下棋 	
		if (!CanMove(-player, depth))
			return value(player, depth);
		return -MoveValue(-player, depth + 1, -beta, -alpha);
	} 	//找位置落子 	
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (makemove[depth].board[i][j] == player) {
				for (int k = 0; k < 24; k++) {
					if (inMap(i + dy[k], j + dx[k])) {
						if (makemove[depth].board[i + dy[k]][j + dx[k]] == 0) {
							MoveTry(j, i, j + dx[k], i + dy[k], player, depth);
							double val = -MoveValue(-player, depth + 1, -beta, -alpha);
							//深度小于一时进行剪枝 							
							if (depth > 1) {
								if (val >= beta) {//剪枝
									return beta;
								}
								if (val > alpha)
								{
									alpha = val;
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
							//复原棋盘，回溯
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