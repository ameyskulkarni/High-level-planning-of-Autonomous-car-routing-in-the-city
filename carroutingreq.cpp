
#include <iostream>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <fstream>
#include "json/value.h"
#include "json/json.h"
#include <json/writer.h>



using namespace std;
using std::ofstream;
using namespace Json;

//Class to hold all the details of all the passengers
class Requests
{

public:
    string name;
    
    pair<int,int> start;
    pair<int,int> end;
    bool status=false;
    int time;
    
};


//Function intializations
void checkaddreq(Json::Value &, vector<Requests> &, vector<pair<int,int>> &,int);
tuple<int,int,int,int,int> destinationdecide(vector<pair<int,int>> &, vector<pair<int,int>> &, pair<int,int> &, int, vector<Requests> & );
void updatetime(vector<Requests> &, pair<int,int>, pair<int,int>, int &);



int main()
{
    int N,cityrow,citycol;
    int globaltime=0;
    int smin,flag,gsindex,geindex,lsindex,leindex;
    flag=0;


     //Collecting the city layout information
    cout<<"How many rows in the city?"<<endl;
    cin>>cityrow;
    cout<<"How many columns in the city?"<<endl;
    cin>>citycol;

    vector<vector<string>> visualization(cityrow, vector<string>(citycol));
    
    Json::Reader reader;
    Json::Value root;
    fstream reqfile("/home/amey/Desktop/Amey Kulkarni Perceptive Automata internship/requestsfr.json");
    reader.parse(reqfile,root);

    
    cout<<root.type()<<endl;
    N = root["requests"].size();
    vector<Requests> req(N);
    vector<pair<int,int>>allstartp;
    vector<pair<int,int>>endp;
    pair<int,int>currpose;
    pair<int,int>prevpose;
    currpose.first=0;
    currpose.second=0;
    bool complete=false;
    cout<<"Number of intial requests: "<<N<<endl;

    
    //The visualization variable initialization
    for(int i=0;i<(int)visualization.size();i++)
    {
        for(int j=0;j<(int)visualization[0].size();j++)
        {
            visualization[i][j]='.';
            cout<<visualization[i][j]<<"   ";
        }
        cout<<endl;
    }
    
    //Initial pull of all requests from the JSON file to store in the class objects
    for(int i=0;i<(int)req.size();i++)
    {
        req[i].name = root["requests"][i]["name"].asString();
        //cout<<req[i].name<<endl;

        req[i].start.first = root["requests"][i]["start"][0].asInt();
        req[i].start.second = root["requests"][i]["start"][1].asInt();

        req[i].end.first = root["requests"][i]["end"][0].asInt();
        req[i].end.second = root["requests"][i]["end"][1].asInt();

        req[i].time=0;

        //Populate a vector with all the start points required
        allstartp.push_back(req[i].start);

    }
    

    //Loop to increment time and position of the vehicle by 1 unit. This is the major loop which runs the algorithm
    while(!complete) //complete is the variable to find if the whole routing task is completed or not
    {
        
        smin=cityrow+citycol;  //variable to record the least distance from current position to other key points(start or end)

        checkaddreq(root,req,allstartp,globaltime);   // Function call to check if there is any new request added at this point of time
  
        tie(lsindex, leindex, gsindex, geindex, flag)=destinationdecide(allstartp, endp, currpose, smin, req); // Function call to find the immediate destination where the car should head now

        updatetime(req,currpose,prevpose,globaltime);  //Function call to update the travel time of each traveller

        prevpose=currpose;
        
        if(flag==0) // Flag is zero when the immediate destination where the car is hearing is a point to collect some traveller
        {
            if(currpose.first!=allstartp[lsindex].first)    // This is the segment of the code where the position of the car is incremented by 1 unit
            {
            if(allstartp[lsindex].first-currpose.first>0)
                currpose.first=currpose.first+1;
            else if(allstartp[lsindex].first-currpose.first<0)
                currpose.first=currpose.first-1;
            }
            else
            {

            if(allstartp[lsindex].second-currpose.second>0)
                currpose.second=currpose.second+1;
            else if(allstartp[lsindex].second-currpose.second<0)
                currpose.second=currpose.second-1;
            }
            
            
        }
        else if(flag==1)  // Flag is one when the immediate destination where the car is hearing is a point to drop some traveller
        {
            if(currpose.first!=endp[leindex].first)
            {
            if(endp[leindex].first-currpose.first>0)
                currpose.first=currpose.first+1;
            else if(endp[leindex].first-currpose.first<0)
                currpose.first=currpose.first-1;
            }
            else
            {

            if(endp[leindex].second-currpose.second>0)
                currpose.second=currpose.second+1;
            else if(endp[leindex].second-currpose.second<0)
                currpose.second=currpose.second-1;
            }

            
            
        }

           

            if(!allstartp.empty())          //code segment to change the status of the traveller from boarded/dropped. 
            {
            if(allstartp[lsindex]==currpose)
                {
                    
                    req[gsindex].status=true;  //status=true means that the traveller is travelling in the car 
                    allstartp.erase(allstartp.begin()+lsindex);
                    endp.push_back(make_pair(req[gsindex].end.first,req[gsindex].end.second)); //This vector populates the unlocked dropping positions after the traveller has boarded
                    cout<<req[gsindex].name<<" was picked from their location"<<endl;

                }
            }

            if(!endp.empty())
            {
            if(endp[leindex]==currpose)
                {
                
                    req[geindex].status=false;          //status=false means he/she has been dropped or yet to be collected
                    endp.erase(endp.begin()+leindex);
                    cout<<req[geindex].name<<" was dropped off and the total time to destination was: "<<req[geindex].time<<endl;

                }
            }

                   
        cout<<"The current position is: ("<<currpose.first<<","<<currpose.second<<")"<<endl; //displays the current position of the car 

        cout<<"People currently in the car are: "<<endl;        //displays the names of the people currently in the car
        for(int i=0;i<(int)req.size();i++)
        {
            if(req[i].status==true)
                cout<<req[i].name<<", ";
          
        }


        /*globaltime=globaltime+1;        //This variable calculates the total time taken to complete the whole task of collecting and dropping all the people
        cout<<endl<<"The global time is: "<<globaltime<<endl;*/
        flag=0;
        
        cout<<endl;

        for(int i=0;i<(int)allstartp.size();i++)         //This is the visualzation code segment which helps understand the movement of the car. $=car movement, *=startpoints, #=drop points
        {
            visualization[allstartp[i].first][allstartp[i].second]='*';
        }

        for(int i=0;i<(int)endp.size();i++)
        {
            visualization[endp[i].first][endp[i].second]='#';
        }

       for(int i=0;i<(int)visualization.size();i++)
       {
        for(int j=0;j<(int)visualization[0].size();j++)
          {
            visualization[currpose.first][currpose.second]='$';
            cout<<visualization[i][j]<<"   ";
          }
        cout<<endl;
       }

        cout<<"_____________________________________________________________________________________________________________________"<<endl;


        if(allstartp.size()==0 && endp.size()==0)
        {
            complete=true;
            cout<<"Everyone has been dropped! Task Complete!!"<<endl;
        }

    }

    return 0;
}


void checkaddreq(Json::Value &root, vector<Requests> &req, vector<pair<int,int>> &allstartp,int globaltime)        //Function to take in dynamic requests
{
        int newreq,sz,x,objsize;
        string addname;

        
        if(globaltime==20 || globaltime==29)
        {
            cout<<"How many more requests: "<<endl;
            cin>>newreq;
            objsize=req.size();
            req.resize(objsize+newreq);
            sz=root["requests"].size();

            for(int k=sz;k<sz+newreq;k++)
            {
                cin.ignore();
                cout<<"Enter the name: ";
                getline (cin,addname);
                root["requests"][k]["name"]=addname;
                cout<<"Enter the start x: "<<endl;
                cin>>x;
                root["requests"][k]["start"][0]=x;
                cout<<"Enter the start y: "<<endl;
                cin>>x;
                root["requests"][k]["start"][1]=x;
                cout<<"Enter the end x: "<<endl;
                cin>>x;
                root["requests"][k]["end"][0]=x;
                cout<<"Enter the end y: "<<endl;
                cin>>x;
                root["requests"][k]["end"][1]=x;

                //cout<<root["requests"][k]["name"].asString()<<endl;
                req[k].name = root["requests"][k]["name"].asString();
                //cout<<req[k].name<<endl;

                req[k].start.first = root["requests"][k]["start"][0].asInt();
                //cout<<req[k].start.first<<endl;
                req[k].start.second = root["requests"][k]["start"][1].asInt();

                req[k].end.first = root["requests"][k]["end"][0].asInt();
                req[k].end.second = root["requests"][k]["end"][1].asInt();

                
                allstartp.push_back(req[k].start);

                cout<<"Request taken"<<endl;

            }


        }

}

//Function to decide where should the car head to
tuple<int,int,int,int,int> destinationdecide(vector<pair<int,int>> &allstartp, vector<pair<int,int>> &endp, pair<int,int> &currpose, int smin, vector<Requests> &req )

{
    int dist,lsindex,gsindex,leindex,geindex,flag;
    leindex=0;geindex=0;dist=0;

     for(int i=0;i<(int)allstartp.size();i++)
        {
            dist=(abs(currpose.first-allstartp[i].first))+(abs(currpose.second-allstartp[i].second));
            if(dist<smin)
            {
                smin=dist;
                for(int j=0;j<(int)req.size();j++)
                {
                    if ((allstartp[i].first==req[j].start.first)&&(allstartp[i].second==req[j].start.second) && req[j].status==false)
                        gsindex=j;

                }
                lsindex=i;
                flag=0;
                
            }


        }

        for(int i=0;i<(int)endp.size();i++)
        {
            dist=(abs(currpose.first-endp[i].first))+(abs(currpose.second-endp[i].second));
            if(dist<=smin)
            {
                smin=dist;
                for(int j=0;j<(int)req.size();j++)
                {
                    if ((endp[i].first==req[j].end.first)&&(endp[i].second==req[j].end.second) && req[j].status==true)
                        geindex=j;

                }
                leindex=i;
                flag=1;
            }


        }


        return make_tuple(lsindex,leindex,gsindex,geindex,flag);


}

void updatetime(vector<Requests> &req,pair<int,int>currpose, pair<int,int> prevpose, int &globaltime )      //Function to update travel time for each traveller
{
    if(currpose!=prevpose)
    {
    globaltime=globaltime+1;        //This variable calculates the total time taken to complete the whole task of collecting and dropping all the people
    cout<<endl<<"The global time is: "<<globaltime<<endl;
    for(int i=0;i<(int)req.size();i++)
    {
        if(req[i].status==true)
            req[i].time+=1;
    }
    }
}
