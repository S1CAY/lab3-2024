#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <iomanip>

struct Process {
    int id;
    int arrivalTime;
    int executionTime;
    int priority;
    int startTime = -1;
    int endTime = -1;
    int waitingTime = 0;
};

void generateProcesses(std::vector<Process>& processes, int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> arrivalDist(0, 10);
    std::uniform_int_distribution<> executionDist(1, 10);
    std::uniform_int_distribution<> priorityDist(1, 5);

    for (int i = 0; i < count; ++i) {
        processes.push_back({
            .id = i + 1,
            .arrivalTime = arrivalDist(gen),
            .executionTime = executionDist(gen),
            .priority = priorityDist(gen)
        });
    }
}

void simulateSJF(std::vector<Process>& processes) {
    int currentTime = 0;
    std::vector<Process*> readyQueue;
    int completedProcesses = 0;

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    while (completedProcesses < processes.size()) {
        for (auto& process : processes) {
            if (process.arrivalTime <= currentTime && process.executionTime > 0 &&
                std::find(readyQueue.begin(), readyQueue.end(), &process) == readyQueue.end()) {
                readyQueue.push_back(&process);
            }
        }

        if (!readyQueue.empty()) {
            auto currentProcess = *std::min_element(readyQueue.begin(), readyQueue.end(),
                [](Process* a, Process* b) { return a->executionTime < b->executionTime; });

            readyQueue.erase(std::find(readyQueue.begin(), readyQueue.end(), currentProcess));

            currentProcess->startTime = currentTime;
            currentTime += currentProcess->executionTime;
            currentProcess->executionTime = 0;
            currentProcess->endTime = currentTime;
            currentProcess->waitingTime = currentProcess->startTime - currentProcess->arrivalTime;

            ++completedProcesses;
        } else {
            ++currentTime;
        }
    }
}

void simulateWithPriorityAndAging(std::vector<Process>& processes, int agingThreshold) {
    int currentTime = 0;

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    std::vector<Process*> readyQueue;
    int completedProcesses = 0;

    while (completedProcesses < processes.size()) {
        for (auto& process : processes) {
            if (process.arrivalTime <= currentTime && process.executionTime > 0 &&
                std::find(readyQueue.begin(), readyQueue.end(), &process) == readyQueue.end()) {
                readyQueue.push_back(&process);
            }
        }

        for (auto& process : readyQueue) {
            if (currentTime - process->arrivalTime >= agingThreshold) {
                process->priority = std::max(1, process->priority - 1);
            }
        }

        if (!readyQueue.empty()) {
            auto currentProcess = *std::max_element(readyQueue.begin(), readyQueue.end(),
                [](Process* a, Process* b) { return a->priority < b->priority; });

            readyQueue.erase(std::find(readyQueue.begin(), readyQueue.end(), currentProcess));

            if (currentProcess->startTime == -1) {
                currentProcess->startTime = currentTime;
                currentProcess->waitingTime = currentTime - currentProcess->arrivalTime;
            }

            currentTime += currentProcess->executionTime;
            currentProcess->executionTime = 0;
            currentProcess->endTime = currentTime;

            ++completedProcesses;
        } else {
            ++currentTime;
        }
    }
}

void printProcesses(const std::vector<Process>& processes) {
    std::cout << std::setw(5) << "ID"
              << std::setw(10) << "Arrival"
              << std::setw(10) << "Exec"
              << std::setw(10) << "Priority"
              << std::setw(10) << "Start"
              << std::setw(10) << "End"
              << std::setw(10) << "Wait" << "\n";

    for (const auto& process : processes) {
        std::cout << std::setw(5) << process.id
                  << std::setw(10) << process.arrivalTime
                  << std::setw(10) << (process.executionTime == 0 ? 0 : process.executionTime)
                  << std::setw(10) << process.priority
                  << std::setw(10) << process.startTime
                  << std::setw(10) << process.endTime
                  << std::setw(10) << process.waitingTime << "\n";
    }
}

int main() {
    int processCount = 5;
    int agingThreshold = 5;

    std::vector<Process> processes;
    generateProcesses(processes, processCount);

    std::cout << "Generated Processes:\n";
    printProcesses(processes);

    std::cout << "\nSimulating Shortest Job First (SJF):\n";
    simulateSJF(processes);
    printProcesses(processes);

    generateProcesses(processes, processCount); // Заново генеруємо для нової симуляції
    std::cout << "\nSimulating Priority Scheduling with Aging:\n";
    simulateWithPriorityAndAging(processes, agingThreshold);
    printProcesses(processes);

    return 0;
}
