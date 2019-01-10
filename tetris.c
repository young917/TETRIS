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
		case MENU_PLAY: autoplay = 0; play(); break;
		case MENU_RANK: rank();break;
		case MENU_REC: {
						   autoplay =1;
						   gamestart=clock();
						   recommendedPlay();
						   break;
					   }
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}
	free(recRoot);
	recRoot=NULL;
	endwin();
	writeRankFile();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;
	for(i=0;i<BLOCK_NUM;i++)
		nextBlock[i]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	for(j=0;j<HEIGHT;j++)	
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;
	if(!autoplay){
		recRoot=(RecNode*)malloc(sizeof(RecNode));
		for(j=0;j<HEIGHT;j++)
			for(i=0;i<WIDTH;i++){
				recRoot->f[j][i]=0;
			}
		recRoot->lv=0;
		recRoot->score=0;
		for(i=0;i<CHILDREN_MAX;i++)
			recRoot->c[i]=NULL;
		recommend(recRoot);
	}
	else
		Init_recommend();
	DrawOutline();
	DrawField();
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawBlock(recommendY,recommendX,nextBlock[0],recommendR,'R');
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

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9,WIDTH+10);
	printw("SCORE");
	DrawBox(10,WIDTH+10,1,8);
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
	move(11,WIDTH+11);
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
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				if(tile == '.')
					printw("%c",tile);
				else{
					attron(A_REVERSE);
					printw("%c",tile);
					attroff(A_REVERSE);
				}
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
			free(recRoot);
			recRoot=NULL;
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

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j;
	int x,y;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				x=blockX+j;
				y=blockY+i;
				if(y>=HEIGHT)
					return 0;
				else if(x<0 || x>=WIDTH)
					return 0;
				else if(f[y][x]==1)
					return 0;
			}
		}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int pastRotate,pastY,pastX;
	int pshdy,pshdx;
	int i,j;
	pastRotate=blockRotate;
	pastY=blockY;
	pastX=blockX;
	switch(command){
		case KEY_UP:
			pastRotate=(blockRotate+3)%4;
			break;
		case KEY_DOWN:
			pastY=blockY-1;
			break;
		case KEY_LEFT:
			pastX=blockX+1;
			break;
		case KEY_RIGHT:
			pastX=blockX-1;
			break;
	}
	DrawBlock(pastY,pastX,currentBlock,pastRotate,'.');
	DrawBlock(recommendY,recommendX,currentBlock,recommendR,'R');
	pshdy=pastY;
	pshdx=pastX;
	while(CheckToMove(field,currentBlock,pastRotate,pshdy,pshdx)){
		pshdy++;
	}
	pshdy--;
	DrawBlock(pshdy,pshdx,currentBlock,pastRotate,'.');
	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);
}
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y,x,blockID,blockRotate,' ');
	DrawShadow(y,x,blockID,blockRotate);
}
void BlockDown(int sig){
	// user code
	int i,j;
	time_t present;
	RecNode *tmp;
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)){
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	}
	else{
		if(blockY == -1){
			gameOver=1;
			return;
		}
		score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score+=DeleteLine(field);
		for(i=0;i<(BLOCK_NUM-1);i++)
			nextBlock[i]=nextBlock[i+1];		
		nextBlock[BLOCK_NUM-1]=rand()%7;
		blockY=-1;
		blockX=(WIDTH)/2-2;
		blockRotate=0;
		DrawField();
		PrintScore(score);
		if(!autoplay){/*
			for(i=0;i<CHILDREN_MAX;i++){
				tmp=recRoot->c[i];
				if(tmp == NULL)
					break;
				else if(tmp->x == blockX && tmp->y == blockY && tmp->r == blockRotate){
					for(j=i+1;j<CHILDREN_MAX;j++){
						if(recRoot->c[j] == NULL)
							break;
						del_recnode(recRoot->c[j]);
					}
					tmp=recRoot;
					recRoot=recRoot->c[i];
					free(tmp);
					renew_recnode(recRoot);
					break;
				}
				else
					del_recnode(tmp);
			}*/
			del_recnode(recRoot);
			recRoot=(RecNode*)malloc(sizeof(RecNode));
			recRoot->lv=0;
			recRoot->score=0;
			for(i=0;i<HEIGHT;i++)
				for(j=0;j<WIDTH;j++)
					recRoot->f[i][j] = field[i][j];
			for(i=0;i<CHILDREN_MAX;i++)
				recRoot->c[i]=NULL;
			recommend(recRoot);
		}
		else
			Init_recommend();

		DrawBlock(recommendY,recommendX,nextBlock[0],recommendR,'R');
		DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
		DrawNextBlock(nextBlock);
	}
	if(autoplay){
		present=clock();
		move(16,WIDTH+11);
		printw("%.6lf sec",(double)(present-gamestart)/(double)CLOCKS_PER_SEC);
	}
	timed_out=0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i,j,k;
	int x,y;
	int num=0,result;
	RecNode *tmp,*del;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j] == 1){
				x=blockX+j;
				y=blockY+i;
				f[y][x]=1;
				if(y==(HEIGHT-1) || f[y+1][x] == 1)
					num++;
			}
		}
	result=num*10;
	return result;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int Max_Height;
	int i,j;
	int del_i,del_j;
	bool flag=false;
	int line,result;
	line=0;
	Max_Height=0;
	while(Max_Height<HEIGHT){
		for(j=0;j<WIDTH;j++)
			if(f[Max_Height][j]){
				flag=true;
				break;
			}
		if(flag)
			break;
		Max_Height++;
	}
	for(i=HEIGHT-1;i>=Max_Height;i--){
		for(j=0;j<WIDTH;j++){
			if(f[i][j]==0)
				break;
		}
		if(j == WIDTH){
			line+=1;
			for(del_i=i-1;del_i>=Max_Height;del_i--)
				for(del_j=0;del_j<WIDTH;del_j++)
					f[del_i+1][del_j]=f[del_i][del_j];
			for(del_j=0;del_j<WIDTH;del_j++)
				f[Max_Height][del_j]=0;
			Max_Height++;
			i++;
		}
	}
	result=line*line*100;
	return result;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	int shdy,shdx;
	shdy=y;
	shdx=x;
	while(CheckToMove(field,blockID,blockRotate,shdy,shdx))
		shdy++;
	shdy--;
	DrawBlock(shdy,shdx,blockID,blockRotate,'/');
}
void createRankList(){
	// user code
	int num,per;
	int i,j,b;
	node *tmp,*pre;
	FILE *fp=fopen("rank.txt","r");
	head=(HEAD*)malloc(sizeof(HEAD));
	if(fscanf(fp,"%d%*c",&num)==EOF){
		head->total_num=0;
		return;
	}
	head->total_num=num;
	pre=NULL;
	if(num<=search){
		for(i=0;i<num;i++){
			tmp=(node*)malloc(sizeof(node));
			fscanf(fp,"%s %d%*c",tmp->name,&(tmp->score));
			tmp->num=1;
			tmp->llink=pre;
			if(pre)
				pre->rlink=tmp;
			pre=tmp;
			head->link[i]=tmp;
		}
		tmp->rlink=NULL;
		for(;i<search;i++)
			head->link[i]=NULL;
	}
	else{
		per=((num-1)/search)+1;
		b=num-(per*search)+search;
		for(i=0;i<search;i++){
			if(i == b)
				per--;
			for(j=0;j<per;j++){
				tmp=(node*)malloc(sizeof(node));
				fscanf(fp,"%s %d%*c",tmp->name,&(tmp->score));
				tmp->llink=pre;
				if(pre)
					pre->rlink=tmp;
				pre=tmp;
				if(j==0){
					tmp->num=per;
					head->link[i]=tmp;
				}
			}
		}
		tmp->rlink=NULL;
	}
}

void rank(){
	// user code
	clear();
	printw("1. list ranks from X to Y\n");
	refresh();
	printw("2. list ranks by a specific name\n");
	refresh();
	printw("3. delete a specific rank\n");
	refresh();
	noecho();
	switch(wgetch(stdscr)){
		case '1': displayrank(); break;
		case '2': findbyname();break;
		case '3': deleterank(); break;
	}
}
void findbyname(){
	char want[NAMELEN];
	node *cur;
	int flag;
	int len,i;
	printw("input the name: ");
	refresh();
	echo();
	scanw("%s",want);
	printw("		name		|	score	\n");
	printw("------------------------------------------------------------\n");
	refresh();
	flag=0;
	cur=head->link[0];
	while(cur){
		if((cur->name[0] == want[0]) && (strlen(cur->name) == strlen(want))){
			len=strlen(want);
			for(i=0;i<len;i++)
				if(cur->name[i] != want[i])
					break;
			if(i == len){
				printw(" %-31s|	%d\n",want,cur->score);
				refresh();
				flag=1;
			}
		}
		cur=cur->rlink;
	}
	if(!flag){
		printw("search failure: no name in the list\n");
		refresh();
	}
	getch();
}
void displayrank(){
	int x,y,i;
	int k,dep,num;
	node* tmp;
	echo();
	x=y=-1;
	printw("X: ");
	refresh();
	scanw("%d",&x);
	printw("Y: ");
	refresh();
	scanw("%d",&y);
	printw("		name			|	score	\n");
	printw("---------------------------------------------------------------------\n");
	if(x>y || y>head->total_num){
		printw("search failure: no rank in the list\n");
		getch();
		return;
	}
	if(x== -1 || x == 1){
		x=1;
		tmp=head->link[0];
	}
	else{
		num=0;
		i=0;
		while(num<x){
			num+=head->link[i]->num;
			i++;
		}
		i--;
		num-=head->link[i]->num;
		tmp=head->link[i];
		while(num<(x-1)){
			tmp=tmp->rlink;
			num++;
		}
	}
	if(y == -1){
		y=head->total_num;
	}
	for(k=x;k<=y;k++){
		printw(" %-39s|	%d\n",tmp->name,tmp->score);
		tmp=tmp->rlink;
	}
	getch();

}
void deleterank(){
	int i, k,j,num;
	int del,judge;
	node *pre,*cur,*tmp;
	printw("input the rank: ");
	refresh();
	echo();
	scanw("%d",&del);
	num=head->total_num;
	if(del<1 || del>num){
		printw("search failure: the rank not in the list\n");
		getch();
		return;
	}
	if(num<=search){
		cur=head->link[del-1];
		pre=cur->llink;
		if(pre){
			pre->rlink=cur->rlink;
		}
		if(cur->rlink)
			cur->rlink->llink=pre;
		tmp=cur;
		cur=cur->rlink;
		free(tmp);
		tmp=NULL;
		while(cur){
			head->link[del-1]=cur;
			del++;
			cur=cur->rlink;
		}
	}
	else{
		judge= (num-1)/search+1;
		num=0;
		//j 구하기
		for(j=0;j<search && del>num ;j++){
			num+=head->link[j]->num;
		}
		//i 구하기
		for(k=0,i=search;k<search;k++){
			if(head->link[k]->num < judge){
				i=k;
				break;
			}
		}
		num-=head->link[j-1]->num;
		del--;
		//삭제과정
		if(num == del){
			cur=head->link[j-1];
			pre=cur->llink;
			head->link[j-1]->rlink->num=head->link[j-1]->num;
			head->link[j-1]=head->link[j-1]->rlink;
			tmp=cur;
			cur=cur->rlink;
			if(pre)
				pre->rlink=cur;
			if(cur)
				cur->llink=pre;
			free(tmp);
			tmp=NULL;
		}
		else{
			cur=head->link[j-1];
			while(num != del){
				cur=cur->rlink;
				num++;
			}
			pre=cur->llink;
			tmp=cur;
			cur=cur->rlink;
			if(pre)
				pre->rlink=cur;
			if(cur)
				cur->llink=pre;
			free(tmp);
			tmp=NULL;
		}
		//인덱스 값 변경
		for(k=i;k<j;k++){
			head->link[k]=head->link[k]->llink;
			head->link[k]->num=head->link[k]->rlink->num;
			head->link[k]->rlink->num=0;
		}
		for(k=j;k<i;k++){
			head->link[k]=head->link[k]->rlink;
			head->link[k]->num=head->link[k]->llink->num;
			head->link[k]->llink->num=0;
		}

		head->link[i-1]->num = (judge-1);
	}

	head->total_num-=1;
	printw("\nresult: the rank deleted\n");
	getch();
}
void writeRankFile(){
	// user code
	FILE *fp=fopen("rank.txt","w");
	node *tmp,*pre;
	tmp=head->link[0];
	fprintf(fp,"%d\n",head->total_num);
	while(tmp){
		fprintf(fp,"%s %d\n",tmp->name,tmp->score);
		pre=tmp;
		tmp=tmp->rlink;
		free(pre);
		pre=NULL;
	}
	free(head);
	head=NULL;
}
void newRank(int score){
	// user code
	int num;
	int judge;
	int i,j,k;
	int flag1,flag2;

	clear();
	head->total_num+=1;
	num=head->total_num;
	judge=((num-1)/search)+1;
	node *tmp,*pre,*next;
	flag1=flag2=true;
	for(k=0,i=search,j=search;k<search;k++){
		tmp=head->link[k];
		if(!tmp)
			break;
		if(!(flag1 || flag2))
			break;
		if(flag1 && tmp->num < judge){
			flag1=false;
			i=k;
		}
		if(flag2 && tmp->score < score){
			flag2=false;
			j=k;
		}
	}
	if(flag1)
		i=k;
	if(flag2)
		j=k;
	//insertion
	tmp=(node*)malloc(sizeof(node));
	printw("your name: ");
	refresh();
	echo();
	scanw("%s",tmp->name);
	tmp->score=score;
	tmp->num=0;
	if(j==0){
		tmp->llink=NULL;
		tmp->rlink=head->link[j];
		head->link[j]->llink=tmp;
	}
	else{
		next=head->link[j-1];
		while(next){
			if(next->score<score)
				break;
			pre=next;
			next=next->rlink;
		}
		pre->rlink=tmp;
		tmp->llink=pre;
		if(next)
			next->llink=tmp;
		tmp->rlink=next;
	}
	//head->link 변경
	if(num<search){
		for(k=j;tmp;k++){
			head->link[k]=tmp;
			tmp=tmp->rlink;
		}
		for(k=j;k<=i;k++){
			head->link[k]=head->link[k]->llink;
			head->link[k]->num=judge;
			head->link[k]->rlink->num=0;
		}

	}
	head->link[i]->num=judge;
}
void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	int block_r,block_y,block_x,temp_score,idx=0;
	int i,j,r,x,y,lv,tmp;

	/*if(root->c[0] != NULL){
		for(i=0;i<CHILDREN_MAX;i++){
			if(root->c[i] == NULL)
				break;
			temp_score=recommend(root->c[i]);
			if(max<temp_score){
				max=temp_score;
				block_r=root->c[i]->r;
				block_x=root->c[i]->x;
				block_y=root->c[i]->y;
			}
		}
	}*/

	RecNode *new_node;
	lv=root->lv+1;
	for(r=0;r<4;r++){
		if(r == 1 && nextBlock[lv-1] == 4)
			break;
		else if(r == 2){
			tmp=nextBlock[lv-1];
			if(tmp == 0 || tmp == 5 || tmp == 6)
				break;
		}
		for(x=-2;x<WIDTH;x++){
			y=-1;
			while(CheckToMove(root->f,nextBlock[lv-1],r,y,x))
				y++;
			if(y == -1)
				continue;
				y--;
			new_node=(RecNode*)malloc(sizeof(RecNode));
			new_node->lv=lv;
			new_node->score=root->score;
			for(i=0;i<HEIGHT;i++)
				for(j=0;j<WIDTH;j++)
					new_node->f[i][j]=root->f[i][j];
			for(i=0;i<CHILDREN_MAX;i++)
				new_node->c[i]=NULL;
			new_node->score+=AddBlockToField(new_node->f,nextBlock[lv-1],r,y,x);
			new_node->score+=DeleteLine(new_node->f);
			root->c[idx++]=new_node;
			if(new_node->lv < BLOCK_NUM){
				temp_score=recommend(new_node);
			}
			else
				temp_score=new_node->score;
			if(max<temp_score){
				max=temp_score;
				block_r=r;
				block_x=x;
				block_y=y;
			}
		}
	}
	if(root->lv == 0){
		recommendR=block_r;
		recommendY=block_y;
		recommendX=block_x;
	}
	return max;
}
void del_recnode(RecNode *del){
	int lv=del->lv;
	int i;
	if(lv == BLOCK_NUM)
		free(del);
	else{
		for(i=0;i<CHILDREN_MAX;i++){
			if(del->c[i] == NULL)
				break;
			del_recnode(del->c[i]);
		}
		free(del);
	}
	del=NULL;
}/*
void renew_recnode(RecNode *node){
	int lv=node->lv;
	int i;
	if(lv == BLOCK_NUM)
		node->lv-=1;
	else{
		for(i=0;i<CHILDREN_MAX;i++){
			if(node->c[i] == NULL)
				break;
			renew_recnode(node->c[i]);
		}
		node->lv-=1;
	}
}*/
void recommendedPlay(){
	int command;

	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	duration=0;
	InitTetris();
	AddOutline();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command = GetCommand();
		if(command == QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
		if(recommendR != blockRotate)
			ProcessCommand(KEY_UP);
		if(recommendX < blockX)
			ProcessCommand(KEY_LEFT);
		else if(recommendX>blockX)
			ProcessCommand(KEY_RIGHT);
		
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
}
void Init_recommend(){
	// user code
	int r,c,height,max_r;
	int i;
	int flag;
	double temp;
	long size;
	time_t start,end;
	start=clock();
	recommend_node *root;
	root=(recommend_node*)calloc(1,sizeof(recommend_node));

	//field에서 최고 높이구하기
	flag=0;
	for(r=0;r<HEIGHT;r++){
		for(c=0;c<WIDTH;c++)
			if(field[r][c]){
				flag=1;
				break;
			}
		if(flag)
			break;
	}
	max_r=r;
	height=HEIGHT-max_r;
	root->max_h=height;

	//num 초기화
	
	for(i=0;i<height;i++)
		root->num[i]=0;

	//field살펴보면서 정보를 root노드에 저장해두기
	
	for(i=0;i<WIDTH;i++)
		root->h[i]=HEIGHT;
	for(c=0;c<WIDTH;c++){
		height=HEIGHT-1-max_r;
		flag=0;
		for(r=max_r;r<HEIGHT;r++,height--){
			if(field[r][c]){
				if(!flag){
					root->h[c]=r;
					flag=1;
				}
				root->num[height]++;
			}
		}
	}
	root->lv=0;
	root->score=0;
	modified_recommend(root);
	size=sizeof(root);
	move(21,WIDTH+11);
	printw("%ld bytes",size);
	free(root);
	root=NULL;
	end=clock();
	temp=((double)(end-start)/(double)CLOCKS_PER_SEC);
	duration+=temp;
	move(16,WIDTH+29);
	printw("%.6lf sec",duration);
}
int modified_recommend(recommend_node* t){
	int Max=0,Max_H=22,Max_blank=2;
	int x,y,r,lv,id;
	int r1,c1,r2,c2,blank;
	int i,j,result_x,result_y,result_r;
	int temp,line,bottom,height;
	int flag_max_h,exit,flag;
	recommend_node* New_node;
	lv=t->lv+1;
	id=nextBlock[lv-1];

	// 블록의 모든 경우를 살펴보기
	
	for(r=0;r<4;r++){
		if(id == 4 && r == 1)
			break;
		else if(id ==0 || id == 5 || id == 6 )
			if(r == 2)
				break;
		for(x=-2;x<WIDTH-1;x++){
			blank=0;
			exit=0;
			flag=0;
			if(x<0){
				for(c1=0;c1<4;c1++){
					for(r1=0;r1<4;r1++)
						if(block[id][r][r1][c1]){
							flag=1;
							break;
						}
					if(flag)
						break;
				}
				if(x+c1<0)
					continue;
			}
			else if(x>(WIDTH-4)){
				for(c1=3;c1>=0;c1--){
					for(r1=0;r1<4;r1++)
						if(block[id][r][r1][c1]){
							flag=1;
							break;
						}
					if(flag)
						break;
				}
				if(x+c1>=WIDTH)
					break;
			}
			//제일 왼쪽아래 (r1,c1)
			flag=0;
			for(r1=3;r1>=0;r1--){
				for(c1=0;c1<4;c1++)
					if(block[id][r][r1][c1]){
						flag=1;
						break;
					}
				if(flag)
					break;
			}
			//가장 아래 블록만 보면서 블록의 y좌표를 결정한다

			y=t->h[x+c1]-1-r1; // 제일 왼쪽아래가 바닥에 닿게 y값 초기화
			bottom=0; // 바닥 닿는 면적
			flag=0;
			exit=0;
			for(c2=0;c2<4;c2++){
				for(r2=r1;r2>=0;r2--){
					if(block[id][r][r2][c2]){
						temp=t->h[x+c2]-(y+r2);
						if(temp<=0){
							blank+=(-temp+1)*(flag);
							y-=(-temp+1);
							bottom=1;
						}
						else
							blank+=(temp-1);
						if(temp == 1)
							bottom++;
						flag++;
						break;
					}
				}
			}			
			//root노드 정보대로 입력

			New_node=(recommend_node*)calloc(1,sizeof(recommend_node));
			New_node->max_h=(t->max_h);
			New_node->score=(t->score);
			for(i=0;i<(t->max_h);i++)
				New_node->num[i]=t->num[i];
			for(i=0;i<WIDTH;i++)
				New_node->h[i]=t->h[i];
			New_node->lv=lv;

			//블록을 올렸을 때의 정보로 업데이트

			flag_max_h=0;
			line=0;
			for(i=0; i<=r1 ; i++){
				height=HEIGHT-1-(y+i);
				for(j=0;j<4;j++){
					if(block[id][r][i][j]){

						//최고 높이 업데이트

						if(!flag_max_h){
							flag_max_h=HEIGHT-(y+i);
							temp=(New_node->max_h);

							//기존보다 블록이 있는 높이가 더 높을 때

							if(temp<flag_max_h){
								New_node->max_h=flag_max_h;
								while(temp != flag_max_h)
									New_node->num[temp++]=0;
							}
						}

						//h배열 업데이트

						temp=y+i;
						if(New_node->h[x+j]>temp)
							New_node->h[x+j]=temp;
						
						//num배열 업데이트

						New_node->num[height]+=1;
					}
				}
			}

			//지울 수 있는 line찾아서 수정하기

			height=(New_node->max_h);
			for(i=(height-1);i>=0;i--){
				if((New_node->num[i]) == WIDTH){
					line++;
					temp=HEIGHT-1-i;
					for(j=0;j<WIDTH;j++){
						if(New_node->h[j] <= temp)
							New_node->h[j] +=1;
					}
					for(j=i+1;j<height;j++)
						New_node->num[j-1]=New_node->num[j];
					New_node->num[(height-1)]=0;
					height--;
				}
			}
			New_node->score+=(line*line*100)+(bottom*10);
			New_node->max_h=height;
			if(line>0 || blank<=1){
				if(lv<BLOCK_NUM)
					temp=modified_recommend(New_node);
				else
					temp=New_node->score;
				if(temp>Max){
					Max=temp;
					Max_H=height;
					Max_blank=blank;
					result_x=x;
					result_r=r;
					result_y=y;
				}
				else if(temp == Max){
					if(Max_blank>blank || height<Max_H){
						result_x=x;
						result_r=r;
						result_y=y;
						Max_blank=blank;
						Max_H=height;
					}
				}					
			}
			free(New_node);
			New_node=NULL;
		}
	}
	if(lv == 1){
		recommendX=result_x;
		recommendY=result_y;
		recommendR=result_r;
	}
	return Max;
}
void AddOutline(){
	move(14,WIDTH+10);
	printw("time");
	DrawBox(15,WIDTH+10,1,15);
	move(14, WIDTH+28);
	printw("time(t)");
	DrawBox(15,WIDTH+28,1,15);
	move(19,WIDTH+10);
	printw("space(t)");
	DrawBox(20,WIDTH+10,1,15);
}
