# k8_resource_allocation

This repository contains an extended implementation of the solution proposed by [James Bremmer](https://github.com/JamesBremner/so79049807) for solving the dual-resource bin packing problem. 
The problem was initially posed by me on [Stack Overflow](https://stackoverflow.com/questions/79049807/genetic-algorithm-for-kubernetes-allocation), and the solution uses the first-fit algorithm for Kubernetes resource allocation challenges.

Overview

* The dual-resource bin packing problem involves assigning tasks that consume two types of resources (e.g., CPU and memory) to bins with finite capacities. 
* The goal is to optimize resource allocation while minimizing wasted capacity. 
* This implementation focuses on Kubernetes, where efficient use of CPU and memory is critical for containerized applications.

Features

    First-Fit Algorithm: Based on James Bremner's proposal, this implementation uses the first-fit heuristic for resource allocation.
    Extended Functionality: Enhancements and optimizations added to improve performance and handle edge cases in Kubernetes environments.
    Dual Resource Handling: Manages two resource constraints simultaneously, ensuring efficient bin packing of both CPU and memory.

Credits

    Original first-fit algorithm proposal by James Bremmer.
    Problem and extension developed by quartermaine.
