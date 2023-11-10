#include <iostream>
#include <vector>
#include <queue>

using namespace std;
// Define the HttpRequest class
class HttpRequest
{
public:
  int request_id;
  int website_id;
  int processing_time;

  HttpRequest(int id, int website, int time)
      : request_id(id), website_id(website), processing_time(time) {}
};

// Define the Website class
class Website
{
public:
  int website_id;
  int owner_id;
  int bandwidth;
  int processing_power;
  int packet_share;
  bool exists;
  std::queue<HttpRequest> request_queue;

  Website(int id, int owner, int bw, int power,bool exi)
      : website_id(id), owner_id(owner), bandwidth(bw), processing_power(power),exists(exi) {}
};

// Define the LoadBalancer class
class LoadBalancer
{
public:
  vector<Website> websites;
  void add_website(int website_id, int owner_id, int bandwidth, int processing_power, bool exists)
  {
    websites.emplace_back(website_id, owner_id, bandwidth, processing_power, exists);
  }

  void enqueue_request(HttpRequest request)
  {
    for (auto &website : websites)
    {
      if (website.website_id == request.website_id)
      {
        website.request_queue.push(request);
        break;
      }
    }
  }
  bool all_empty()
  {
    for (auto &website : websites)
    {
      if (website.request_queue.size() > 0)
      {
        return false;
      }
    }
    return true;
  }

  void dequeue_request(int current)
  {
      int take_out = min((int)websites[current].request_queue.size(), websites[current].packet_share);
      cout << "Executing " << take_out << " "
           << "request/requests for website with id " << websites[current].website_id << endl;
      while (take_out--)
      {
        HttpRequest curr_req = websites[current].request_queue.front();
        cout << "Executed request with id " << curr_req.request_id << endl;
        websites[current].request_queue.pop();
      }
      if(websites[current].request_queue.size()==0)
      {
        websites[current].exists=false;
      }
  }
};

int main()
{
  LoadBalancer lb;
  int TOTALBANDWIDTH = 0;

  cout << "Enter total number of websites : ";
  int tot_websites;
  cin >> tot_websites;
  for (int i = 0; i < tot_websites; i++)
  {
    cout<<"Enter bandwidth and processing power for website number "<<i+1<<" : ";
    int bw;int pp;cin>>bw>>pp;
    TOTALBANDWIDTH += bw;
    lb.add_website(i+1, i+1, bw, pp, true);

  }
  cout << "Enter total number of http requests : ";
  int tot_req;
  cin >> tot_req;
  for (int i = 0; i < tot_req; i++)
  {
    cout << "Enter Destination id and processing time for request with id " << i + 1<<" : ";
    int did;int pt;cin>>did;cin>>pt;
    for(int j=0;j<pt;j++)
    {
      lb.enqueue_request(HttpRequest(i+1, did, 1));
    }  
  }
  cout << "*******************************************************" << endl;
  while (true)
  {
    int curr_tot_bandwidth = 0;
    if (lb.all_empty())
    {
      cout << "All http requests processed successfully all the websites queue are empty" << endl;
      break;
    }
    for (int i = 0; i < tot_websites; i++)
    {
      if (lb.websites[i].exists == true)
      {
        curr_tot_bandwidth += lb.websites[i].bandwidth;
      }
    }
    for (int i = 0; i < tot_websites; i++)
    {
      if (lb.websites[i].exists == true)
      {
        lb.websites[i].packet_share = ((TOTALBANDWIDTH*lb.websites[i].bandwidth * lb.websites[i].processing_power) / (curr_tot_bandwidth)); 
      }
    }
    for (int i = 0; i < tot_websites; i++)
    {
      if(lb.websites[i].exists==true)
      {
        lb.dequeue_request(i);
      }
    }
  }
  cout << "*******************************************************" << endl;

  return 0;
}
