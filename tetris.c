#include "tetris.h"

static struct sigaction act, oact;

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
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i, j;
    for(j = 0; j < HEIGHT; j++)
        for(i = 0; i < WIDTH; i++)
            field[j][i] = 0;

	 for ( i = 1 ; i < 8 ; i++ ) 
					 init_pair(i,i,0);
    nextBlock[0] = rand() % 7;
    nextBlock[1] = rand() % 7;
    nextBlock[2] = rand() % 7;
    blockRotate = 0;
    blockY = -1; // Start just outside the visible field
    blockX = WIDTH / 2 - 2;
    score = 0;
    gameOver = 0;
    timed_out = 0;

    DrawOutline();
    DrawField();
    DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
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

	DrawBox(9,WIDTH+10,4,8);

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
	printw("%6d",score);
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

	/* 두번째 블록 */
	for( i = 0; i < 4; i++ ){
		move(10+i,WIDTH+13);
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
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

/* 현재 회전 상태와 위치를 바탕으로 이동 가능 여부를 판단한다. */
int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j;
	for(i = 0;i < 4;i++) {
		for (j = 0; j < 4;j++) {
			if(block[currentBlock][blockRotate][i][j]==1) {
				if(f[i+blockY][j+blockX] == 1)
					return 0;
				if(i+blockY >= HEIGHT || j+blockX < 0 || j+blockX >= WIDTH)
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 

	int i,j;
	int oldShadowY;
	int blk=currentBlock, rot=blockRotate,y=blockY,x=blockX;

	switch(command){
		case KEY_UP: rot=(rot+3)%4; break;
		case KEY_DOWN: y--; break;
		case KEY_RIGHT: x--; break;
		case KEY_LEFT: x++; break;
	}

	oldShadowY = y;
	while(CheckToMove(f,blk,rot,++oldShadowY,x));
	--oldShadowY;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++){
			if(block[blk][rot][i][j]==1){
				if(i+y >= 0){ 
					move(i+y+1,j+x+1);
					printw(".");
				}
				if(i+oldShadowY >= 0){ 
					move(i+oldShadowY+1, j+x+1);
					printw(".");
				}
			}
		}
	DrawBlockWithFeatures(blockY,blockX,currentBlock, blockRotate);
	move(HEIGHT, WIDTH+10);
}

void BlockDown(int sig){
	// user code
	//강의자료 p26-27의 플로우차트를 참고한다.

	  int drawFlag = 0;
    if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)) {
        blockY++;
        DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
    } else {
        if (blockY == -1) gameOver = 1; // Set game over if the block cannot move down from the initial position
        else {
            int touchPointsScore = AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
            int linesCleared = DeleteLine(field);
            score += linesCleared + touchPointsScore;
            nextBlock[0] = nextBlock[1];
            nextBlock[1] = nextBlock[2];
            nextBlock[2] = rand() % 7;
            blockRotate = 0;
            blockY = -1;
            blockX = WIDTH / 2 - 2;
            DrawNextBlock(nextBlock);
            PrintScore(score);
            DrawField();
        }
    }
    timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX) {
    int i, j, touched = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (block[currentBlock][blockRotate][i][j] == 1) {
                f[blockY + i][blockX + j] = 1;
                if (blockY + i + 1 >= HEIGHT || f[blockY + i + 1][blockX + j] == 1) {
                    touched++; // Increase touch points when block is at the bottom or above another block
                }
            }
        }
    }
    return touched * 10; 
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code

	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.

	int fullLinesDeleted = 0;

	// 필드의 모든 라인을 확인
    for (int currentRow = HEIGHT - 1; currentRow > 0; currentRow--) {
        int isFullLine = 1;  // 이 라인이 꽉 찼는지 확인하는 플래그

        // 현재 라인의 모든 칸을 확인
        for (int col = 0; col < WIDTH; col++) {
            if (f[currentRow][col] != 1) {
                isFullLine = 0;  // 하나라도 비어있으면 꽉 찬 라인이 아님
                break;
            }
        }

        // 꽉 찬 라인이면 위의 라인들을 내림
        if (isFullLine) {
            for (int rowToMove = currentRow; rowToMove > 0; rowToMove--) {
                for (int col = 0; col < WIDTH; col++) {
                    f[rowToMove][col] = f[rowToMove - 1][col];
                }
            }

            // 가장 위의 라인을 비움
            for (int col = 0; col < WIDTH; col++) {
                f[0][col] = 0;  // 빈 칸으로 설정
            }

            fullLinesDeleted++;  // 삭제된 라인 수 증가
            currentRow++;  // 삭제 후 같은 라인을 다시 확인
        }
    }

    return fullLinesDeleted * fullLinesDeleted * 100;
}



///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate){
	while(CheckToMove(field,nextBlock[0],blockRotate,y+1,x))
		y++;
	DrawBlock(y,x,blockID,blockRotate,'/');
}

void createRankList()
{
		  // user code

		FILE *fp = fopen("rank.txt", "r"); // 파일을 읽기 모드로 열기
   		 if (fp == NULL) {
        perror("Failed to open file"); // 파일 열기 실패 처리
        return;
    }
		  Node **NewRank;
		  int i;
		  char name[NAMELEN];
		  int score;
		  int num;

		fscanf(fp,"%d",&num);

		Ranknumber=num;

		Node **newRank = (Node**)malloc(sizeof(Node*) * Ranknumber);
    	if (newRank == NULL) {
        perror("Memory allocation failed"); // 메모리 할당 실패 처리
        fclose(fp);
        return;
    }

		for (int i = 0; i < Ranknumber; i++) {
        char name[NAMELEN];
        int score;

        fscanf(fp, "%s %d", name, &score); // 이름과 점수 읽기

        newRank[i] = (Node*)malloc(sizeof(Node));
        if (newRank[i] == NULL) {
            perror("Memory allocation failed for Node"); // 노드 메모리 할당 실패 처리
            continue;
        }
        strcpy(newRank[i]->name, name);
        newRank[i]->score = score;
        newRank[i]->link = NULL;

        // 노드 연결
        if (Head == NULL) {
            Head = newRank[i];
        } else {
            Tail->link = newRank[i];
        }
        Tail = newRank[i]; // 새로운 노드가 마지막 노드가 됨
    }

    fclose(fp); // 파일 닫기
}

void rank(){
		  // user code


		  int i,input,x=0,y=0,nameflag,rank;
		  int tmp;
		  char name[NAMELEN];
		  Node *prev=NULL,*temp=Head;
		  clear();
		  printw("1. list ranks from X to Y\n2. list ranks by a specific name\n3. delete a specific rank\n");
		  input=wgetch(stdscr);
		  echo();

		  if(input=='1')
		  {
					 printw("X: ");
					 scanw("%d",&x);
					 printw("Y: ");
					 scanw("%d",&y);

					 printw("\tname\t|\tscore\n--------------------------------\n");
					 if(x==0 && y!=0){
								for(i=0;i<y;i++){
										  printw(" %s\t\t| %d\n",temp->name,temp->score);
										  temp=temp->link;
								}
					 }
					 else if(x!=0 && y==0){
								for(i=0;i<x-1;i++) temp=temp->link;
								while(temp){
										  printw(" %s\t\t| %d\n",temp->name,temp->score);
										  temp=temp->link;
								}
					 }
					 else if(x==0 && y==0){
								while(temp){
										  printw(" %s\t\t| %d\n",temp->name,temp->score);
										  temp=temp->link;
								}
					 }
					 else if(x>y) printw("search failure: no rank in the list\n");
					 else if(x<0 || y>Ranknumber) printw("search failure: no rank in the list\n");

					 else{
								for(i=0;i<x-1;i++) temp=temp->link;
								for(i=x;i<=y;i++){
										  printw("%s\t\t| %d\n",temp->name,temp->score);
										  temp=temp->link;
								}
					 }

		  }


		  else if(input =='2')
		  {
					 nameflag=0;
					 printw("Input the name : ");
					 scanw("%s",name);
					 while(temp){
								if(!strcmp(temp->name,name)){
										  printw(" %s\t\t| %d\n",temp->name,temp->score);
										  nameflag=1;
								}
								temp=temp->link;
					 }
					 if(nameflag==0) printw("\nsearch failure: no rank in the list\n");

		  }
		  else if(input =='3')
		  {
					 printw("Input the rank : ");
					 scanw("%d",&rank);
					 if(rank>Ranknumber) printw("\nsearch failure: the rank not in the list\n");
					 else{
								for(i=0;i<rank-1;i++){
										  prev=temp;
										  temp=temp->link;
								}
								if(prev==NULL) Head=Head->link;
								else prev->link=temp->link;
								free(temp);
								Ranknumber--;
								printw("\nresult: the rank deleted\n");
					 }
					 writeRankFile();

		  }

		  getch();
		  noecho();
}

void writeRankFile(){
	// user code
	Node *current = Head; // Head에서 시작
    FILE *fp = fopen("rank.txt", "w"); // 파일을 쓰기 모드로 열기

    if (fp == NULL) {
        perror("Failed to open file"); // 파일 열기 실패 처리
        return;
    }

    // 랭킹 리스트의 노드 수 기록
    fprintf(fp, "%d\n", Ranknumber);

    // 모든 랭킹 항목을 파일에 쓰기
    while (current != NULL) {
        fprintf(fp, "%s %d\n", current->name, current->score);
        current = current->link; // 다음 노드로 이동
    }

    fclose(fp); // 파일 닫기
}

void newRank(int score)
{
	 // user code
	Node *newRank, *temp = Head;
    char name[NAMELEN];

    // 사용자 인터페이스 초기화
    clear();
    echo();
    printw("Your name: ");
    scanw("%s", name);

    // 새 랭크 노드 할당 및 초기화
    newRank = (Node *)malloc(sizeof(Node));
    strcpy(newRank->name, name);
    newRank->score = score;

    // 랭킹 리스트에 새 노드 삽입
    if (Head == NULL) {
        newRank->link = NULL;
        Head = newRank;
        Tail = newRank;  // 첫 노드가 Tail이기도 함
    } else {
        while (temp->link != NULL && temp->link->score >= score) {
            temp = temp->link;
        }
        newRank->link = temp->link;
        temp->link = newRank;
        if (newRank->link == NULL) {
            Tail = newRank;  // 새 노드가 마지막 노드면 Tail 갱신
        }
    }

    Ranknumber++;

    // 파일에 랭킹 정보 기록
    writeRankFile();

    // 사용자 인터페이스 정리
    noecho();
    getch();
}


void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}

/* */
void DrawBlockWithFeatures (int y, int x, int blockID, int blockRotate){
	DrawShadow(y,x,blockID,blockRotate);
	DrawBlock(y,x,blockID,blockRotate,' ');
}