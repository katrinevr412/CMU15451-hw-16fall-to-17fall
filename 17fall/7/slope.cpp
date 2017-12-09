#include <stdio.h>
#include <algorithm>
#include <stdlib.h>
#include <queue>
using namespace std;

struct UpdateRequest {
    //a request to update a node's distance, used in Dijkstra
    int index;
    double distance;
    UpdateRequest(int index, double distance): index(index), distance(distance) {}
};

struct UpdateRequestComparator {
    //a comparator for a min heap on distance
    bool operator() (const UpdateRequest &thisRequest, const UpdateRequest &anotherRequest) {
        return thisRequest.distance > anotherRequest.distance;
    }
};

class SlopeSolver {

private:
    //fields
    int numOfNodes; //total number of nodes
    int numOfKnownNodes; //total number of known nodes
    int lowestKnownNode; //index of smallest known node
    bool isKnownNode[1023]; //whether a node is a node with known heights
    int knownNodeIndex[1023]; //list of known node indexes
    double height[1023]; //heights of nodes. Unknown nodes don't matter
    double curSlope; //current slope
    double upperBound; //slope upper bound, used for binary search. Initially it should be
    //the value when all unknown nodes use the lowest height
    int adjNum[1023]; //adjacent node number for each node, remain unchanged once initialized
    int adjList[1023][1025]; //adjacent lists for each node, remain unchanged once initialized
    double adjWeight[1023][1025]; //adjacent weights for each node; change when curSlope changes

public:
    //constructor, do some basic init
    SlopeSolver() {
        numOfNodes = 0;
        numOfKnownNodes = 0;
        lowestKnownNode = -1;
        curSlope = 1.;
        upperBound = 0.;
        for (int i = 0; i < 1023; i++) {
            adjNum[i] = 0;
        }
    }

    //Initialization. Return number of known nodes.
    int Init() {
        //first do some initialization
        scanf("%d", &numOfNodes);
        int numOfEdges;
        scanf("%d", &numOfEdges);
        double minHeight = 1e9;
        char tmp[31];
        for (int index = 0; index < numOfNodes; index++) {
            scanf("%s", tmp);
            if (tmp[0] == '*') {
                //unknown node
                isKnownNode[index] = false;
            } else {
                //known node
                isKnownNode[index] = true;
                double nodeHeight = (double)(atoi(tmp));
                height[index] = nodeHeight;
                knownNodeIndex[numOfKnownNodes++] = index;
                //update minimum height
                if (nodeHeight < minHeight) {
                    minHeight = nodeHeight;
                    lowestKnownNode = index;
                }
            }
        }
        if (numOfKnownNodes > 1) {
            //if there are at least 2 known nodes, translate all higher known nodes by the offset according to lowest one
            for (int index = 0; index < numOfKnownNodes; index++) {
                height[knownNodeIndex[index]] -= minHeight;
            }
        } else {
            //if there is at most 1 known node, we can return because the answer is always zero
            return numOfKnownNodes;
        }
        //construct edges:
        //for each actual edge, we generate a pair of edges currently use a dummy slope of 1.0
        //additionally, we have an outgoing edge from lowest known node to every other known node
        for (int i = 0; i < numOfEdges; i++) {
            int node1, node2;
            double length;
            scanf("%d%d%lf", &node1, &node2, &length);
            adjList[node1][adjNum[node1]] = node2;
            adjList[node2][adjNum[node2]] = node1;
            adjWeight[node1][adjNum[node1]] = length;
            adjWeight[node2][adjNum[node2]] = length;
            adjNum[node1]++;
            adjNum[node2]++;
            //update upper bound
            if (isKnownNode[node1]) {
                double slopeValue = height[node1] / length;
                if (slopeValue > upperBound) {
                    upperBound = slopeValue;
                }
            } else if (isKnownNode[node2]) {
                double slopeValue = height[node2] / length;
                if (slopeValue > upperBound) {
                    upperBound = slopeValue;
                }
            }
        }
        //now add edges from lowest known node to other known nodes. these values stay the same regardless of the slope
        //there should be numOfKnownNodes - 1 such edges.
        for (int i = 0; i < numOfKnownNodes; i++) {
            int index = knownNodeIndex[i];
            if (index != lowestKnownNode) {
                adjList[lowestKnownNode][adjNum[lowestKnownNode]] = index;
                adjWeight[lowestKnownNode][adjNum[lowestKnownNode]] = height[index];
                adjNum[lowestKnownNode]++;
            }
        }
        return numOfKnownNodes;
    }

    //reset the graph's weights according to a new slope value
    void ResetGraph(double newSlope) {
        double resetRatio = newSlope / curSlope;
        for (int i = 0; i < numOfNodes; i++) {
            if (i != lowestKnownNode) {
                //reset all slope values
                for (int j = 0; j < adjNum[i]; j++) {
                    adjWeight[i][j] *= resetRatio;
                }
            } else {
                //for the lowest known node, only reset the actual connection part
                for (int j = 0; j < adjNum[i] - numOfKnownNodes + 1; j++) {
                    //the last (numOfKnownNodes - 1) edges are for connections to other known nodes
                    adjWeight[i][j] *= resetRatio;
                }
            }
        }
        //update current slope
        curSlope = newSlope;
    }

    //run a Dijkstra to verify current slope
    //return true iff this slope has a solution
    bool RunDijkstraOnCurSlope() {
        double distance[1023]; //the temporary distance
        bool accepted[1023]; //whether a node is accepted
        int numOfAcceptedKnownNodes = 0; //number of accepted known nodes. If this attains numOfKnownNodes, return true
        for (int i = 0; i < numOfNodes; i++) {
            //set distance to infinity, and accepted to false
            distance[i] = 1e9;
            accepted[i] = false;
        }
        //a priority queue holding all update requests
        priority_queue<UpdateRequest, vector<UpdateRequest>, UpdateRequestComparator> updateRequests;
        //first update lowest known node
        updateRequests.push(UpdateRequest(lowestKnownNode, 0.));
        while (!updateRequests.empty()) {
            //pops out the first one
            UpdateRequest updateRequest = updateRequests.top();
            updateRequests.pop();
            int updateNode = updateRequest.index;
            double updateDistance = updateRequest.distance;
            if (accepted[updateNode]) {
                //this node is already accepted, ignore
                continue;
            }
            //accept this node
            accepted[updateNode] = true;
            distance[updateNode] = updateDistance;
            if (isKnownNode[updateNode]) {
                //if it is a known node
                if (updateDistance < height[updateNode]) {
                    //this means a violation! immediately return false
                    return false;
                } else {
                    numOfAcceptedKnownNodes++;
                    if (numOfAcceptedKnownNodes == numOfKnownNodes) {
                        //all known nodes have been accepted, and there is no violation, return true
                        return true;
                    }
                }
            }
            //for all its adjacent un-accepted node, update distance and insert them into the priority queue
            for (int i = 0; i < adjNum[updateNode]; i++) {
                int adjNode = adjList[updateNode][i];
                if (accepted[adjNode]) {
                    continue;
                }
                double newDistance = updateDistance + adjWeight[updateNode][i];
                if (newDistance < distance[adjNode]) {
                    if (isKnownNode[adjNode] && newDistance < height[adjNode]) {
                        //this is also a violation because even a potential shortest path is shorter. return false.
                        return false;
                    }
                    distance[adjNode] = newDistance;
                    updateRequests.push(UpdateRequest(adjNode, newDistance));
                }
            }
        }
        //actually this line should not be executed
        return true;
    }

    //solve
    double Solve() {
        double lower = 0., upper = upperBound;
        double epsilon = 1e-7; //the condition to terminate the search. Possibly adjustment needed
        while (upper - lower > epsilon) {
            double middle = (upper + lower) * .5;
            //reset the graph weights
            ResetGraph(middle);
            if (RunDijkstraOnCurSlope()) {
                //middle passed the test.
                upper = middle;
            } else {
                lower = middle;
            }
        }
        return upper;
    }
};

int main() {
    SlopeSolver *slopeSolver = new SlopeSolver();
    if (slopeSolver -> Init() <= 1) {
        //if there is at most one known node
        printf("slope = 0.000000\n");
    } else {
        //solve it
        printf("slope = %.6lf\n", slopeSolver -> Solve());
    }
    return 0;
}