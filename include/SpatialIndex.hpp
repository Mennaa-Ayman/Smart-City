/* ========================================================================
Spatial Indexing (Trees) 
1. QuadTree: Geographic partitioning for nearest-neighbor searches 
2. BST: Sorted delivery deadlines for priority-based assignment 
3. SegmentTree: Range queries for traffic density analysis
======================================================================== */

#ifndef SPATIALINDEX_HPP
#define SPATIALINDEX_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include "Utils.hpp"

// ==================== QUADTREE CLASS ====================
// Partitions city space into 4 quadrants for efficient spatial queries
class QuadTree {
private:
    struct Node {
        double x, y, width, height;                    // quadrant bounds
        std::vector<Location*> locations;              // locations in this node
        Node* children[4];                             // NW, NE, SW, SE
        Node(double cx, double cy, double w, double h) 
            : x(cx), y(cy), width(w), height(h) { 
            for (int i = 0; i < 4; i++) children[i] = nullptr; 
        }
    };
    
    Node* root;
    static const int MAX_CAPACITY = 4;
    
    // Helper functions for recursive operations
    void insert(Node* node, Location* loc);
    void radiusSearch(Node* node, double cx, double cy, double r, std::vector<Location*>& result);
    Location* findNearest(Node* node, double x, double y, double& minDist, Location* current);
    void deleteTree(Node* node);
    
    // Distance calculation between two geographic points
    double distance(double x1, double y1, double x2, double y2) const {
        double dx = x2 - x1, dy = y2 - y1;
        return std::sqrt(dx * dx + dy * dy);
    }
    
public:
    QuadTree(double centerX, double centerY, double width, double height);
    ~QuadTree();
    
    void insert(Location* location);
    std::vector<Location*> radiusSearch(double centerX, double centerY, double radius);
    Location* findNearest(double x, double y);
};

// ==================== BINARY SEARCH TREE CLASS ====================
// Maintains deliveries sorted by deadline for priority-based scheduling
class BST {
private:
    struct Node {
        Delivery delivery;    // stores complete delivery data
        Node* left;
        Node* right;
        Node(const Delivery& d) : delivery(d), left(nullptr), right(nullptr) {}
    };
    
    Node* root;
    
    // Helper functions for recursive operations
    Node* insertHelper(Node* node, const Delivery& delivery);
    Node* deleteHelper(Node* node, const std::string& id);
    Node* findMin(Node* node);
    void collectInRange(Node* node, const std::string& start, const std::string& end, 
                       std::vector<Delivery>& result);
    void deleteTree(Node* node);
    
public:
    BST();
    ~BST();
    
    void insert(const Delivery& delivery);
    Delivery* getEarliestDeadline();
    std::vector<Delivery> rangeSearch(const std::string& startDeadline, const std::string& endDeadline);
    void deleteDelivery(const std::string& deliveryId);
};

// ==================== SEGMENT TREE CLASS ====================
// Efficiently queries traffic statistics across road ranges
class SegmentTree {
private:
    struct Node {
        int left, right;
        int maxTraffic, minTraffic, sumTraffic;
        Node(int l = 0, int r = 0) 
            : left(l), right(r), maxTraffic(0), minTraffic(0), sumTraffic(0) {}
    };
    
    std::vector<Node> tree;
    int size;
    
    void build(int node, int start, int end, const std::vector<int>& traffic);
    void query(int node, int start, int end, int l, int r, int& maxVal, int& minVal, int& sumVal);
    void update(int node, int start, int end, int idx, int newTraffic);
    
public:
    SegmentTree(const std::vector<int>& trafficDensities);
    
    int queryMax(int left, int right);
    int queryMin(int left, int right);
    double queryAverage(int left, int right);
    void updateTraffic(int roadIdx, int newTraffic);
};

#endif // SPATIALINDEX_HPP