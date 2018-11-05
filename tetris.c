#include "tetris.h"

static struct sigaction act, oact;
struct Node {
	int UserNumber;
	char Name[NAMELEN];
	int Score;
	struct Node* PreNode;
	struct Node* NextNode;
};

struct Node* LankingList;
RNode* root;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	
	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT:
			writeRankFile();
			exit=1;
			break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	nextBlock[3]=rand()%7;

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();

	recommend(field, 0);
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawShadow(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	
	move(9,WIDTH+10);
	DrawBox(10,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(17,WIDTH+10);
	printw("SCORE");
	DrawBox(18,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	case ' ':
		while(1) {
				if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX))	blockY++;
				else break;
		}
		BlockDown(0);
			/*AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score+=DeleteLine(field);
			PrintScore(score);
			nextBlock[0]=nextBlock[1];
			nextBlock[1]=nextBlock[2];
			nextBlock[2]=rand()%7;
			blockY=-1;
			blockX=WIDTH/2-2;
			DrawField();
			DrawNextBlock(nextBlock);*/
			timed_out=0;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(19,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}

	for( i = 0; i < 4; i++ ){
		move(11+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			clear();
			newRank(score);
			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	clear();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;

	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			if(block[currentBlock][blockRotate][i][j]==1) {
				if(j+blockX>=WIDTH) return 0;
				if(i+blockY<0) return 0;
				if(i+blockY>=HEIGHT) return 0;
				if(j+blockX<0) return 0;
				if(f[blockY+i][blockX+j]==1) return 0;
			}
		}
	}

	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int temp=blockRotate; // 변형 이전 모양 임시 변수.
	int tempX=blockX, tempY=blockY; // 변형 이전 좌표 임시 변수.
	int i,j;
	int ShadowY=tempY;
	char tile;


	if(command==KEY_UP) {
		if(blockRotate == 0) temp = 3;
		else temp = blockRotate-1;
	}
	if(command==KEY_LEFT)	tempX=blockX+1;
	if(command==KEY_RIGHT)	tempX=blockX-1;
	if(command==KEY_DOWN)	tempY=blockY-1;
	/*if(command==' ') {
		while(1) {
				if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX))	blockY++;
				else break;
		}
	}*/

	for(i=0;i<4;i++) {		//	이전 블럭 위치의 그림 지우기
		for(j=0;j<4;j++){
			if(block[currentBlock][temp][i][j]==1 && i+tempY>=0){
				move(i+tempY+1,j+tempX+1);
				tile='.';
				printw("%c",tile);
			}
		}
	}

	while(1) {
		if(CheckToMove(field, currentBlock, temp, ShadowY+1, tempX)==1)	ShadowY++;
		else break;
	}


	for(i=0;i<4;i++) {		//	이전 블륵 위치의 그림자 지우기
		for(j=0;j<4;j++) {
			if(block[currentBlock][temp][i][j]==1 && i+ShadowY>=0) {
				move(i+ShadowY+1,j+tempX+1);
				tile='.';
				printw("%c",tile);
			}
		}
	}
	
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {
	if(recommendFlag==1)	DrawRecommend(recommendY,recommendX, nextBlock[0], recommendR);
	DrawShadow(y, x, blockID, blockRotate);
	DrawBlock(y, x, blockID, blockRotate, ' ');
}

void BlockDown(int sig){
	if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)==1){
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	else {
		if(blockY==-1) gameOver = true;
		else {
			AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
			score+=DeleteLine(field);
			PrintScore(score);
			nextBlock[0]=nextBlock[1];
			nextBlock[1]=nextBlock[2];
			nextBlock[2]=nextBlock[3];
			nextBlock[3]=rand()%7;
			blockY=-1;
			blockX=WIDTH/2-2;
			DrawField();
			if(recommendFlag==1) {
				recommend(field, 0);
				GMax=0;
				RecommendTreeFree(root);
			}
			DrawNextBlock(nextBlock);	
		}
	}
	timed_out=0;
}

void AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j;
	int touched=0;	

	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			if(block[currentBlock][blockRotate][i][j]==1) {
				if(blockY+i+1 == HEIGHT) touched++;
				else {
					if(field[blockY+i+1][blockX+j]==1)	touched++;
				}
				field[blockY+i][blockX+j]=1;
			}
		}
	}
	score += touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i, j, q, w;
	int checktemp;
	int DeletedLineNum=0;

	for(i=1; i<=HEIGHT; i++) {
		checktemp=0;
		for(j=0; j<WIDTH; j++) {
			if(f[i][j]==1)
				checktemp++;
		}
		if(checktemp==WIDTH) {
			DeletedLineNum++;
			for(w=i; w>0 ; w--) {
				for(q=0; q<WIDTH; q++)
					f[w][q]=f[w-1][q]; 
			}
		}
	}

	return DeletedLineNum * DeletedLineNum * 100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	int tempY=y, i, j;
	char tile='/';

	while(1) {
		if(CheckToMove(field, blockID, blockRotate, tempY+1, x)==1)	tempY++;
		else break;
	}

	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+tempY>=0){
				move(i+tempY+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void createRankList(){
	FILE* FP;
	int i;
	char* tempName=(char*)malloc(NAMELEN*sizeof(char));
	struct Node* NextNode;
	struct Node* temp;
	struct Node* PreNode;

	LankingList=(struct Node*)malloc(sizeof(struct Node));
	temp = LankingList;

	FP= fopen("rank.txt", "r");
	if(FP==NULL) {
		printf("랭크 정보를 등록한 파일이 없습니다 !");
		FP= fopen("rank.txt","w");
		fprintf(FP,"%d",0);
		fclose(FP);
	}

	FP=fopen("rank.txt", "r");
	fscanf(FP, "%d", &(*temp).UserNumber);
	PreNode=NULL;
	fgetc(FP);
	for(i=0; i<(*LankingList).UserNumber; i++) {
		NextNode=NULL;
		fscanf(FP, "%s", tempName);
		strcpy((*temp).Name, tempName);
		fscanf(FP, "%d", &(*temp).Score);
		fgetc(FP);

		NextNode=(struct Node*)malloc(sizeof(struct Node));
		if(i+1==(*LankingList).UserNumber) {
			(*temp).NextNode=NULL;
			(*temp).PreNode=PreNode;
		}
		else {
			(*temp).PreNode=PreNode;
			(*temp).NextNode=NextNode;
		}
		PreNode=temp;
		temp=NextNode;
	}
}

void rank() {
	int count, X=-10, Y=-10, command, i;
	int printFlag=0;
	char* InputName=(char*)malloc((NAMELEN)*sizeof(char));
	struct Node* temp=LankingList;

	clear();
	move(0,0);
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	command=wgetch(stdscr);
	command-=48;
	switch(command) {
		case 1:
			echo();
			printw("X: ");
			scanw("%d", &X);
			printw("Y: ");
			scanw("%d", &Y);
			noecho();
			if(X==-10) X=1;
			if(Y==-10) Y=LankingList->UserNumber;

			//if(X<=0 || (Y>(*LankingList).UserNumber&&Y!=1000) || (X>Y&&X!=1000) ) 	{
			if(X<=0 || Y>LankingList->UserNumber || X>Y)
				printw("\nSearch Failure: no rank in the list\n");

			else {
				printw("\n");
				printw("       name       |   score   \n");
				printw("------------------------------\n");
				
				for(count=1; count<=(*LankingList).UserNumber; count++) {
					if( count>=X && count <=Y )
						printw(" %-17s| %d\n", (*temp).Name, (*temp).Score);
					temp=(*temp).NextNode;
				}
			}
			break;

		case 2:
			printw("input the name: ");
			echo();
			scanw("%s", InputName);
			noecho();
			printw("       name       |   score   \n");
			printw("------------------------------\n");

			for(count=1; count<=LankingList->UserNumber; count++) {
				if(strcmp(temp->Name, InputName)==0) {
					printw(" %-17s| %d\n", temp->Name, temp->Score);
					printFlag=1;
				}
				temp=temp->NextNode;
			}
			if(printFlag==0) 
				printw("\nsearch failure: no name in the list\n");
			break;

		case 3:
			printw("input the rank: ");
			echo();
			scanw("%d", &X);
			noecho();
			if(X<=0 || X>LankingList->UserNumber) 
				printw("\nsearch failure: the rank not in the list\n");
			else {
				// 삭제
				for(count=1; count<=LankingList->UserNumber; count++) {
					if(count==X && count !=1 && count != LankingList->UserNumber) { // 중간 노드 삭제
						(temp->PreNode)->NextNode=temp->NextNode;
						LankingList->UserNumber--;
						free(temp);
						break;
					}
					else if(count==X && count==1) { // 맨 앞 노드 삭제
						temp->NextNode->UserNumber=LankingList->UserNumber;
						LankingList=temp->NextNode;
						LankingList->UserNumber--;
						free(temp);
						break;
					}
					else if(count==X && count == LankingList->UserNumber) { // 맨 뒷 노드 삭제
						temp->PreNode->NextNode=NULL;
						LankingList->UserNumber--;
						free(temp);
						break;
					}
					temp=temp->NextNode;
				}
			}
			
	}
	printw("Please Input Space to Quit");
	wgetch(stdscr);
}

void newRank(int score){
	char* InputName = (char*)malloc(NAMELEN*sizeof(char));
	struct Node* PreNode;
	struct Node* InputNode;
	struct Node* temp = LankingList;
	
	int i, tempUserNumber=(*LankingList).UserNumber;
	int breakFlag=0;
	
	clear();

	move(0,0);
	printw("Input your name (<= 16 Characters.) : ");
	echo();
	scanw("%s", InputName);
	noecho();
	InputNode=(struct Node*)malloc(sizeof(struct Node));
	(*InputNode).Score=score;
	strcpy((*InputNode).Name, InputName);
	for(i=0; i<(*LankingList).UserNumber; i++) { 
		if( (*temp).Score < score ) { // temp노드와 input노드 비교. input노드가 더 클 시에
			if(i==0) { // 가장 앞에 삽입 될 때
				(*InputNode).NextNode=temp;
				(*InputNode).PreNode=NULL;
				LankingList=InputNode;
			}
			else { // 중간에 삽입 될 때
				(*InputNode).NextNode=temp;
				(*InputNode).PreNode=(*temp).PreNode;
				((*temp).PreNode)->NextNode=InputNode;
			}
			breakFlag=1;
			break;
		}
		if(i!=((*LankingList).UserNumber-1)) temp=(*temp).NextNode;
	}
	//제일 뒤에 삽입 될 때
	if(breakFlag==0) {
		(*InputNode).PreNode=temp;
		(*InputNode).NextNode=NULL;
		(*temp).NextNode=InputNode;
	}

	(*LankingList).UserNumber=tempUserNumber+1;
}

void writeRankFile(){
	FILE *FP;
	struct Node* temp=LankingList;

	FP=fopen("rank.txt","w");
	fprintf(FP, "%d\n", (*LankingList).UserNumber);

	do {
		fprintf(FP, "%s %d\n", (*temp).Name, (*temp).Score);
		temp=(*temp).NextNode;
	} while(temp!=NULL);
}

int recommend(char fieldOri[HEIGHT][WIDTH], int lv) {
	RNode *curr, *temp;
	int Vert, Hori, BR, i, j, q, w, y, x, CN;
	int touched=0;
	int leftbound=100, rightbound=100;
	int MaxLv=BLOCK_NUM;	// 최대 깊이를 대입.
	int MAX=0;
	//int ep1,ep2,ep3;

	
	// lv == 0 일시 헤드노드 할당, 정보생성.
	if(lv==0) {
		root=(RNode*)malloc(sizeof(RNode));
		curr=root;
		curr->level=lv;
		curr->parent=NULL;
		curr->accumulatedScore=0;
		for(j=0; j<HEIGHT; j++) 		// 오리지날 필드 복사
				for(w=0; w<WIDTH; w++) 
					curr->recField[j][w]=fieldOri[j][w];
	}
	else 	curr=Gcurr;
	
	curr->child=(RNode**)malloc(36*sizeof(RNode*));
	// for문, 현재 노드에 관해서 자식 생성.
	numchild=-1;					// 초기화.
	for(BR=0; BR<4; BR++) { 			// 블럭 로테이트.
		for(q=0; q<4; q++) {				// bound 찾기용.
			if(block[nextBlock[lv]][BR][q][0]==1)
				leftbound=0;
			if(block[nextBlock[lv]][BR][q][3]==1) 
				rightbound=WIDTH-3;
		}
		if(leftbound!=0) leftbound=-1;
		if(rightbound!=WIDTH-3) rightbound=WIDTH-2;

		for(q=leftbound; q<=rightbound; q++) {		// 가로 입력 시작.
			curr->child[++numchild]=(RNode *)malloc(sizeof(RNode));
			curr->child[numchild]->level=lv+1;	// level 등록
			curr->child[numchild]->parent=curr;
			curr->child[numchild]->numchild=0;
			curr->child[numchild]->child=NULL;	// 자식 포인터 초기화
			for(j=0; j<HEIGHT; j++) 		// 부모의 필드 복사
				for(w=0; w<WIDTH; w++) 
					curr->child[numchild]->recField[j][w]=curr->recField[j][w];

			// 점수 등록을 위한 사전준비 단계.
			x=q, y=0, touched=0;				// 초기화
			// 입력받은 필드에서 이동하는 가능 위치 조사.
			while(1) {
				if(CheckToMove(curr->child[numchild]->recField, nextBlock[lv], BR, y+1, x)) y++;
				else break;
			}
			if(curr->child[numchild]->level==1) {
				curr->child[numchild]->X=x;
				curr->child[numchild]->Y=y;
				curr->child[numchild]->R=BR;	// x,y 좌표, 블럭 로테이션 등록.
			}
			// 닿는 면적 점수 계산.
			for(i=0; i<4; i++) {
				for(j=0; j<4; j++) {
					if(block[nextBlock[lv]][BR][i][j]==1) {	
						if(y+i+1 == HEIGHT) touched++;
						else {
							if(curr->child[numchild]->recField[y+i+1][x+j]==1)	touched++;
						}
					}
				}
			}

			// 해당 자식 노드의 최종 점수 대입. (지워진 점수 + 부모 노드의 점수 + 닿은 면적의 점수.)
			//curr->child[numchild]->accumulatedScore = DeleteLine(curr->child[numchild]->recField);
			curr->child[numchild]->accumulatedScore = DeleteLine(curr->child[numchild]->recField) + curr->accumulatedScore + touched*10;
				
		}
	}
	curr->numchild=numchild;
	
	// [ 아직 할 필요 없음 ]프루닝

	

	//ep1=nextBlock[0];
	//ep2=nextBlock[1];
	//ep3=nextBlock[2];
	// 자식 노드를 재귀로 다시 호출. (최대 깊이에 도달하지 않았을 때)
	if(lv!=MaxLv-1) {
		for(i=0; i<curr->numchild; i++) {
			Gcurr=curr->child[i];
			recommend(curr->child[i]->recField, lv+1);
		}
	}
	// lv == MaxLv-1 일 시에 추천 루트 산출.
	else {
		for(i=0; i<curr->numchild; i++) {
			if(GMax<curr->child[i]->accumulatedScore) {
				GMax=curr->child[i]->accumulatedScore;
				temp=curr;
				while(temp->level!=1) {
					temp=temp->parent;
				}
				recommendX=temp->X;
				recommendY=temp->Y;
				recommendR=temp->R;
			}
		}
	}


	//nextBlock[0]=ep1;
	//nextBlock[1]=ep2;
	//nextBlock[2]=ep3;

	return 0;
}

void RecommendTreeFree(RNode* curr) {
	int i;
	
	if(curr == NULL)	return;

	for(i=0; i<curr->numchild; i++)
			RecommendTreeFree(curr->child[i]);
	free(curr->child);
	free(curr);
}

void Prunning();

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	int tempY=y, i, j;
	char tile='R';

	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+tempY>=0){
				move(i+tempY+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void recommendedPlay(){
	// user code
}
