#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>
#include <utility>
#include <map>
#include <set>
#include <cstdlib>
#define ll long long
using namespace std;
struct node
{
    int backofftime;
    int duration;
    int numberofbackoff;
};
int curr;
int success=0;
int collision=0;
int blocked=0;
int fac(int n) 
{ 
    return (n==1 || n==0) ? 1: n * fac(n - 1);  
} 
int main()
{
    int n;
    cout<<"Enter total number of nodes : ";
    cin >> n;
    int t;
    cout<<"Enter duration for each node : ";
    cin>>t;
    int z;
    cout<<"Enter range of backoff time for each node 0 - ";
    cin>>z;
    int totalround;
    cout<<"Enter total time for simulation : ";
    cin>>totalround;

    struct node nodes[n];
    bool line = false;
    for (int i = 0; i < n; i++)
    {
        nodes[i].duration = t;
        nodes[i].backofftime = 0;
        nodes[i].numberofbackoff=0;
    }
    for (int _ = 0; _ < totalround; _++)
    {
        cout << "============= TIME t = " << _ << "=============" << endl;
        if (line == false)
        {
            int tempcolcount=0;
            int count = 0;
            int ithnode;
            for (int i = 0; i < n; i++)
            {
                if (nodes[i].backofftime == 0)
                {
                    count++;
                    ithnode = i;
                }
            }
            if (count > 1)
            {
                for (int i = 0; i < n; i++)
                {
                    if (nodes[i].backofftime == 0)
                    {
                        cout << "\033[31mCollision occured for node : \033[0m" << i << endl;
                        nodes[i].backofftime = rand() % z;
                        nodes[i].numberofbackoff++;
                        tempcolcount++;
                        cout << "New backofftime for node " << i << " is " << nodes[i].backofftime << endl;
                    }
                    else
                    {
                        nodes[i].backofftime--;
                    }
                }
            }
            if (count == 1)
            {
                line = true;
                cout << "Node " << ithnode << " is transmitting with remaining time " << nodes[ithnode].duration << endl;
                nodes[ithnode].duration--;
                curr = ithnode;
                for (int i = 0; i < n; i++)
                {
                    if (i != ithnode)
                    {
                        nodes[i].backofftime--;
                    }
                }
            }
            if (count == 0)
            {
                for (int i = 0; i < n; i++)
                {
                    nodes[i].backofftime--;
                }
            }
            if(tempcolcount!=0)
            {
                 collision+=fac(tempcolcount);
            // cout<<"debug print collision up "<<collision<<endl;
            // cout<<tempcolcount<<endl;
            }
           
        }
        else if(line == true)
        {
            if (nodes[curr].duration == 0)
            {
                cout <<"\033[32mnode\033[0m "<< curr << " \033[32mexecuted\033[0m" << endl;success++;
                cout<<"Number of times/time "<<curr<<" had to backoff before successfull transmission "<<nodes[curr].numberofbackoff<<endl;
                nodes[curr].numberofbackoff=0;
                nodes[curr].duration = 5;
                cout << "duration for node " << curr << " reset to " << nodes[curr].duration << endl;
                nodes[curr].backofftime = rand() % z;
                cout << "New backoff time for " << curr << " node is " << nodes[curr].backofftime << endl;
                int count = 0;
                int ithnode;
                for (int i = 0; i < n; i++)
                {
                    if (i != curr)
                    {
                        if (nodes[i].backofftime == 0)
                        {
                            count++;
                            ithnode = i;
                        }
                    }
                }
                if (count > 1)
                {
                    int tempcolcount=0;
                    line = false;
                    for (int i = 0; i < n; i++)
                    {
                        if (i != curr)
                        {
                            if (nodes[i].backofftime == 0)
                            {
                                cout << "\033[31mCollision occured for node : \033[0m" << i << endl;tempcolcount++;
                                nodes[i].numberofbackoff++;
                                nodes[i].backofftime = rand() % z;
                                cout << "New backofftime for node " << i << " is " << nodes[i].backofftime << endl;
                            }
                            else
                            {
                                nodes[i].backofftime--;
                            }
                        }
                    }
                    if(tempcolcount!=0)
                    {
                        collision+=fac(tempcolcount);
                    }
                }
                if (count == 1)
                {
                    line = true;
                    cout << "Node " << ithnode << " is transmitting with remaining time " << nodes[ithnode].duration << endl;
                    nodes[ithnode].duration--;
                    curr = ithnode;
                    for (int i = 0; i < n; i++)
                    {
                        if (i != ithnode)
                        {
                            nodes[i].backofftime--;
                        }
                    }
                }
                if (count == 0)
                {
                    line = false;
                    for (int i = 0; i < n; i++)
                    {
                        if (i != curr)
                            nodes[i].backofftime--;
                    }
                }
            }
            else if(nodes[curr].duration>0)
            {
                cout << "Currently executing node " << curr << " with duration " << nodes[curr].duration << endl;
                nodes[curr].duration--;
                for (int i = 0; i < n; i++)
                {
                    if (i != curr)
                    {
                        if (nodes[i].backofftime == 0)
                        {
                            cout << i << "\033[31m blocked as \033[0m"<<curr<<"\033[31m is executing\033[0m"<<endl;blocked++;
                            nodes[i].numberofbackoff++;
                            nodes[i].backofftime = rand() % z;
                            cout << "New backofftime for node " << i << " is " << nodes[i].backofftime << endl;
                        }
                        else
                        {
                            nodes[i].backofftime--;
                        }
                    }
                }
                
                  
            }
        }
    }
    cout<<"========   STATISTICS   ========"<<endl;
    cout<<"Number of Successfull transmissions : "<<success<<endl;
    cout<<"Number of Collisions : "<<collision<<endl;
    cout<<"Number of times nodes blocked : "<<blocked<<endl;

    return 0;
}