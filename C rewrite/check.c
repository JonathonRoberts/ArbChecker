#include <stdio.h>
#include <ctype.h>

/*Requires libxml*/

float profit(int noodds,float odds[]);
int getodds(char url[]);
int getgames(char url[]);
void crawlsite();
void uniq();
void list();
void getallmarkets();
void findall();

int main(){
   return 1;
}

float profit(int noodds,float odds[]){
	float percentageprofit = 0;
	int i;
	for(i=0;i<=noodds;i++){
		percentageprofit+=(100/odds[i]);
	}
	return 100-percentageprofit;
}
int getodds(char url[]){
	p
}
int getgames(char url[]){
}
void crawlsite(){
}
void uniq(){
}
void list(){
}
void getallmarkets(){
}
void findall(){
}
