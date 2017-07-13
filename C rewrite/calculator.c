#include <stdio.h>
#include <string.h>
#include <stdlib.h>

float setreturn(int noodds, float odds[]);
void changeodds(int *noodds, float *op);

int main(int argc, char *argv[]){
	int i;
	int noodds = argc-1;//number of odds
	float odds[20];
	float *op;//odds pointer required for changeodds()
	op = odds;
	if(noodds<2){
		system("clear");
		printf("Two or more odds required\n");
		changeodds(&noodds,op);
	}
	else{
		for(i=1;i<argc;i++){
			char oddstring[20];
			char comstring[20];
			if(sscanf(argv[i],"%[.-9]-%[.-9^/]",oddstring,comstring)==2){
				odds[i-1] = atof(oddstring)*(1-(atof(comstring)/100));
			}
			else if(sscanf(argv[i],"%[.-9^/]",oddstring)==1){
				odds[i-1] = atof(oddstring);
			}
			else{
				printf("Invalid input\n");
				changeodds(&noodds,op);
			}
			oddstring[0]=0;
			comstring[0]=0;
		}
	}
	float returnodds = setreturn(noodds, odds);
	float totalstake = 100;
	int firstrunflag = 0;
	char str[15];
	int a;
	float returncash = totalstake * returnodds;
	while(1){
		if(firstrunflag == 0){
			firstrunflag++;
		}
		else{
			fgets(str,15,stdin);
			//if only one character is entered a = character entered
			if(str[2]==0){
				a = str[0];
			}
			switch(a){
				case 'c': {changeodds(&noodds,op);
					  returnodds = setreturn(noodds,odds);
					  returncash = totalstake*returnodds;break;}
				case 'q': return 0;
				default: {totalstake =atof(str);
						 returncash = totalstake*returnodds;}
			}
		}
		a = 0;
		//Output
		system("clear");
		printf("c - change odds : q - quit\n");
		printf("Total stake: %.2f\n",totalstake);
		printf("Total return:  %.2f\n",returncash);
		printf("Odds\tStake\n");
		for(i = 0;i<noodds;i++){
			printf("%.2f\t%.2f\n",odds[i],(returncash/odds[i]));
		}
		printf("Input total stake: ");
	}
}

float setreturn(int noods, float odds[]){
	int i;
	float returnodds = 0;
	for(i = 0;i<noods;i++){
		returnodds += (1/odds[i]);
	}
	return (1/returnodds);
}

void changeodds(int *noodds, float *op){
	char str[60];
	char *p;
	char tmpword[10];
	char oddstring[10];
	char comstring[10];
	printf("To add a commission, place it after the outcome seperated by \"-\"\ne.g: 2.75-2 2.8 3\n");
	do{
		printf("Enter odds seperated by a space: ");
		*noodds = 0;
		if (fgets(str, sizeof str, stdin) != NULL) {
			p = str;
			while (sscanf(p, "%s",tmpword ) != -1) {
				if(sscanf(tmpword,"%[.-9^/]-%[.-9^/]",oddstring,comstring)==2){
					op[(*noodds)++] = atof(oddstring)*(1-(atof(comstring)/100));
				}
				else if(sscanf(tmpword,"%[.-9^/]",oddstring)==1){
					op[(*noodds)++] = atof(oddstring);
				}
				else{//invalid input
					str[0]=0;
					*noodds=0;
					continue;
				}
					p+= strlen(tmpword)+1;
					oddstring[0] = 0;
					comstring[0] = 0;
			  }
		}
		if(*noodds<2)
			printf("Invalid input\n");
	}while((*noodds)<2);
}
