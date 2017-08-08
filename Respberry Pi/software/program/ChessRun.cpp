#include<stdio.h>
#include<unistd.h>
#include<cv.h>
#include<highgui.h>
using namespace cv;
struct Chess{
	short state[13][13];
	Point left[13][13];
	Point right[13][13];
}chess;
struct ChangePoint{
	Point p;
	short state;
	struct ChangePoint* next;
};
void readFile();
void initChess(char* left, char* right);
short recognize(int m, int n, Mat left, Mat right);
int checkChange(char* left, char* right);
bool check(int x, int y, bool player);
int value(int x, int y, bool player);
void computer(int &x, int &y);
void writeToFile(char c,int x,int y);
int main(){
	readFile();
	int state;
	char left[]="../source/left.jpg";
	char right[]="../source/right.jpg";
	int x = 0, y = 0;
	do{
		system("rm -rf ../source/left.jpg ../source/right.jpg");
		system("../campic.sh");
		if(access(left,F_OK)==0&&access(left,R_OK)==0&&access(right,F_OK)==0&&access(right,R_OK)==0){
			state = checkChange(left, right);
			if(state!=0)
				break;
		}
		else
			return -1;		//表示没有得到图片
		sleep(3);
	} while (state == 0);
	if(state == -1){
		writeToFile('e', 0, 0);	//表示棋盘有非正常变化
		return -2;
	}else if (state == 1){
		writeToFile('t', 0, 0);	//表示玩家胜利
		return 1;
	}
	else{			
		computer(x, y);
		if (check(x, y, false)){
			writeToFile('f', x, y);
			return 2;		//表示电脑胜利
		}
		else{
			writeToFile('c', x, y);
			return 0;		//表示游戏继续
		}
	}
	
}
void readFile(){
	FILE* file = fopen("../source/chessFile.txt", "r");
	for (int i = 0; i < 13; i++){
		for (int j = 0; j < 13; j++){
			fscanf(file, "%d%d%d%d%d", &chess.state[i][j], &chess.left[i][j].x, &chess.left[i][j].y, &chess.right[i][j].x, &chess.right[i][j].y);
		}
	}
}
int checkChange(char* left, char* right){
	Mat imageLeft = imread(left, CV_8U);
	threshold(imageLeft, imageLeft, 130, 255, THRESH_BINARY);
	Mat imageRight = imread(right, CV_8U);
	threshold(imageRight, imageRight, 130, 255, THRESH_BINARY);
	for (int i = 0; i < 13; i++){
		for (int j = 0; j < 13; j++){
			short temp = recognize(i, j, imageLeft, imageRight);
			if (temp == 1 && chess.state[i][j] == 0){
				chess.state[i][j] = 1;
				if (check(i, j, true))
					return 1;		//玩家胜利
				else
					return 2;		//玩家没有胜利
			}else if(temp!=chess.state[i][j])
				return -1;		//检测到非正常的变化
		}
	}
	return 0;			//没有检测到改变
}
short recognize(int m, int n, Mat left, Mat right){
	int lx = chess.left[m][n].x;
	int ly = chess.left[m][n].y;
	int rx = chess.right[m][n].x;
	int ry = chess.right[m][n].y;
	int lw = 0, rw = 0, ls = 0, rs = 0;
	for (int i = lx - 10; i < lx + 10; i++){
		for (int j = ly - 10; j < ly + 10; j++){
			if (i >= 0 && i <= left.rows&&j >= 0 && j <= left.cols){
				if (left.at<uchar>(i, j)>127){
					lw++;
				}
				ls++;
			}
			
		}
	}
	for (int i = rx - 10; i < rx + 10; i++){
		for (int j = ry - 10; j < ry + 10; j++){
			if (i >= 0 && i <= left.rows&&j >= 0 && j <= left.cols){
				if (right.at<uchar>(i, j)>127){
					rw++;
				}
				rs++;
			}
		}
	}
	if (lw > ls * 7 / 8 || rw > rs * 7 / 8)
		return 1;	//白字
	else if (lw < ls / 4 || rw < rs / 4)
		return 2;	//黑子
	else
		return 0;	//无子
}
bool check(int x, int y, bool player){
	if (value(x, y, !player)>30000){
		return true;
	}
	return false;
}
int value(int x, int y, bool play){
	short player = play ? 1 : 2;
	int v = 0, k;
	int up, down, left, right, num = 0, x1, x2, y1, y2;
	up = x - 4 > 0 ? x - 4 : 0;
	down = x + 4 < 12 ? x + 4 : 12;
	left = y - 4 > 0 ? y - 4 : 0;
	right = y + 4 < 12 ? y + 4 : 12;
	//横向
	x1 = x2 = x;
	while (x1 >= up && chess.state[x1][y] == player)	x1--;
	while (x2 <= down && chess.state[x2][y] == player)	x2++;
	if (x2 - x1 < 6){
		k = 64 << (x2 - x1);
	}
	else{
		return 32765 + player;
	}
	if (x1 == -1 || chess.state[x1][y] == (player ^ 3))	k /= 2;
	if (x2 == 13 || chess.state[x2][y] == (player ^ 3))	k /= 2;
	v += k;
	//纵向
	y1 = y2 = y;
	while (y1 >= left && chess.state[x][y1] == player)	y1--;
	while (y2 <= right && chess.state[x][y2] == player)	y2++;
	if (y2 - y1 < 6){
		k = 64 << (y2 - y1);
	}
	else{
		return 32765 + player;
	}
	if (y1 == -1 || chess.state[x][y1] == (player ^ 3))	k /= 2;
	if (y2 == 13 || chess.state[x][y2] == (player ^ 3))	k /= 2;
	v += k;
	//右下
	x1 = x2 = x;
	y1 = y2 = y;
	while (x1 >= up && y1 >= left && chess.state[x1][y1] == player){ x1--;	y1--; }
	while (x2 <= down && y2 <= right && chess.state[x2][y2] == player){ x2++;	y2++; }
	if (x2 - x1 < 6){
		k = 64 << (x2 - x1);
	}
	else{
		return 32465 + player * 100;
	}
	if (x1 == -1 || y1 == -1 || chess.state[x1][y1] == (player ^ 3))	k /= 2;
	if (x2 == 13 || y2 == 13 || chess.state[x2][y2] == (player ^ 3))	k /= 2;
	v += k;
	//左下
	x1 = x2 = x;
	y1 = y2 = y;
	while (x1 >= up && y2 <= right && chess.state[x1][y2] == player){ x1--;	y2++; }
	while (x2 <= down && y1 >= left && chess.state[x2][y1] == player){ x2++;	y1--; }
	if (x2 - x1 < 6){
		k = 64 << (y2 - y1);
	}
	else{
		return 32465 + player * 100;
	}
	if (x1 == -1 || y2 == 13 || chess.state[x1][y2] == (player ^ 3))	k /= 2;
	if (x2 == 13 || y1 == -1 || chess.state[x2][y1] == (player ^ 3))	k /= 2;
	v += k;
	return v;
}
void computer(int &x, int &y){
	int value1[13][13], value2[13][13];
	for (int i = 0; i < 13; i++){
		for (int j = 0; j < 13; j++){
			if (chess.state[i][j] == 0){
				chess.state[i][j] = 1;
				value1[i][j] = value(i, j, true) + 72 - (i - 6)*(i - 6) - (j - 6)*(j - 6);
				chess.state[i][j] = 2;
				value2[i][j] = value(i, j, false) + 72 - (i - 6)*(i - 6) - (j - 6)*(j - 6);
				chess.state[i][j] = 0;
			}
			else{
				value1[i][j] = value2[i][j] = 0;
			}
		}
	}
	int max1x, max1y, max2x, max2y, max1 = 0, max2 = 0;
	for (int i = 0; i < 13; i++){
		for (int j = 0; j < 13; j++){
			if (max1 < value1[i][j]){
				max1x = i;
				max1y = j;
				max1 = value1[i][j];
			}
			if (max2 < value2[i][j]){
				max2x = i;
				max2y = j;
				max2 = value2[i][j];
			}
		}
	}
	if (max1 < max2){
		x = max2x;
		y = max2y;
	}
	else{
		x = max1x;
		y = max1y;
	}
}
void writeToFile(char c, int x, int y){
	chess.state[x][y] = 2;
	FILE* chessFile = fopen("../source/chessFile.txt", "w");
	for (int i = 0; i < 13; i++){
		for (int j = 0; j < 13; j++){
			fprintf(chessFile, " %d %d %d %d %d\n", chess.state[i][j], chess.left[i][j].x, chess.left[i][j].y, chess.right[i][j].x, chess.right[i][j].y);
		}
	}
	FILE* nextStep = fopen("next.txt", "w");
	fprintf(nextStep, "%c %d %d", c, x, y);
}
