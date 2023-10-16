#include <iostream>
#include <algorithm>
#include <string>
#include <cmath>
#include <utility>
#include <map>
#include <queue>
#include <set>

using namespace std;

#define ll long long
#define INF 0x3f3f3f3f
int V;
vector<int> idstore;

class Router
{
    public:
        int id;
        vector<Router *> neighbors;
        map<int, int> routing_table;

        Router()
        {
            this->id = V;
            V++;
        }

        void add_neighbor(Router *neighbor)
        {
            neighbors.push_back(neighbor);
        }

        void print_neighbor()
        {
            for (auto j : neighbors)
                cout << j->id << endl;
        }

        void update_routing_table()
        {
            priority_queue<pair<int, Router *>, vector<pair<int, Router *>>, greater<pair<int, Router *>>>
                pq;
            map<int, int> parentmap;
            vector<int> dist(V, INF);
            pq.push(make_pair(0, this));
            dist[this->id] = 0;
            while (!pq.empty())
            {
                Router *u = pq.top().second;
                pq.pop();
                for (auto j : u->neighbors)
                {
                    Router *v = j;
                    if (dist[v->id] > dist[u->id] + 1)
                    {
                        dist[v->id] = dist[u->id] + 1;
                        parentmap[j->id] = u->id;
                        pq.push(make_pair(dist[v->id], v));
                    }
                }
            }

            for (int i = 0; i < V; i++)
            {
                if (idstore[i] != this->id)
                {
                    int destination = idstore[i];
                    while (parentmap[destination] != this->id)
                    {
                        destination = parentmap[destination];
                    }
                    routing_table[idstore[i]] = destination;
                }
            }
        }

        void print_routing_table()
        {
            cout<<endl;
            cout << "For source node " << this->id << " : " << endl;
            cout<<"\t______________________________________"<<endl;
            cout<<"\t|";
            cout << "    Destination"<< "    |    Next Hop    |" << endl;
            for (int i = 0; i < V; i++)
            {

                if (idstore[i] != this->id)
                {
                    
                    cout << "\t|        " << idstore[i] << "          |        " << routing_table[idstore[i]] <<"       |"<< endl;
                }
            }
            cout<<"\t\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E";
            cout<<"\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E\u203E";
            cout<<"\u203E\u203E\u203E\u203E\u203E\u203E\u203E"<<endl;
        }
};

int main()
{
    V = 0;
    // map<int, Router *> idtorouter;

    Router router[5];

    // for(int i=0;i<V;i++){
    //     idtorouter[i] = &router[i];
    // }

    for(int i=0;i<V;i++){
        idstore.push_back(i);
    }

    router[2].add_neighbor(&router[1]);                 /*                  1                   */
    router[1].add_neighbor(&router[2]);                 /*                 / \                  */
    router[1].add_neighbor(&router[3]);                 /*                /   \                 */
    router[3].add_neighbor(&router[1]);                 /*               /     \                */
    router[3].add_neighbor(&router[2]);                 /*              2-------3               */
    router[2].add_neighbor(&router[3]);                 /*              |      /|               */
    router[3].add_neighbor(&router[4]);                 /*              |     / |               */
    router[4].add_neighbor(&router[3]);                 /*              |    /  |               */
    router[3].add_neighbor(&router[0]);                 /*              |   /   |               */
    router[0].add_neighbor(&router[3]);                 /*              |  /    |               */
    router[2].add_neighbor(&router[4]);                 /*              | /     |               */
    router[4].add_neighbor(&router[2]);                 /*              4       0               */


    for(int i=0;i<V;i++){
        router[i].update_routing_table();
        router[i].print_routing_table();
    }

    int startingnode, endingnode;

    cout << "\033[32mEnter valid starting node id : \033[0m";
    cin >> startingnode;
    cout << endl;

    cout << "Enter valid destination node id : ";
    cin >> endingnode;
    cout << endl;

    cout << "Source to destination path simulation : ";

    int currnode = startingnode;
    while (currnode != endingnode)
    {
        Router *curr_router = &router[currnode];
        cout << curr_router->id << " --> ";
        currnode = curr_router->routing_table[endingnode];
    }
    cout << endingnode << endl;

    return 0;
}