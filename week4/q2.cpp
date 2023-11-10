#include <iostream>
#include <queue>
#include <random>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <iomanip>
#include <algorithm>

using namespace std;
const int SimulationTimeInMinutes = 1000;

// Function to generate random exponential times
double generateTime(double lambda) {
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<double> exponential(lambda);
    return exponential(gen);
}

// Function for passenger arrival and queuing
void passengerArrivalAndQueuing(double lambda, double mu, int K, vector<queue<double>>& q,
                                 vector<int>& totalPassengers, vector<mutex>& passengersMutex, mutex& outputMutex) {
    double currentTime = 0.0;
    int i = 0;
    int S = q.size();
    while (currentTime < SimulationTimeInMinutes) {
        int idx = rand() % S;

        if (q[idx].size() < K) {
            double interArrivalTime = generateTime(lambda);
            i++;

            currentTime += interArrivalTime;

            passengersMutex[idx].lock();
            q[idx].push(currentTime);
            totalPassengers[idx]++;
            passengersMutex[idx].unlock();
        }
    }

    for (int i = 0; i < S; i++) {
        passengersMutex[i].lock();
        q[i].push(currentTime);
        passengersMutex[i].unlock();
    }
}

// Function for passenger servicing
void passengerServicing(int scanner, double mu, vector<queue<double>>& queue,
                        vector<double>& totalTimeInSystem, vector<double>& totalTimeWaitingInQueue,
                        vector<mutex>& passengersMutex, mutex& outputMutex, vector<mutex>& timeMutex,
                        vector<double>& totalServiceTime) {
    double currentTime = 0;
    int i = 1;
    while (true) {
        passengersMutex[scanner].lock();
        if (!queue[scanner].empty()) {
            double arrivalTime = queue[scanner].front();
            if (arrivalTime > SimulationTimeInMinutes) {
                passengersMutex[scanner].unlock();
                break;
            }

            queue[scanner].pop();
            passengersMutex[scanner].unlock();

            if (arrivalTime > currentTime) {
                currentTime = arrivalTime;
            }
            double waitingTime = currentTime - arrivalTime;

            double serviceTime = generateTime(mu);

            currentTime += serviceTime;
            if (currentTime > SimulationTimeInMinutes) break;

            timeMutex[scanner].lock();
            totalTimeWaitingInQueue[scanner] += waitingTime;
            totalTimeInSystem[scanner] += currentTime - arrivalTime;
            totalServiceTime[scanner] += serviceTime;
            timeMutex[scanner].unlock();

            outputMutex.lock();
            cout << "[" << scanner << "]" << left << "Packet " << setw(6) << i << " :: "
                 << "ArrivalTime = " << setw(13) << arrivalTime << " :: "
                 << "ServiceTime = " << setw(13) << serviceTime << " :: "
                 << "WaitingTime = " << setw(13) << waitingTime << " :: "
                 << "CurrentTime = " << setw(13) << currentTime << endl;
            outputMutex.unlock();
            i++;
        } else {
            passengersMutex[scanner].unlock();
        }
    }
}

int main() {
    double lambda, mu;
    int K, S;
    cout << "Enter arrival rate (lambda): ";
    cin >> lambda;
    cout << "Enter service rate (mu): ";
    cin >> mu;
    cout << "Enter buffer size (K): ";
    cin >> K;
    cout << "Enter number of scanners (S): ";
    cin >> S;

    default_random_engine arrival(time(NULL));
    default_random_engine service(time(NULL));

    exponential_distribution<double> arr(lambda);
    exponential_distribution<double> ser(mu);

    vector<queue<double>> q(S);
    vector<double> totalTimeInSystem(S, 0.0);
    vector<double> totalTimeWaitingInQueue(S, 0.0);
    vector<double> totalServiceTime(S, 0.0);
    vector<int> totalPassengers(S, 0);
    vector<mutex> passengersMutex(S);
    mutex outputMutex;
    vector<mutex> timeMutex(S);

    thread arrivalThread(passengerArrivalAndQueuing, lambda, mu, K, ref(q), ref(totalPassengers),
                         ref(passengersMutex), ref(outputMutex));

    vector<thread> temp;
    for (int i = 0; i < S; i++) {
        temp.emplace_back(passengerServicing, i, mu, ref(q), ref(totalTimeInSystem),
                          ref(totalTimeWaitingInQueue), ref(passengersMutex), ref(outputMutex),
                          ref(timeMutex), ref(totalServiceTime));
    }

    arrivalThread.join();
    for (auto& thread : temp) {
        thread.join();
    }
    double total_wait_time = 0.0;
    double total_system_util = 0.0;
    double tot_queu_len = 0.0;

    for (int i = 0; i < S; i++) { 
        cout << endl;
        cout << " Scanner :" << i << endl;
        lock_guard<mutex> lock1(outputMutex);
        lock_guard<mutex> lock2(timeMutex[i]);
        cout << "Total passengers: " << totalPassengers[i] << endl;
        cout << "Average System Time: " << totalTimeInSystem[i] / max(1, totalPassengers[i] - 1) << " minutes" << endl;
        cout << "Average Waiting Time: " << totalTimeWaitingInQueue[i] / max(1, totalPassengers[i] - 1) << " minutes" << endl;
        cout << "Average Queue Length: " << totalTimeInSystem[i] / SimulationTimeInMinutes << " passengers/minute" << endl;
        cout << "System Utilization: " << (totalServiceTime[i] / SimulationTimeInMinutes) * 100 << "%" << endl;
        total_wait_time=total_wait_time+totalTimeWaitingInQueue[i] / max(1, totalPassengers[i] - 1);
        total_system_util=total_system_util+(totalServiceTime[i] / SimulationTimeInMinutes) * 100;
        tot_queu_len=tot_queu_len+totalTimeInSystem[i] / SimulationTimeInMinutes;
    }

        cout<<"total average wait time : "<<(total_wait_time/S)*60.00<<endl;
        cout<<"total system utilization : "<<total_system_util/S<<endl;
        cout<<"total average queue length : "<<tot_queu_len/S<<endl;



    return 0;
}