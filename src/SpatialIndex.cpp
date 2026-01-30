#include "SpatialIndex.hpp"
#include <limits>

// ==================== QUADTREE IMPLEMENTATION ====================

QuadTree::QuadTree(double centerX, double centerY, double width, double height) {
    root = new Node(centerX, centerY, width, height);
}

QuadTree::~QuadTree() {
    deleteTree(root);
}

void QuadTree::deleteTree(Node* node) {
    if (!node) return;
    for (int i = 0; i < 4; i++) deleteTree(node->children[i]);
    delete node;
}

void QuadTree::insert(Location* location) {
    insert(root, location);
}

void QuadTree::insert(Node* node, Location* loc) {
    if (!node) return;
    
    // Check bounds
    double hw = node->width / 2, hh = node->height / 2;
    if (loc->longitude < node->x - hw || loc->longitude > node->x + hw ||
        loc->latitude < node->y - hh || loc->latitude > node->y + hh) return;
    
    // Leaf node - add location or subdivide
    if (!node->children[0]) {
        if (node->locations.size() < MAX_CAPACITY) {
            node->locations.push_back(loc);
            return;
        }
        // Subdivide into 4 quadrants
        node->children[0] = new Node(node->x - hw/2, node->y + hh/2, hw, hh); // NW
        node->children[1] = new Node(node->x + hw/2, node->y + hh/2, hw, hh); // NE
        node->children[2] = new Node(node->x - hw/2, node->y - hh/2, hw, hh); // SW
        node->children[3] = new Node(node->x + hw/2, node->y - hh/2, hw, hh); // SE
        
        // Redistribute locations
        for (auto loc : node->locations) {
            for (int i = 0; i < 4; i++) insert(node->children[i], loc);
        }
        node->locations.clear();
    }
    
    // Insert into children
    for (int i = 0; i < 4; i++) insert(node->children[i], loc);
}

std::vector<Location*> QuadTree::radiusSearch(double centerX, double centerY, double radius) {
    std::vector<Location*> result;
    radiusSearch(root, centerX, centerY, radius, result);
    return result;
}

void QuadTree::radiusSearch(Node* node, double cx, double cy, double r, std::vector<Location*>& result) {
    if (!node) return;
    
    // Check if quadrant intersects circle
    double hw = node->width / 2, hh = node->height / 2;
    double closestX = std::max(node->x - hw, std::min(cx, node->x + hw));
    double closestY = std::max(node->y - hh, std::min(cy, node->y + hh));
    
    if (distance(cx, cy, closestX, closestY) > r) return;
    
    if (!node->children[0]) {
        // Leaf: check each location
        for (auto loc : node->locations) {
            if (distance(cx, cy, loc->longitude, loc->latitude) <= r) {
                result.push_back(loc);
            }
        }
    } else {
        // Recurse to children
        for (int i = 0; i < 4; i++) radiusSearch(node->children[i], cx, cy, r, result);
    }
}

Location* QuadTree::findNearest(double x, double y) {
    double minDist = std::numeric_limits<double>::max();
    return findNearest(root, x, y, minDist, nullptr);
}

Location* QuadTree::findNearest(Node* node, double x, double y, double& minDist, Location* current) {
    if (!node) return current;
    
    if (!node->children[0]) {
        for (auto loc : node->locations) {
            double dist = distance(x, y, loc->longitude, loc->latitude);
            if (dist < minDist) {
                minDist = dist;
                current = loc;
            }
        }
        return current;
    }
    
    for (int i = 0; i < 4; i++) {
        current = findNearest(node->children[i], x, y, minDist, current);
    }
    return current;
}

// ==================== BINARY SEARCH TREE IMPLEMENTATION ====================

BST::BST() : root(nullptr) {}

BST::~BST() {
    deleteTree(root);
}

void BST::deleteTree(Node* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

void BST::insert(const Delivery& delivery) {
    root = insertHelper(root, delivery);
}

BST::Node* BST::insertHelper(Node* node, const Delivery& delivery) {
    if (!node) return new Node(delivery);
    
    if (delivery.deadline < node->delivery.deadline) {
        node->left = insertHelper(node->left, delivery);
    } else {
        node->right = insertHelper(node->right, delivery);
    }
    return node;
}

Delivery* BST::getEarliestDeadline() {
    Node* minNode = findMin(root);
    return minNode ? &minNode->delivery : nullptr;
}

BST::Node* BST::findMin(Node* node) {
    if (!node) return nullptr;
    while (node->left) node = node->left;
    return node;
}

std::vector<Delivery> BST::rangeSearch(const std::string& startDeadline, const std::string& endDeadline) {
    std::vector<Delivery> result;
    collectInRange(root, startDeadline, endDeadline, result);
    return result;
}

void BST::collectInRange(Node* node, const std::string& start, const std::string& end, 
                         std::vector<Delivery>& result) {
    if (!node) return;
    
    if (node->delivery.deadline >= start && node->delivery.deadline <= end) {
        result.push_back(node->delivery);
    }
    collectInRange(node->left, start, end, result);
    collectInRange(node->right, start, end, result);
}

void BST::deleteDelivery(const std::string& deliveryId) {
    root = deleteHelper(root, deliveryId);
}

BST::Node* BST::deleteHelper(Node* node, const std::string& id) {
    if (!node) return nullptr;
    
    if (id < node->delivery.id) {
        node->left = deleteHelper(node->left, id);
    } else if (id > node->delivery.id) {
        node->right = deleteHelper(node->right, id);
    } else {
        // Delete node
        if (!node->left && !node->right) {
            delete node;
            return nullptr;
        }
        if (!node->left) {
            Node* temp = node->right;
            delete node;
            return temp;
        }
        if (!node->right) {
            Node* temp = node->left;
            delete node;
            return temp;
        }
        
        // Two children: replace with successor
        Node* successor = findMin(node->right);
        node->delivery = successor->delivery;
        node->right = deleteHelper(node->right, successor->delivery.id);
    }
    return node;
}

// ==================== SEGMENT TREE IMPLEMENTATION ====================

SegmentTree::SegmentTree(const std::vector<int>& trafficDensities) {
    size = trafficDensities.size();
    tree.resize(4 * size);
    if (size > 0) build(0, 0, size - 1, trafficDensities);
}

void SegmentTree::build(int node, int start, int end, const std::vector<int>& traffic) {
    if (start == end) {
        tree[node] = Node(start, end);
        tree[node].maxTraffic = tree[node].minTraffic = tree[node].sumTraffic = traffic[start];
        return;
    }
    
    int mid = (start + end) / 2;
    int left = 2 * node + 1, right = 2 * node + 2;
    
    build(left, start, mid, traffic);
    build(right, mid + 1, end, traffic);
    
    tree[node].left = start;
    tree[node].right = end;
    tree[node].maxTraffic = std::max(tree[left].maxTraffic, tree[right].maxTraffic);
    tree[node].minTraffic = std::min(tree[left].minTraffic, tree[right].minTraffic);
    tree[node].sumTraffic = tree[left].sumTraffic + tree[right].sumTraffic;
}

int SegmentTree::queryMax(int left, int right) {
    int maxVal = 0, minVal = 0, sumVal = 0;
    query(0, 0, size - 1, left, right, maxVal, minVal, sumVal);
    return maxVal;
}

int SegmentTree::queryMin(int left, int right) {
    int maxVal = 0, minVal = 0, sumVal = 0;
    query(0, 0, size - 1, left, right, maxVal, minVal, sumVal);
    return minVal;
}

double SegmentTree::queryAverage(int left, int right) {
    if (left > right || left < 0 || right >= size) return 0.0;
    int maxVal = 0, minVal = 0, sumVal = 0;
    query(0, 0, size - 1, left, right, maxVal, minVal, sumVal);
    int count = right - left + 1;
    return count > 0 ? (double)sumVal / count : 0.0;
}

void SegmentTree::query(int node, int start, int end, int l, int r, 
                        int& maxVal, int& minVal, int& sumVal) {
    if (l > end || r < start || l > r) return;
    
    if (l <= start && end <= r) {
        maxVal = std::max(maxVal, tree[node].maxTraffic);
        minVal = std::min(minVal, tree[node].minTraffic);
        sumVal += tree[node].sumTraffic;
        return;
    }
    
    int mid = (start + end) / 2;
    query(2 * node + 1, start, mid, l, r, maxVal, minVal, sumVal);
    query(2 * node + 2, mid + 1, end, l, r, maxVal, minVal, sumVal);
}

void SegmentTree::updateTraffic(int roadIdx, int newTraffic) {
    if (roadIdx < 0 || roadIdx >= size) return;
    update(0, 0, size - 1, roadIdx, newTraffic);
}

void SegmentTree::update(int node, int start, int end, int idx, int newTraffic) {
    if (start == end) {
        tree[node].maxTraffic = tree[node].minTraffic = tree[node].sumTraffic = newTraffic;
        return;
    }
    
    int mid = (start + end) / 2;
    if (idx <= mid) {
        update(2 * node + 1, start, mid, idx, newTraffic);
    } else {
        update(2 * node + 2, mid + 1, end, idx, newTraffic);
    }
    
    int left = 2 * node + 1, right = 2 * node + 2;
    tree[node].maxTraffic = std::max(tree[left].maxTraffic, tree[right].maxTraffic);
    tree[node].minTraffic = std::min(tree[left].minTraffic, tree[right].minTraffic);
    tree[node].sumTraffic = tree[left].sumTraffic + tree[right].sumTraffic;
}

