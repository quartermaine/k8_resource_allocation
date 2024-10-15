#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iomanip>  // For setting precision in output

class cThing {
public:
    std::string myName;
    int myRes1;  // Remaining CPU after allocation
    int myRes2;  // Remaining RAM after allocation
    int initialCPU;  // Initial CPU capacity
    int initialRAM;  // Initial RAM capacity
    bool myPacked;
    std::vector<std::string> myItems;

    // Add used resource trackers
    int used_myRes1;  // Used CPU in this bin
    int used_myRes2;  // Used RAM in this bin

    cThing(const std::string name)
        : myName(name),
          myPacked(false),
          used_myRes1(0),  // Initialize used resources to zero
          used_myRes2(0)   // Initialize used resources to zero
    {
    }

    std::string text() const {
        std::stringstream ss;
        ss << myName << " CPU: " << myRes1 << " RAM: " << myRes2 << "\n";
        return ss.str();
    }

    void ContentsDisplay();

    bool pack(cThing &item);
};

std::vector<cThing> theItems;
std::vector<cThing> theBins;

void create_pods_and_nodes(int n_pods = 30, int n_nodes = 3) {
    std::srand(std::time(0));  // Correct seeding for random

    std::vector<int> cpu = {2, 4, 8, 16, 32, 64, 128};
    std::vector<int> ram = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};

    for (int i = 0; i < n_pods; i++)
        theItems.emplace_back("pod_" + std::to_string(i + 1));

    for (int i = 0; i < n_nodes; i++)
        theBins.emplace_back("node_" + std::to_string(i + 1));

    for (auto &item : theItems) {
        item.myRes1 = cpu[rand() % 3];   // Random CPU from [2, 4, 8]
        item.myRes2 = ram[rand() % 4];   // Random RAM from [4, 8, 16, 32]
    }

    for (auto &bin : theBins) {
        bin.myRes1 = cpu[rand() % 2 + 4];   // Random CPU from [32, 64]
        bin.myRes2 = ram[rand() % 4 + 4];   // Random RAM from [128, 256, 512, 1024]
        bin.initialCPU = bin.myRes1;  // Store initial CPU capacity
        bin.initialRAM = bin.myRes2;  // Store initial RAM capacity
    }
}

void text() {
    std::cout << "Items (Pods):\n";
    int totalPodCPU = 0;
    int totalPodRAM = 0;

    for (auto &t : theItems) {
        std::cout << t.text();
        totalPodCPU += t.myRes1;  // Sum up CPU for pods
        totalPodRAM += t.myRes2;  // Sum up RAM for pods
    }
    // Print total CPU and RAM for pods
    std::cout << "---- Total CPU of Pods: " << totalPodCPU << ", Total RAM of Pods: " << totalPodRAM << " ----\n";

    std::cout << "\nBins (Nodes):\n";
    int totalNodeCPU = 0;
    int totalNodeRAM = 0;

    for (auto &t : theBins) {
        std::cout << t.text();
        totalNodeCPU += t.myRes1;  // Sum up CPU for nodes
        totalNodeRAM += t.myRes2;  // Sum up RAM for nodes
    }
    // Print total CPU and RAM for nodes
    std::cout << "---- Total CPU of Nodes: " << totalNodeCPU << ", Total RAM of Nodes: " << totalNodeRAM << " ----\n";
}

void cThing::ContentsDisplay() {
    if (myItems.empty()) {
        std::cout << myName << " is empty\n";
        return;
    }
    std::cout << myName << " contains: ";
    for (auto &n : myItems)
        std::cout << n << " ";
    std::cout << "\n";
}

bool cThing::pack(cThing &item) {
    // Deduct the item's resource requirements from the bin's resources
    myRes1 -= item.myRes1;
    myRes2 -= item.myRes2;

    // Update used resources
    used_myRes1 += item.myRes1;  // Increment used CPU
    used_myRes2 += item.myRes2;  // Increment used RAM

    item.myPacked = true;
    myItems.push_back(item.myName);

    return true;
}

void pack() {
    // Sort items in decreasing order of total resource requirements
    std::sort(
        theItems.begin(), theItems.end(),
        [](const cThing &a, const cThing &b) -> bool
        {
            return (a.myRes1 + a.myRes2) > (b.myRes1 + b.myRes2);
        });

    // Try to fit each item into the first bin that can accommodate it
    for (cThing &item : theItems) {
        bool packed = false;  // Track if the item is packed
        for (cThing &bin : theBins) {
            // Check if the item can fit into the current bin
            if (item.myRes1 + bin.used_myRes1 <= bin.initialCPU &&
                item.myRes2 + bin.used_myRes2 <= bin.initialRAM)
            {
                // Pack the item into the bin
                bin.pack(item);
                packed = true;
                break; // Stop trying to fit into other bins
            }
        }

        // If not packed, log more information about the item and its requirements
        if (!packed) {
            std::cout << "Item " << item.myName << " could not be packed. "
                      << "Requires CPU: " << item.myRes1 
                      << ", RAM: " << item.myRes2 << "\n";
        }
    }
}

void packDisplay() {
    int unpackedCount = 0;
    std::cout << "\nUnpacked Items (Pods):\n";
    for (auto &item : theItems) {
        if (!item.myPacked) {
            std::cout << item.myName << " ";
            unpackedCount++;
        }
    }
    std::cout << "\n";
    if (!unpackedCount)
        std::cout << "All items packed\n";
    else
        std::cout << unpackedCount << " items did not fit\n";

    std::cout << "\nPacked Bins (Nodes) Contents:\n";
    for (auto &bin : theBins)
        bin.ContentsDisplay();
}

struct NodeUtilization {
    std::string node;
    int total_cpu_used;
    int total_ram_used;
    int node_cpu;  // Initial node CPU capacity
    int node_ram;  // Initial node RAM capacity
    float cpu_utilization;
    float ram_utilization;
};

std::vector<NodeUtilization> node_utilization_data;

void calculate_utilization() {
    for (auto& bin : theBins) {
        int total_cpu = 0;
        int total_ram = 0;

        for (auto& pod_name : bin.myItems) {
            for (auto& item : theItems) {
                if (item.myName == pod_name) {
                    total_cpu += item.myRes1;
                    total_ram += item.myRes2;
                }
            }
        }

        // Calculate CPU and RAM utilization using the initial CPU and RAM capacity
        float cpu_util = (bin.initialCPU != 0) ? (float)total_cpu / bin.initialCPU * 100 : 0;
        float ram_util = (bin.initialRAM != 0) ? (float)total_ram / bin.initialRAM * 100 : 0;

        node_utilization_data.push_back({bin.myName, total_cpu, total_ram, bin.initialCPU, bin.initialRAM, cpu_util, ram_util});
    }
}

void print_node_utilization(std::vector<NodeUtilization>& node_data) {
    // Sort the node data by node names
    std::sort(node_data.begin(), node_data.end(), [](const NodeUtilization &a, const NodeUtilization &b) {
        return a.node < b.node; // Sort by node name
    });

    std::cout << "\nTotal CPU and RAM utilization for each node:\n";
    std::cout << std::setw(10) << "Node" 
              << std::setw(15) << "Total_CPU_Used" 
              << std::setw(15) << "Total_RAM_Used" 
              << std::setw(10) << "Node_CPU" 
              << std::setw(10) << "Node_RAM" 
              << std::setw(15) << "CPU_Utilization"
              << std::setw(15) << "RAM_Utilization" << "\n";

    for (const auto& node : node_data) {
        std::cout << std::setw(10) << node.node
                  << std::setw(15) << node.total_cpu_used
                  << std::setw(15) << node.total_ram_used
                  << std::setw(10) << node.node_cpu
                  << std::setw(10) << node.node_ram
                  << std::setw(15) << std::fixed << std::setprecision(3) << node.cpu_utilization
                  << std::setw(15) << std::fixed << std::setprecision(3) << node.ram_utilization << "\n";
    }
}

int main() {
    create_pods_and_nodes(36, 4);
    text();
    pack();
    packDisplay();
    calculate_utilization();
    print_node_utilization(node_utilization_data);

    return 0;
}



