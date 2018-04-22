/******************************************************************
 * FileName : Astar_search_algorithm.c
 * Author   : Lishuxiang
 * E-mail   : lishuxiang@cug.edu.cn
 * Time     : Sun 22 Apr 2018 08:52:15 AM CST
 * Function : A* search algorithm
******************************************************************/

#include<stdio.h> 
#include<stdlib.h>

#define MAP_HEIGHT 5
#define MAP_WIDTH 7

int openset_size=0;
int closedset_size=0;
int pathset_size=0;

struct node {
	int x,y;
	int f,g,h; //f(x)=g(x)+h(x)
	struct node *p; //point ot parent node
	struct node *next;
	int is_openset;
	int is_closedset;
};

struct node *addToOpenset(struct node *set,struct node *which) {
	struct node* head=set;
	openset_size++;
	which->next=NULL;
	which->is_openset=1;
	if(openset_size-1==0) {
		return which;
	}
	while(set->next!=NULL){
		set=set->next;
	}
	set->next=which;
	return head;
}

struct node *addToClosedset(struct node *set,struct node* which) {
	struct node* head=set;
	closedset_size++;
	which->next=NULL;
	which->is_closedset=1;
	if(closedset_size-1==0) {
		return which;
	}
	while(set->next!=NULL){
		set=set->next;
	}
	set->next=which;
	return head;
}

struct node *removeFromOpenset(struct node *set,struct node *which) {
	struct node *head=set;
	openset_size--;
	which->is_openset=0;
	which->is_closedset=1;
	if(openset_size==0) {
		return NULL;
	}
	int first=1;
	while(set->x!=which->x&&set->y!=which->y) {
		set=set->next;
		first=0;
	}
	if(first==0){
		set->next=which->next;
	} else {
		return which->next;
	}
	return head;
}

void cal(struct node* n,int goal_x,int goal_y) {
	n->g = n->p->g + abs(n->p->x - n->x) + abs(n->p->y - n->y);
	n->h = abs(goal_x - n->x) + abs(goal_y - n->y);
	n->f = n->g + n->h;
}

struct node *f_min(struct node* set) {
	struct node *ret=set;
	while(set==NULL){
		if(ret->f>set->f){
			ret=set;
		}
		set=set->next;
	}
	return ret;
}

struct node* pick_node(struct node* set,int x,int y) {
	while(set!=NULL){
		if(set->x==x&&set->y==y) {
			return set;
		}
		set=set->next;
	}
	return NULL;
}

struct node* beside(struct node* openset,struct node* closedset,struct node* now,int x,int y,int goal_x,int goal_y) {
	//return openset
	//重复,重新计算f,g,h判断走本条路径时是否更优
	struct node* old_p;
	int old_f,new_f;
	struct node* tmp;
	tmp=pick_node(openset,x,y);
	if(tmp!=NULL){
		//重复 判断当前路线是否更优
		old_p=tmp->p;
		old_f=tmp->f;
		tmp->p=now;
		cal(tmp,goal_x,goal_y);
		new_f=tmp->f;
		if(new_f>old_f) {
			//还原
			tmp->p=old_p;
			cal(tmp,goal_x,goal_y);
		}
	} else {
		tmp=pick_node(closedset,x,y);
		if(tmp==NULL) {
			tmp = (struct node*) malloc(sizeof(struct node));
			tmp->x=x;
			tmp->y=y;
			tmp->p=now;
			tmp->is_closedset=0;
			tmp->is_openset=1;
			cal(tmp,goal_x,goal_y);
			openset=addToOpenset(openset,tmp);
		}
	}
	return openset;
}

struct node *AStar(int map[MAP_HEIGHT][MAP_WIDTH],struct node *start,struct node *goal) {
	struct node* openset = addToOpenset(NULL,start);
	struct node* closedset = NULL;
	struct node* now;
	while(openset_size!=0) {
		now=f_min(openset);
		if(now->x==goal->x&&now->y==goal->y) {
			return now;
		}
		openset=removeFromOpenset(openset,now);
		closedset=addToClosedset(closedset,now);
		//添加相邻可到达节点到openset
		if(now->x!=0&&map[now->x-1][now->y]!=4) {
			openset=beside(openset,closedset,now,now->x-1,now->y,goal->x,goal->y);
		}
		if(now->x<MAP_HEIGHT-1&&map[now->x+1][now->y]!=4) {
			openset=beside(openset,closedset,now,now->x+1,now->y,goal->x,goal->y);
		}
		if(now->y!=0&&map[now->x][now->y-1]!=4) {
			openset=beside(openset,closedset,now,now->x,now->y-1,goal->x,goal->y);
		}
		if(now->y<MAP_WIDTH-1&&map[now->x][now->y+1]!=4) {
			openset=beside(openset,closedset,now,now->x,now->y+1,goal->x,goal->y);
		}
	}
	return NULL;
}

void printMap(int map[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; ++i) {
		for (int j = 0; j < MAP_WIDTH; ++j) {
			printf("%d ", map[i][j]);
		}	
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	//0 for space,1 for start,2 for goal,4 for block
	int map[MAP_HEIGHT][MAP_WIDTH] = {
		{0,0,0,4,0,0,0},
		{0,0,0,4,0,0,0},
		{0,1,0,4,0,2,0},
		{0,0,0,4,0,0,0},
		{0,0,0,4,0,0,0}
	};
	printf("map:\n");
	printMap(map);
	struct node start,goal;
	start.x=2;
	start.y=1;
	start.p=NULL;
	goal.x=2;
	goal.y=5;
	struct node *path = AStar(map,&start,&goal);
	if(path==NULL){
		//寻路失败
		printf("failed\n");
	} else {
		//在地图上填充路径
		while(path!=NULL) {
			map[path->x][path->y]=3;
			path=path->p;
		}
		//输出路径地图
		printf("\npath:\n");
		printMap(map);
	}
	return 0;
}
