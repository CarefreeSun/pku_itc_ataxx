#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
//���/���1����Ϊ@����������/���2Ϊ*

#define NUM2 29
#define NUM1 15

using namespace std;

ofstream outFile;
ifstream inFile;

char board[NUM1][NUM2];// ����
int gridInfo[7][7];//�㷨�е�����
int startX, startY, resultX, resultY;//AI�ƶ���ʽ
//Ȩֵ����
double weight[7][7] = {
	{ 1.00, 0.95, 0.85, 0.80, 0.80, 0.90, 1.00, },
	{ 0.95, 0.90, 0.83, 0.75, 0.72, 0.72, 0.79, },
	{ 0.85, 0.85, 0.80, 0.70, 0.67, 0.71, 0.78, },
	{ 0.80, 0.75, 0.72, 0.70, 0.70, 0.74, 0.80, },
	{ 0.75, 0.70, 0.67, 0.70, 0.78, 0.84, 0.85, },
	{ 0.80, 0.75, 0.75, 0.75, 0.84, 0.87, 0.97, },
	{ 1.00, 0.80, 0.78, 0.80, 0.85, 0.96, 1.00, }
};
//�ƶ�����
int dx[24] = { -1, 0, 1, 1, 1, 0, -1, -1, -2, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2 };
int dy[24] = { -1, -1, -1, 0, 1, 1, 1, 0, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2, 2, 2, 2, 1, 0, -1 };

//��¼ÿһ���ƶ������״̬ 
struct Move {
	int s1;
	int s2;
	int r1;
	int r2;
	int board[7][7] = { 0 };
};

Move makemove[5];//��¼�������ʱ��һ��·���ϵĽڵ�

void ShowMenu();//չʾ��ʼ�˵�
void NewBoard(char board[NUM1][NUM2]);//�������
void ShowBoard(char board[NUM1][NUM2]);//չʾ����
bool UserPlay(char board[NUM1][NUM2]);//������ӵĲ���
bool Law(int x1, int y1, int x2, int y2, char board[NUM1][NUM2]);//�ж����User�ƶ��Ƿ���Ϲ���
bool Law2(int x1, int y1, int x2, int y2, char board[NUM1][NUM2]);//�ж����User2�ƶ��Ƿ���Ϲ���
bool ComLaw(int x2, int y2, char board[NUM1][NUM2]);//�ж�ĳλ���Ƿ���������
void Assimi(char board[NUM1][NUM2], char piece, int x2, int y2);//ͬ���Է�������
void Play(char board[NUM1][NUM2]);//�˻���ս�������֣���Ϸ���̺���
void Play2(char board[NUM1][NUM2]);//�˻���ս�������֣�����
void Playpvp(char board[NUM1][NUM2]);//���˶�ս
void ComPlay(char board[NUM1][NUM2]);//��������
bool JudgeStep(char board[NUM1][NUM2], char piece);//�ж��ܷ�����
void Count(char board[NUM1][NUM2]);//��������������ʾ
void Countpvp(char board[NUM1][NUM2]);//ͳ��pvpģʽ����������ʾ
bool Result(char board[NUM1][NUM2]);//�жϽ��
bool Resultpvp(char board[NUM1][NUM2]);//�ж����˶�ս���
void Save(char board[NUM1][NUM2], char mode);//�浵
void Read(char board[NUM1][NUM2], char &mode);//����
//����ΪMinmax����-�¼�֦�븨������
inline bool inMap(int x, int y);//�ж��Ƿ��ڵ�ͼ��
double value(int player, int depth);//������ǰ�����ֵ
bool CanMove(int player, int depth);//�ܷ��ƶ�
void MoveTry(int s1, int r1, int s2, int r2, int player, int depth);//�����ƶ�һ��
void ChangeBoard(char board[NUM1][NUM2]);//���������ת��Ϊ�㷨�е����
double MoveValue(int player, int depth, double alpha, double beta);//minmax��֦

int main() {
	cout << endl;
	cout << "\t\t��ӭ����CarefreeSun��ͬ���壡\\(^o^)/" << endl;
	cout << "\t\tף�������죡" << endl;
	while (true) {
		ShowMenu();
		char select;
		cin >> select;
		system("cls");
		switch (select) {
		case '1':
			cout << "\t\t��ѡ��������еĶԾ�ģʽ:" << endl;
			cout << "\t\t1.�˻���ս�������֣�" << endl;
			cout << "\t\t2.�˻���ս�������֣�" << endl;
			cout << "\t\t3.���˶�ս" << endl;
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
		//��ȡ�浵
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
		//ֱ���˳�
		case '3':
			cout << endl;
			cout << "\t\t��ӭ���´����棡�ټ���";
			return 0;
			break;
		default:
			cout << "\t\t���������Ҷ����������ٴ����룡" << endl;
			break;
		}
	}
	system("pause");
	return 0;
}

// չʾ��ʼ�˵�
void ShowMenu() {
	cout << "\t\tѡ��������еĲ�����" << endl;
	cout << "\t\t1.�µĿ�ʼ" << endl;
	cout << "\t\t2.��ȡ�浵" << endl;
	cout << "\t\t3.�˳���Ϸ" << endl;
	cout << "\t\t����������ѡ��";
}

// ������
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

// ��ʾ����
void ShowBoard(char board[NUM1][NUM2]) {
	cout << setw(32) << "y��" << endl;
	cout << setw(19) << 1;
	for (int i = 2; i <= 7; i++) {
		cout << setw(4) << i;
	}
	cout << endl;
	for (int i = 0; i <= NUM1 - 1; i++) {
		if (i % 2 == 1) {
			if (i == 7) {
				cout << setw(14) << "x��" << setw(2) << 4;
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

//�ж��ƶ��Ƿ�Ϸ�
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

//�ж����User�ƶ��Ƿ���Ϲ���
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

// �������
bool UserPlay(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '@')) {
		cout << "\t\t��User�Ѿ����Ͼ�·�ˣ�" << endl;
		string key;
		cout << "\t\t�����������ַ�����Ȩ��";
		cin >> key;
		return true;
	}
	else {
		string chx1, chy1;
		cout << "\t\t��������User���ƶ������ӵ����꣺" << endl;
		cout << "\t\tx��";
		cin >> chx1;
		if (chx1[0] == '0') return false;
		cout << "\t\ty��";
		cin >> chy1;
		if (chy1[0] == '0') return false;
		string chx2, chy2;
		cout << "\t\t��������User���ƶ��������꣺" << endl;
		cout << "\t\tx��";
		cin >> chx2;
		if (chx2[0] == '0') return false;
		cout << "\t\ty��";
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
			cout << "\t\t�����ƶ����Ϲ���,���������룡" << endl;
			UserPlay(board);
		}
		return true;
	}
}

//���User2����(ִ*��
bool User2Play(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '*')) {
		cout << "\t\t��User2�Ѿ����Ͼ�·�ˣ�" << endl;
		string key;
		cout << "\t\t�����������ַ�����Ȩ��";
		cin >> key;
		return true;
	}
	else {
		string chx1, chy1;
		cout << "\t\t��������User2���ƶ������ӵ����꣺" << endl;
		cout << "\t\tx��";
		cin >> chx1;
		if (chx1[0] == '0') return false;
		cout << "\t\ty��";
		cin >> chy1;
		if (chy1[0] == '0') return false;
		string chx2, chy2;
		cout << "\t\t��������User���ƶ��������꣺" << endl;
		cout << "\t\tx��";
		cin >> chx2;
		if (chx2[0] == '0') return false;
		cout << "\t\ty��";
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
			cout << "\t\t�����ƶ����Ϲ���,���������룡" << endl;
			User2Play(board);
		}
		return true;
	}
}

//ͬ���Է�������
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

//��������
void ComPlay(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '*')) {
		cout << "\t\t�����Ѿ���̱���ˣ�" << endl;
		string key;
		cout << "\t\t�밴�����������";
		cin >> key;
		cout << endl;
	}
	else {
		ChangeBoard(board);
		MoveValue(1, 0, -10000, 10000);
		int x1 = startX + 1;//��һ��Ϊ��ת�����꣨�㷨��ʹ��0~6��
		int y1 = startY + 1;
		int x2 = resultX + 1;
		int y2 = resultY + 1;
		cout << "\t\t���Խ���" << x1 << ',' << y1 << "�����������ƶ����ˣ�" << x2 << ',' << y2 << "��" << endl;
		if (abs(x2 - x1) == 2 || abs(y2 - y1) == 2) {
			board[2 * x1 - 1][4 * y1 - 2] = ' ';
			board[2 * x2 - 1][4 * y2 - 2] = '*';
		}
		else board[2 * x2 - 1][4 * y2 - 2] = '*';
		Assimi(board, '*', x2, y2);
	}
}

// �˻���ս�������֣���Ϸ�����еĺ���
void Play(char board[NUM1][NUM2]) {
	int flag = 1;
	while (flag == 1) {
		Count(board);
		ShowBoard(board);
		cout << "\t\t1.������Ϸ" << endl;
		cout << "\t\t2.�浵���������˵�" << endl;
		cout << "\t\t3.ֱ���˳�" << endl;
		cout << "\t\t��ѡ��������еĲ�����";
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
				cout << "\t\t���������괦����0�Ի�������ѡ��˵�" << endl;
				if (!UserPlay(board)) {
					system("cls");
					break;
				}
				system("cls");
				ComPlay(board);
				//�ж϶Ծ��Ƿ����
				if (Result(board)) {
					Count(board);
					ShowBoard(board);
					while (true) {
						cout << "\t\t��ѡ���Ƿ����öԾֽ����1-�ǣ�2-�񣩣�";
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
							cout << "\t\t���������ҿ����������������룡" << endl;
						}
					}
					flag = 0;
					cout << "\t\t�밴������������˵���";
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
			cout << "\t\t���������ҿ����������������룡" << endl;
			break;
		}
	}
}

//�˻���ս�������֣�����
void Play2(char board[NUM1][NUM2]) {
	int flag = 1;
	while (flag == 1) {
		Count(board);
		ShowBoard(board);
		cout << "\t\t1.������Ϸ" << endl;
		cout << "\t\t2.�浵���������˵�" << endl;
		cout << "\t\t3.ֱ���˳�" << endl;
		cout << "\t\t��ѡ��������еĲ�����";
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
				cout << "\t\t���������괦����0�Ի�������ѡ��˵�" << endl;
				if (!UserPlay(board)) {
					system("cls");
					break;
				}
				system("cls");
				ComPlay(board);				
				//�ж϶Ծ��Ƿ����
				if (Result(board)) {
					Count(board);
					ShowBoard(board);
					while (true) {
						cout << "\t\t��ѡ���Ƿ����öԾֽ����1-�ǣ�2-�񣩣�";
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
							cout << "\t\t���������ҿ����������������룡" << endl;
						}
					}
					flag = 0;
					cout << "\t\t�밴������������˵���";
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
			cout << "\t\t���������ҿ����������������룡" << endl;
			break;
		}
	}
}

//���˶�ս
void Playpvp(char board[NUM1][NUM2]) {
	int flag = 1;
	while (flag == 1) {
		Count(board);
		ShowBoard(board);
		cout << "\t\t1.������Ϸ" << endl;
		cout << "\t\t2.�浵���������˵�" << endl;
		cout << "\t\t3.ֱ���˳�" << endl;
		cout << "\t\t��ѡ��������еĲ�����";
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
				cout << "\t\t���������괦����0�Ի�������ѡ��˵�" << endl;
				if (!UserPlay(board)) {
					system("cls");
					break;
				}
				system("cls");
				cout << "\t\tMODE : P V P" << endl;
				Countpvp(board);
				ShowBoard(board);
				cout << "\t\t���������괦����0�Ի�������ѡ��˵�" << endl;
				if (!User2Play(board)) {
					system("cls");
					break;
				}
				system("cls");
				//�ж϶Ծ��Ƿ����
				if (Resultpvp(board)) {
					Count(board);
					ShowBoard(board);
					while (true) {
						cout << "\t\t��ѡ���Ƿ����öԾֽ����1-�ǣ�2-�񣩣�";
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
							cout << "\t\t���������ҿ����������������룡" << endl;
						}
					}
					flag = 0;
					cout << "\t\t�밴������������˵���";
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
			cout << "\t\t���������ҿ����������������룡" << endl;
			break;
		}
	}
}

//����������ʾ��PVC��
void Count(char board[NUM1][NUM2]) {
	int users = 0;
	int coms = 0;
	for (int i = 1; i <= 7; i++) {
		for (int j = 1; j <= 7; j++) {
			if (board[2 * i - 1][4 * j - 2] == '@') users++;
			else if (board[2 * i - 1][4 * j - 2] == '*') coms++;
		}
	}
	cout << "\t\t����˫�����������£�" << endl;
	cout << "\t\tUser(@)��" << users << "  |  " << "Computer(*)��" << coms << endl;
}

//����������ʾ��PVP��
void Countpvp(char board[NUM1][NUM2]) {
	int users = 0;
	int coms = 0;
	for (int i = 1; i <= 7; i++) {
		for (int j = 1; j <= 7; j++) {
			if (board[2 * i - 1][4 * j - 2] == '@') users++;
			else if (board[2 * i - 1][4 * j - 2] == '*') coms++;
		}
	}
	cout << "\t\t����˫�����������£�" << endl;
	cout << "\t\tUser(@)��" << users << "  |  " << "User2(*)��" << coms << endl;
}

// �ж��ܷ�����(true��ʾ��������)
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

//�ж�����λ���Ƿ���������
bool ComLaw(int x2, int y2, char board[NUM1][NUM2]) {
	if (x2 < 1 || y2 < 1 || x2 > 7 || y2 > 7) return false;
	return true;
}

//�жϽ��
bool Result(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '@') && JudgeStep(board, '*')) {
		cout << "\t\t��Ϸ�����������ǿ���˭��ʤ���ߣ�" << endl;
		int users = 0;
		int coms = 0;
		for (int i = 1; i <= 7; i++) {
			for (int j = 1; j <= 7; j++) {
				if (board[2 * i - 1][4 * j - 2] == '@') users++;
				else if (board[2 * i - 1][4 * j - 2] == '*') coms++;
			}
		}
		if (users > coms) {
			cout << "\t\t����ʤ�ˣ����Ѿ���Խ�˹����ܵ��۷壡" << endl;
		}
		else if (users < coms) {
			cout << "\t\t����Ȼ���ˣ�����Ϊ��һ���˹����ϣ�" << endl;
		}
		else {
			cout << "\t\t���͵���ƽ����ɫ���´��ټӰѾ��ɣ�" << endl;
		}
		return true;
	}
	return false;
}

//�ж����˶�ս���
bool Resultpvp(char board[NUM1][NUM2]) {
	if (JudgeStep(board, '@') && JudgeStep(board, '*')) {
		cout << "\t\t��Ϸ�����������ǿ���˭��ʤ���ߣ�" << endl;
		int users = 0;
		int users2 = 0;
		for (int i = 1; i <= 7; i++) {
			for (int j = 1; j <= 7; j++) {
				if (board[2 * i - 1][4 * j - 2] == '@') users++;
				else if (board[2 * i - 1][4 * j - 2] == '*') users2++;
			}
		}
		if (users > users2) {
			cout << "\t\t��ϲuser��ʤ��" << endl;
		}
		else if (users < users2) {
			cout << "\t\t��ϲuser2��ʤ��" << endl;
		}
		return true;
	}
	return false;
}

//�浵
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

//����
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

//�ж��Ƿ���������
inline bool inMap(int x, int y)
{
	if (x < 0 || x > 6 || y < 0 || y > 6)
		return false;
	return true;
}

//������ǰ�����ֵ
double value(int player, int depth) {
	double sum = 0;
	for (int i = 0; i <= 6; i++) {
		for (int j = 0; j <= 6; j++) {
			sum += makemove[depth].board[i][j] * weight[i][j];
		}
	}
	return player * sum;
}

//�ж��ܷ��ƶ�
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

//����
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

//�������е�����ת��Ϊ�㷨�е�����
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

//AI�����㷨minmax��֦
double MoveValue(int player, int depth, double alpha, double beta) {//������ǰ״̬������ 
	double max = -10000;
	if (depth == 4) {//��������������+1��5��ᳬʱ�� 		
		return value(player, depth - 1);
	}
	if (depth == 0) {//��ʼ���ݴ����� 	
		for (int m = 0; m < 7; m++) {
			for (int n = 0; n < 7; n++) {
				makemove[0].board[m][n] = gridInfo[m][n];
				MoveValue(player, depth + 1, alpha, beta);
			}
		}
		return 0;
	} 	//����� 	
	for (int m = 0; m < 7; m++) {
		for (int n = 0; n < 7; n++) {
			makemove[depth].board[m][n] = makemove[depth - 1].board[m][n];
		}
	}
	if (!CanMove(player, depth)) {//����������� 	
		if (!CanMove(-player, depth))
			return value(player, depth);
		return -MoveValue(-player, depth + 1, -beta, -alpha);
	} 	//��λ������ 	
	for (int i = 0; i < 7; i++) {
		for (int j = 0; j < 7; j++) {
			if (makemove[depth].board[i][j] == player) {
				for (int k = 0; k < 24; k++) {
					if (inMap(i + dy[k], j + dx[k])) {
						if (makemove[depth].board[i + dy[k]][j + dx[k]] == 0) {
							MoveTry(j, i, j + dx[k], i + dy[k], player, depth);
							double val = -MoveValue(-player, depth + 1, -beta, -alpha);
							//���С��һʱ���м�֦ 							
							if (depth > 1) {
								if (val >= beta) {//��֦
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