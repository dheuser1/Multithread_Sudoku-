#include<pthread.h>
#include<iostream>
#include<fstream>
#include<string>
#include<utility>
using namespace std;

//the puzzle
int data[9][9];

//this is what a thread runs
void* check (void* parameters)
{
	int *hold=(int*)parameters;
	int temp=0;
	bool seen[9];
	//set up seen, says if a number has been seen before or not
	for(int i=0; i<9; i++)
	{
		seen[i]=false;
	}
	//check a row
	if((*hold)<9)
	{
		for(int i=0; i<9; i++)
		{
			temp=data[*hold][i];
			//take away one to make it between 0 and 8
			temp--;
			//to big
			if(temp>8)
			{
				//do this so from one number we can get row from hold/10 and col from hold%10
				//all returns if an error are found are in this form
				*hold= ((*hold)*10+i);
				return (void*)hold;
			}
			//seen twice
			else if(seen[temp]==true)
			{
				*hold= ((*hold)*10+i);
				return (void*)hold;
			}
			//seen for first time
			else
			{
				seen[temp]=true;
			}
		}
		//no error
		*hold=-1;
		return (void*)hold;
	}
	//check a col
	else if((*hold)<18)
	{
		//want hold to be between 0 and 8
		(*hold)-=9;
		for(int i=0; i<9; i++)
		{
			temp=data[*hold][i];
			temp--;
			//to big
			if(temp>8)
			{
				*hold= ((*hold)*10+i);
				return (void*)hold;
			}
			//seen twice
			else if(seen[temp]==true)
			{
				*hold= ((*hold)*10+i);
				return (void*)hold;
			}
			//seen for first time
			else
			{
				seen[temp]=true;
			}
		}
		//no error
		*hold=-1;
		return (void*)hold;
	}
	//chack 3X3 box
	else
	{
		//want hold to be between 0 and 8
		(*hold)-=18;
		int row, col;
		//set up index for 3X3 box row and col can have value of 0,1,2
		row=(*hold)/3;
		col=(*hold)%3;
		//multiply by three for offest of ones that came before, if it is 0 nothing came before,
		//but X*0 is 0 so this still works in the case of row==0 or col==0
		row*=3;
		col*=3;
		for(int i=row; i<row+3; i++)
		{
			for(int j=col; j<col+3; j++)
			{
				temp=data[i][j];
				temp--;
				//to big
				if(temp>8)
				{
					*hold= (10*i+j);
					return (void*)hold;

				}
				//seen twice
				else if(seen[temp]==true)
				{
					*hold= (10*i+j);
					return (void*)hold;
				}
				//seen for first time
				else
				{
					seen[temp]=true;
				}
			}
		}
		//no error
		*hold=-1;
		return (void*)hold;
	}
}
//read in the file
void read(char* name)
{
	ifstream infile(name);
	int count=0;
	//this geta a line and reads it into the data array
	while(infile>>data[count][0]>>data[count][1]>>data[count][2]>>data[count][3]>>data[count][4]
	      >>data[count][5]>>data[count][6]>>data[count][7]>>data[count][8])
	{
		count++;
	}
}

int main(int argc, char* argv[])
{
	//read in the sodoku puzzle from the file
	read(argv[1]);

	//the array of pthread_t
	pthread_t thread_id[27];
	//the array of pthread_attr_t
	pthread_attr_t attr[27];
	//array of arguments
	int args[27];
	//hold what threads return
	void* ans[27];
	//see if puzzle was valid or not
	bool good=true;

	//get default attributes
	for(int i=0; i<27; i++)
	{
		pthread_attr_init(&attr[i]);
	}
	//these will be the args for the threads
	for(int i=0; i<27; i++)
	{
		args[i]=i;
	}
	//make threads
	for(int i=0; i<27; i++)
	{
		pthread_create(&thread_id[i], &attr[i], check, &args[i]);
	}
	//join threads
	for(int i=0; i<27; i++)
	{
		pthread_join(thread_id[i],&ans[i]);
	}

	//print error if any
	for(int i=0; i<27; i++)
	{
		if((*(int*)ans[i])==-1)
		{
			//no error do nothing
		}
		//note that puzzle goes 0-8, not 1-9
		else
		{
			int temp=(*(int*)ans[i]);
			if(i<9)
			{
				cout<<"row error at("<<temp/10<<","<<temp%10<<") value is "<<data[temp/10][temp%10]<<endl;
				good=false;
			}
			else if(i<18)
			{
				cout<<"col error at("<<temp/10<<","<<temp%10<<") value is "<<data[temp/10][temp%10]<<endl;
				good=false;
			}
			else
			{
				cout<<"3X3 box error at("<<temp/10<<","<<temp%10<<") value is "<<data[temp/10][temp%10]<<endl;
				good=false;
			}
		}
	}
	//no errors found
	if(good==true)
	{
		cout<<"looks good..."<<endl;
	}
	return 0;
}
