//trustgame.cpp

//Definition of basic setting
#include<iostream>
#include<boost/random.hpp>
#include<ctime>
#include <vector>
#include<list>
#include <algorithm>
#include <fstream>
#include <numeric>
using namespace std;


#define MU 0.025 
#define STRATEGY_I 3
#define STRATEGY 2
#define COMPETENCE
#define DPLAYER 250
#define RPLAYER 250
#define B 0.1 
#define STEP1 200 
#define STEP2 100 


const double P = 1; 
const double R = 4;
const double S = 0;
const double T = 6;


double UniformRandom()
{
  static boost::mt19937 gen(static_cast<unsigned int>(time(0)));
  static boost::uniform_real<> dst(0,1);
  static boost::variate_generator< boost::mt19937&,boost::uniform_real<> > rand(gen,dst);
  return rand();
}

//Definition of Donors

class D_Agent
{
  
  public:
  int ID;
  int payoff;
  int EG;
  int MG;
  int flag;
  int partner;

  //Constructor
  D_Agent();
  
  //function for mutation 
  void mutation() {
	double random=UniformRandom();
	double random2=UniformRandom();
	
	if(random<MU)
	{EG=rand()%STRATEGY;}

	if(random2<MU)
	{MG=rand()%STRATEGY;}
	
  }
};
  
 // Definition of Donors' constructor
 D_Agent::D_Agent()
 {
   ID=0;
   EG=0;
   MG=1;
   payoff=0;
  };
 
//Definition of Recipients
class R_Agent
{
  
  public:
  int ID;
  int payoff;
  int intention;
  double competence;
  int flag;
  int partner;
  int history;

  //Constructor
  R_Agent();
  
  //function for mutation
  void mutation() {
	double random=UniformRandom();
	if(random<MU)
	{intention=rand()%STRATEGY_I;}
	
  }
};
  
//Definition of Recipients' constructor
R_Agent::R_Agent()
{
  ID=0;
  intention=0;
  competence=0;
	payoff=0;
};

//Creating Game class
class Game
{
  D_Agent &donor;
  R_Agent &recipient;
  
  public:
  Game(D_Agent &d, R_Agent &r);
  void run();
};
  
//Definition of modified trust game 
Game::Game(D_Agent &d, R_Agent &r):
  donor(d),
  recipient(r)
  {};

void Game::run()
{  //When the recipient has intention to cooperate
  if( recipient.intention/(STRATEGY_I-1) > UniformRandom() )
	{ 
	//if the recipient succeeds to accomplish the task	
	 if( recipient.competence>UniformRandom())
	 {
	  donor.payoff = donor.payoff + R;
	  recipient.payoff = recipient.payoff + R;
	  recipient.history = 0;
	 }
	 else
	  {//if the recipient fails to accomplish the task
		donor.payoff = donor.payoff + S;
		recipient.payoff = recipient.payoff + S;
		recipient.history = 1;
	  }
    }
  else
  {//When the recopient does not have intention to cooperate
	//if the recipient succeeds to accomplish the task
	if( recipient.competence>UniformRandom())
	{
	  donor.payoff = donor.payoff + S;
	  recipient.payoff = recipient.payoff + T;
	  recipient.history = 1;
	}
	else
	{//if the recipient fails to accomplish the task
		donor.payoff = donor.payoff + S;
		recipient.payoff = recipient.payoff + S;
		recipient.history = 1;
	}
  }
}

//Definition of evolution
void D_evolution(D_Agent& x, D_Agent& y)
{
  if(x.payoff < y.payoff)
  {
	x.EG = y.EG;
	x.MG = y.MG;
  }
  else
  {
	y.EG = x.EG;
	y.MG = x.MG;
  }
}

void R_evolution(R_Agent& x, R_Agent& y)
{
  if(x.payoff < y.payoff)
  {
	x.intention = y.intention;
  }
  else
  {
	y.intention = x.intention;
  }
}

//main function
int main()
{
  srand(static_cast<unsigned int>(time(NULL)));
  
//creating and opening an file
ofstream fout("data.csv", ios_base::in | ios_base::trunc);
if(!fout) {cout <<"failure in file open !"<<endl;exit(1);}
fout<<"ave-Intention"<<","<<"ST"<<','<<"EG"<<','<<"MG"<<','<<"SG"<<endl; 
	
//creating the Donors
vector<D_Agent> d_agent(DPLAYER);
for (int i=0;i<DPLAYER;i++)
{
  d_agent[i].ID = i;
  d_agent[i].EG = (int)(UniformRandom()*100)%STRATEGY;
  d_agent[i].MG = (int)(UniformRandom()*100)%STRATEGY;
}
//creating the recipients	
vector<R_Agent> r_agent(RPLAYER);
for (int i=0;i<RPLAYER;i++)
{
  r_agent[i].ID = i+DPLAYER;
  r_agent[i].intention = (int)(UniformRandom()*100)%STRATEGY_I;
  r_agent[i].competence = 0.65;
  for (int st=0;st<STRATEGY-1;st++) r_agent[i].history = 0;	
}

//evolution loop (number of trials = STEP2)
for (int k=0;k<STEP2;k++)
{ //initializing
  int intention_sum=0;
  int ST=0;int EG=0;int MG=0;int SG=0;
  if(k%20==0){ cout<<k<<endl;}
  for (int i=0;i<DPLAYER;i++)
  {
	d_agent[i].payoff = 0;
	d_agent[i].partner = 9999;
	d_agent[i].flag = 0;
  }
  for (int i=0;i<RPLAYER;i++)
  {
	r_agent[i].payoff = 0;
	r_agent[i].history = 0;
	r_agent[i].partner = 9999;
	r_agent[i].flag = 0;
  }
  
  //Excecution of modified trust game (number of rounds = STEP1)
  for (int j=0;j<STEP1;j++)
  {
	//partner-matching
	random_shuffle(d_agent.begin(), d_agent.end());
	random_shuffle(r_agent.begin(), r_agent.end());
		for (int d=0;d<DPLAYER;d++)
			{if (d_agent[d].flag == 0)	
				{for (int r=0; r<RPLAYER;r++)	
				{if	(r_agent[r].history <= d_agent[d].MG & r_agent[r].flag == 0)
					{	d_agent[d].partner = r_agent[r].ID;
						r_agent[r].partner = d_agent[d].ID;
						d_agent[d].flag = 1;
						r_agent[r].flag = 1;
						break;
					}
				}
				}
			}
	
	//players without a partner get payoff p 
	for (int i=0;i<DPLAYER;i++)
	{  if (d_agent[i].flag == 0) d_agent[i].payoff = d_agent[i].payoff + P;}
	for (int i=0;i<RPLAYER;i++)
	{  if (r_agent[i].flag == 0) r_agent[i].payoff = r_agent[i].payoff + P;}
			
	//Donors with a partner play the modified trust game 
	for (int d=0;d<DPLAYER;d++)
	{
			for (int r=0; r<RPLAYER; r++){
			if (d_agent[d].partner == r_agent[r].ID)
			{
			  Game game(d_agent[d], r_agent[r]);
				game.run();
						
				//exit (commitment) process  		
				if (r_agent[r].history>d_agent[d].EG)//checking the current partner's action
				
				{ 
					d_agent[d].flag = 0;d_agent[d].partner = 9999;
					r_agent[r].flag = 0;r_agent[r].partner = 9999;
				} 
				else if (B>UniformRandom())//exogeniously breaking the partnership on the probability B 
				{ 
				d_agent[d].flag = 0;d_agent[d].partner = 9999;
				r_agent[r].flag = 0;r_agent[r].partner = 9999;
				} 
				}
			}
	}
	
  }
	
		
//evolution
random_shuffle(d_agent.begin(),d_agent.end());
for (int i=0;i<DPLAYER;i+=2)
{
   D_evolution(d_agent[i],d_agent[i+1]);
   d_agent[i].mutation();
   d_agent[i+1].mutation();
} 
  list<int> a1_index;
  list<int>::iterator a1_it = a1_index.begin(); 
  random_shuffle(r_agent.begin(),r_agent.end());
for (int i=0;i<RPLAYER;i++)	a1_index.push_back(i);

  a1_it=a1_index.begin();
  while(a1_it!=a1_index.end())
  {
	int a=*a1_it;
	++a1_it;
	int b=*a1_it;
	R_evolution(r_agent[a],r_agent[b]);
	r_agent[a].mutation();r_agent[b].mutation();
	if (a1_it!=a1_index.end()) ++a1_it;
	else cout <<"oops   " <<endl;
	}
//data output
for (int i =0; i<DPLAYER; i++)
{
	
	if (d_agent[i].EG == 0 & d_agent[i].MG == 0) ST+=1;
		else if (d_agent[i].EG == 1 & d_agent[i].MG == 0) EG+=1;
		else if (d_agent[i].EG == 0 & d_agent[i].MG == 1) MG+=1;
		else if (d_agent[i].EG == 1 & d_agent[i].MG == 1) SG+=1;
  }
  
for (int i =0; i<RPLAYER; i++)
{
intention_sum+=r_agent[i].intention;
}
	
	fout<<(double)intention_sum/((double)RPLAYER*2)<<","<<(double)ST/(double)DPLAYER<<","<<(double)EG/(double)DPLAYER<<","<<(double)MG/(double)DPLAYER<<","<<(double)SG/(double)DPLAYER<<endl; 
}
}

