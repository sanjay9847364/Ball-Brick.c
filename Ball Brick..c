#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>

struct cell{
  // property of each cell in the game
  bool isWall,isGround;
  bool isNumericalBrick;
  bool isDemolishingBrick;
  bool isPoweredBrick;
  union {int weight; char type;}brick;
};

struct cell **arr;
int n;
int ball_count, brick_count=0;
int base=0, base_c;
int ball_r, ball_c;
bool gameover=false;

void load_grid(){
  char choice='y',type[3];
  int r,c,weight;
  do{
    printf("Enter the brick's position and the brick type : ");
    scanf("%d %d %s",&r,&c,type);

    //boundary conditions
    if(r<0 || r>=n-1 || c<0 || c>=n-1)continue;

    //increment block_count only for new blocks
    if(arr[r][c].isNumericalBrick==true && arr[r][c].brick.weight==0)brick_count++;

    if(strcmp(type,"B")==0){
      arr[r][c].isNumericalBrick=false;
      arr[r][c].isPoweredBrick=true;
    }
    else if(strcmp(type,"DS")==0){
      arr[r][c].isNumericalBrick=false;
      arr[r][c].isDemolishingBrick=true;
      arr[r][c].brick.type='S';
    }
    else if(strcmp(type,"DE")==0){
      arr[r][c].isNumericalBrick=false;
      arr[r][c].isDemolishingBrick=true;
      arr[r][c].brick.type='E';
    }
    else{
      weight = atoi(type);
      arr[r][c].isNumericalBrick=true;
      arr[r][c].brick.weight=weight;
      if(weight==0)brick_count--;  //weight=0 means no block/empty
    }

    printf("Do you want to continue(Y or N)?");
    scanf(" %c",&choice);

  }while(choice=='y' || choice =='Y');
  if(brick_count==0)gameover=true;
}

void print_grid(){
  //calculate position of the base rudder
  int base_left = base_c-(int)(base/2);
  if(base_left<1)base_left=1;
  int base_right = base_left+base;
  if(base_right>=n)base_right=n-1;

  for(int r=0;r<n;r++){
    for(int c=0;c<n;c++){
      if(arr[r][c].isWall)printf("W ");
      else if(arr[r][c].isGround){
        if(ball_c==c)printf("o ");
        else if(c>=base_left && c<=base_right)printf("__");
        else printf("G ");
      }
      else if(arr[r][c].isNumericalBrick){
        if(arr[r][c].brick.weight==0)printf("  ");
        else printf("%d ",arr[r][c].brick.weight);
      }
      else if(arr[r][c].isDemolishingBrick)printf("D%c",arr[r][c].brick.type);
      else if(arr[r][c].isPoweredBrick)printf("B ");
      else printf("  ");
    }
    printf("\n");
  }
  printf("Ball count is %d\n",ball_count);
}

void demolishTheBrick(int r, int c){
  if(r>n || r<0 || c>n || c<0)return;
  else if(arr[r][c].isWall)return;
  else if(arr[r][c].isGround)return;
  else if(arr[r][c].isDemolishingBrick){
    char type = arr[r][c].brick.type;
    arr[r][c].isDemolishingBrick=false;
    arr[r][c].isNumericalBrick=true;
    arr[r][c].brick.weight=0;
    brick_count--;
    if(brick_count==0){
      gameover=true;
      return;
    }
    if(type=='S'){
      demolishTheBrick(r-1,c-1);demolishTheBrick(r-1,c);demolishTheBrick(r-1,c+1);
      demolishTheBrick(r,c-1);                          demolishTheBrick(r,c+1);
      demolishTheBrick(r+1,c-1);demolishTheBrick(r+1,c);demolishTheBrick(r+1,c+1);
    }
    else if(type=='E'){
      //entire row
      for(int i=0;i<n;i++)demolishTheBrick(r,i);
    }
  }
  else if(arr[r][c].isPoweredBrick){
    arr[r][c].isPoweredBrick=false;
    arr[r][c].isNumericalBrick=true;
    arr[r][c].brick.weight=0;
    base++;
    brick_count--;
    if(brick_count==0)gameover=true;
  }
  else if(arr[r][c].isNumericalBrick){
    if(arr[r][c].brick.weight!=0)brick_count--;
    arr[r][c].brick.weight=0;
    if(brick_count==0)gameover=true;
  }
}

void traverse(char dir[3]){
  //how to traverse the ball - the increments only
  int x=-1,y=0;
  if(strcmp(dir,"ST")==0)y=0;
  else if(strcmp(dir,"LD")==0)y=-1;
  else if(strcmp(dir,"RD")==0)y=1;

  bool hitWall=false;
  ball_r=n-1;
  while(!gameover){
    ball_r+=x;ball_c+=y;
    // printf("(%d,%d)\n",ball_r,ball_c);

    if(arr[ball_r][ball_c].isGround)break;
    else if(arr[ball_r][ball_c].isWall){
      if(hitWall){ // the ball is lost
        ball_count--;
        if(ball_count<1)gameover=true;
        ball_c=n/2;
        ball_r=n-1;
      }
      else {
        hitWall=true;
        x = 0;
        if(y==-1)y=1;
        else if(y==1)y=-1;
        else if(y==0)x=1;
      }
    }
    else if(arr[ball_r][ball_c].isNumericalBrick){
      if(arr[ball_r][ball_c].brick.weight>0){
        arr[ball_r][ball_c].brick.weight--;
        if(arr[ball_r][ball_c].brick.weight==0)brick_count--;
        if(brick_count==0)gameover=true;
        hitWall=false;
        ball_r-=x;
        x=1; y=0;
      }
    }
    else if(arr[ball_r][ball_c].isDemolishingBrick){
      demolishTheBrick(ball_r,ball_c);
      hitWall=false;
      ball_r-=x;
      x=1; y=0;
    }
    else if(arr[ball_r][ball_c].isPoweredBrick){
      arr[ball_r][ball_c].isPoweredBrick=false;
      arr[ball_r][ball_c].isNumericalBrick=true;
      arr[ball_r][ball_c].brick.weight=0;
      brick_count--;
      if(brick_count==0)gameover=true;
      base++;
      hitWall=false;
      ball_r-=x;
      x=1; y=0;
    }
  }
  //calculate base rudder space
  int base_left = base_c-(int)(base/2);
  if(base_left<1)base_left=1;
  int base_right = base_left+base;
  if(base_right>=n)base_right=n-1;

  ball_r=n-1;
  if(ball_c<base_left || ball_c>base_right){
    ball_count--;
    ball_c=n/2;
    if(ball_count<1)gameover=true;
  }
}

void main(){
  printf("Enter the size of the N*N matrix : ");
  scanf("%d",&n);
  brick_count=0;base=0;
  base_c=n/2;
  ball_c=n/2;
  ball_r=n-1;
  gameover=false;

  arr = malloc(n*sizeof(struct cell*));
  for(int r=0;r<n;r++){
    arr[r]=malloc(n*sizeof(struct cell));
    for(int c=0;c<n;c++){
      if(c==0 || c==n-1 || r==0)arr[r][c].isWall=true;
      else if(r==n-1)arr[r][c].isGround=true;
      else{
        arr[r][c].isNumericalBrick=true;  //default value
        arr[r][c].brick.weight=0;
      }
    }
  }

  load_grid();
  printf("Enter ball Count : ");
  scanf("%d",&ball_count);
  print_grid();

  char dir[3];
  while(!gameover){
    printf("Enter the direction in which the ball need to traverse : ");
    scanf("%s",dir);
    traverse(dir);
    print_grid();
  }
  if(brick_count==0)printf("You win Hurray..!!\n");
  else printf("You lose GAME OVER..!!\n");  //ball_count = 0

  for(int r=0;r<n;r++)free(arr[r]);
  free(arr);
}

