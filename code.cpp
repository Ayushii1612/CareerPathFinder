#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <fstream>
#include <stdexcept>
using namespace std;

// Utility Functions
string hashPassword(const string& password) {
    hash<string> hasher;
    return to_string(hasher(password));
}

string getCurrentTimestamp() {
    time_t now = time(0);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

// Forward Declarations (Good practice for larger projects)
class User;
class UserManager;
class Trie;
class AVLTree;
class Graph;
class SegmentTree;
class DoublyLinkedList;
class JobPortal;

// User Authentication System
class User {
public:
    string username;
    string passwordHash;
    vector<string> jobPreferences;
    DoublyLinkedList* applicationHistory;
    
    // Default constructor
    User() : username(""), passwordHash(""), applicationHistory(nullptr) {}
    
    // Parameterized constructor
    User(string uname, string pwdHash) : username(uname), passwordHash(pwdHash), applicationHistory(nullptr) {}
};

// Doubly Linked List for Job Application History
class DoublyLinkedList {
public:
    struct Node {
        string jobTitle;
        string status;
        Node* prev;
        Node* next;
        Node(const string& title, const string& stat) : jobTitle(title), status(stat), prev(nullptr), next(nullptr) {}
    };
    
    Node* head;
    Node* tail;
    stack<Node*> undoStack;
    stack<Node*> redoStack;
    
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}
    
    ~DoublyLinkedList() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        tail = nullptr;
    }
    
    void applyForJob(const string& jobTitle, const string& status = "Applied") {
        Node* newNode = new Node(jobTitle, status);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        undoStack.push(newNode);
        redoStack = stack<Node*>(); // Clear redo stack on new action
    }
    
    void updateStatus(const string& jobTitle, const string& newStatus) {
        Node* curr = head;
        while (curr) {
            if (curr->jobTitle == jobTitle) {
                curr->status = newStatus;
                undoStack.push(curr); // important for undo
                redoStack = stack<Node*>();
                return;
            }
            curr = curr->next;
        }
        throw runtime_error("Job title not found in application history.");
    }
    
    void undo() {
        if (undoStack.empty()) {
            cout << "Nothing to undo!\n";
            return;
        }
        Node* lastNode = undoStack.top();
        undoStack.pop();
        redoStack.push(lastNode); // push to redo
        
        if (lastNode == tail) {
            tail = lastNode->prev;
            if (tail) {
                tail->next = nullptr;
            }
        } else if (lastNode == head) {
            head = lastNode->next;
            if (head) {
                head->prev = nullptr;
            }
        } else {
            Node* prevNode = lastNode->prev;
            Node* nextNode = lastNode->next;
            if (prevNode) {
                prevNode->next = nextNode;
            }
            if (nextNode) {
                nextNode->prev = prevNode;
            }
        }
    }
    
    void redo() {
        if (redoStack.empty()) {
            cout << "Nothing to redo!\n";
            return;
        }
        Node* lastNode = redoStack.top();
        redoStack.pop();
        undoStack.push(lastNode); // push to undo
        
        if (!head) {
            head = tail = lastNode;
        } else {
            Node* prevTail = tail;
            tail->next = lastNode;
            lastNode->prev = prevTail;
            tail = lastNode;
        }
    }
    
    void printHistory() const {
        Node* curr = head;
        while (curr) {
            cout << curr->jobTitle << " (" << curr->status << ") -> ";
            curr = curr->next;
        }
        cout << "NULL\n";
    }
};

class UserManager {
public:
    unordered_map<string, User> users;
    
    void signup(const string& username, const string& password) {
        if (users.find(username) != users.end()) {
            throw runtime_error("Username already exists.");
        }
        string passwordHash = hashPassword(password);
        users[username] = User(username, passwordHash);
        users[username].applicationHistory = new DoublyLinkedList();
    }
    
    bool login(const string& username, const string& password) {
        if (users.find(username) == users.end()) {
            return false;
        }
        string passwordHash = hashPassword(password);
        return users[username].passwordHash == passwordHash;
    }
    
    User& getUser(const string& username) {
        if (users.find(username) == users.end()) {
            throw runtime_error("User not found.");
        }
        return users[username];
    }
};

// Trie for Job Search Autocomplete
class Trie {
public:
    struct Node {
        unordered_map<char, Node*> children;
        bool isEnd;
        
        Node() : isEnd(false) {}
        ~Node() {
            for (auto& child : children) {
                delete child.second;
            }
        }
    };
    
    Node* root;
    
    Trie() {
        root = new Node();
    }
    
    ~Trie() {
        delete root;
    }
    
    void insert(const string& word) {
        Node* curr = root;
        for (char c : word) {
            char lowerC = tolower(c); // Handle case insensitivity
            if (!curr->children.count(lowerC)) {
                curr->children[lowerC] = new Node();
            }
            curr = curr->children[lowerC];
        }
        curr->isEnd = true;
    }
    
    void autocomplete(Node* node, string prefix, vector<string>& suggestions) const {
        if (node->isEnd) {
            suggestions.push_back(prefix);
        }
        for (const auto& child : node->children) {
            autocomplete(child.second, prefix + child.first, suggestions);
        }
    }
    
    vector<string> search(const string& prefix) const {
        vector<string> suggestions;
        Node* curr = root;
        string lowerPrefix = "";
        for (char c : prefix) {
            lowerPrefix += tolower(c);
        }
        for (char c : lowerPrefix) {
            if (!curr->children.count(c)) {
                return suggestions;
            }
            curr = curr->children[c];
        }
        autocomplete(curr, lowerPrefix, suggestions);
        return suggestions;
    }
};

// AVL Tree for Balanced Job Storage
class AVLTree {
public:
    struct Node {
        string jobTitle;
        string company;
        string location;
        string skills;
        int salary;
        int experience;
        bool isRemote;
        Node* left;
        Node* right;
        int height;
        
        Node(const string& title, const string& comp, const string& loc, const string& sk, int sal, int exp, bool remote)
            : jobTitle(title), company(comp), location(loc), skills(sk), salary(sal), experience(exp), isRemote(remote), left(nullptr), right(nullptr), height(1) {}
        
        ~Node() {
            if (left) delete left; // Recursive deletion
            if (right) delete right;
        }
    };
    
    Node* root;
    
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        delete root;
    }
    
    int height(Node* node) const {
        return node ? node->height : 0;
    }
    
    int balanceFactor(Node* node) const {
        return node ? height(node->left) - height(node->right) : 0;
    }
    
    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }
    
    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }
    
    Node* insert(Node* node, const string& jobTitle, const string& company, const string& location, const string& skills, int salary, int experience, bool isRemote) {
        if (!node) return new Node(jobTitle, company, location, skills, salary, experience, isRemote);
        
        if (jobTitle < node->jobTitle)
            node->left = insert(node->left, jobTitle, company, location, skills, salary, experience, isRemote);
        else if (jobTitle > node->jobTitle)
            node->right = insert(node->right, jobTitle, company, location, skills, salary, experience, isRemote);
        else
            return node;
        
        node->height = 1 + max(height(node->left), height(node->right));
        int balance = balanceFactor(node);
        
        if (balance > 1 && jobTitle < node->left->jobTitle)
            return rightRotate(node);
        
        if (balance < -1 && jobTitle > node->right->jobTitle)
            return leftRotate(node);
        
        if (balance > 1 && jobTitle > node->left->jobTitle) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        
        if (balance < -1 && jobTitle < node->right->jobTitle) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        
        return node;
    }
    
    Node* search(Node* node, const string& jobTitle) const {
        if (!node || node->jobTitle == jobTitle)
            return node;
        
        if (jobTitle < node->jobTitle)
            return search(node->left, jobTitle);
        
        return search(node->right, jobTitle);
    }
    
    void inorder(Node* node, vector<Node*>& jobs) const {
        if (!node) return;
        inorder(node->left, jobs);
        jobs.push_back(node);
        inorder(node->right, jobs);
    }
    
    vector<Node*> sortBySalary() const {
        vector<Node*> jobs;
        inorder(root, jobs);
        sort(jobs.begin(), jobs.end(), [](Node* a, Node* b) {
            return a->salary < b->salary;
        });
        return jobs;
    }
    
    vector<Node*> sortByExperience() const {
        vector<Node*> jobs;
        inorder(root, jobs);
        sort(jobs.begin(), jobs.end(), [](Node* a, Node* b) {
            return a->experience < b->experience;
        });
        return jobs;
    }
    
    vector<Node*> searchByLocation(const string& location) const {
        vector<Node*> matchingJobs;
        vector<Node*> allJobs;
        inorder(root, allJobs);
        
        for (Node* job : allJobs) {
            if (job->location == location) {
                matchingJobs.push_back(job);
            }
        }
        return matchingJobs;
    }
    
    void displayAllJobs() const {
        vector<Node*> jobs;
        inorder(root, jobs);
        
        if (jobs.empty()) {
            cout << "No jobs available." << endl;
            return;
        }
        
        for (const auto& job : jobs) {
            cout << "Job Title: " << job->jobTitle << "\n";
            cout << "Company: " << job->company << "\n";
            cout << "Location: " << job->location << "\n";
            cout << "Skills: " << job->skills << "\n";
            cout << "Salary: " << job->salary << "\n";
            cout << "Experience: " << job->experience << " years\n";
            cout << "Remote: " << (job->isRemote ? "Yes" : "No") << "\n\n";
        }
    }
};

// Graph for Career Path Recommendations
class Graph {
public:
    unordered_map<string, vector<pair<string, int>>> adjList;
    
    void addEdge(const string& u, const string& v, int weight) {
        adjList[u].push_back({v, weight});
        adjList[v].push_back({u, weight});
    }
    
    vector<string> bfs(const string& start) const {
        if (adjList.find(start) == adjList.end()) {
            return {}; // Return empty vector if start node not found
        }
        
        vector<string> result;
        unordered_map<string, bool> visited;
        queue<string> q;
        
        q.push(start);
        visited[start] = true;
        
        while (!q.empty()) {
            string node = q.front();
            q.pop();
            result.push_back(node);
            
            for (const auto& neighbor : adjList.at(node)) {
                if (!visited[neighbor.first]) {
                    visited[neighbor.first] = true;
                    q.push(neighbor.first);
                }
            }
        }
        
        return result;
    }
    
    unordered_map<string, int> dijkstra(const string& start) const {
        if (adjList.find(start) == adjList.end()) {
            return {};
        }
        
        unordered_map<string, int> distances;
        for (const auto& node : adjList) {
            distances[node.first] = numeric_limits<int>::max();
        }
        
        distances[start] = 0;
        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
        pq.push({0, start});
        
        while (!pq.empty()) {
            string u = pq.top().second;
            int dist = pq.top().first;
            pq.pop();
            
            if (dist > distances[u]) continue;
            
            for (const auto& neighbor : adjList.at(u)) {
                string v = neighbor.first;
                int weight = neighbor.second;
                
                if (distances[u] + weight < distances[v]) {
                    distances[v] = distances[u] + weight;
                    pq.push({distances[v], v});
                }
            }
        }
        
        return distances;
    }
};

// Segment Tree for Salary Range Queries
class SegmentTree {
public:
    vector<int> tree;
    int n;
    
    SegmentTree(const vector<int>& salaries) : n(salaries.size()) {
        tree.resize(2 * n);
        for (int i = 0; i < n; i++) {
            tree[n + i] = salaries[i];
        }
        for (int i = n - 1; i > 0; i--) {
            tree[i] = tree[2 * i] + tree[2 * i + 1];
        }
    }
    
    int query(int l, int r) const {
        if (l < 0 || r >= n || l > r) {
            throw out_of_range("Invalid range for query.");
        }
        
        l += n;
        r += n;
        int sum = 0;
        
        while (l <= r) {
            if (l % 2 == 1) sum += tree[l++];
            if (r % 2 == 0) sum += tree[r--];
            l /= 2;
            r /= 2;
        }
        
        return sum;
    }
};

// Main Program
class JobPortal {
public:
    UserManager userManager;
    Trie jobTitleTrie;
    AVLTree jobListings;
    Graph careerGraph;
    vector<int> salaries;
    SegmentTree* salarySegmentTree;
    string currentUser; // Track logged-in user
    
    JobPortal() : salaries({50000, 60000, 70000, 80000, 90000, 100000, 120000}), salarySegmentTree(nullptr) {
        salarySegmentTree = new SegmentTree(salaries);
        initializeSampleData();
        currentUser = "";
    }
    
    ~JobPortal() {
        delete salarySegmentTree;
    }
    
    void run() {
        while (true) {
            displayMenu();
            int choice = getUserChoice();
            if (choice == 12) {
                break;
            }
            try {
                handleChoice(choice);
            } catch (const exception& e) {
                cerr << "Error: " << e.what() << endl;
            }
        }
    }

private:
    void displayMenu() const {
        cout << "\nSmart Job Recruitment System\n";
        cout << "1. User Login\n";
        cout << "2. User Signup\n";
        if (currentUser != "") {
            cout << "3. Job Search Autocomplete\n";
            cout << "4. Search Job Details\n";
            cout << "5. Career Path Recommendations\n";
            cout << "6. Salary Range Query (Illustrative)\n";
            cout << "7. Apply for Job\n";
            cout << "8. Undo/Redo Application\n";
            cout << "9. Sort Jobs by Salary\n";
            cout << "10. Sort Jobs by Experience\n";
            cout << "11. Display All Jobs\n";
        }
        cout << "12. Exit\n";
        cout << "Enter your choice: ";
    }
    
    int getUserChoice() const {
        int choice;
        while (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Consume the newline
        return choice;
    }
    
    void handleChoice(int choice) {
        string username, password, prefix, title;
        int l, r, op;
        
        switch (choice) {
            case 1:
                cout << "Enter username: ";
                getline(cin, username);
                cout << "Enter password: ";
                getline(cin, password);
                
                if (userManager.login(username, password)) {
                    cout << "Login successful! Welcome, " << username << "!\n";
                    currentUser = username;
                } else {
                    cout << "Login failed. Incorrect username or password.\n";
                    currentUser = "";
                }
                break;
                
            case 2:
                cout << "Enter username: ";
                getline(cin, username);
                cout << "Enter password: ";
                getline(cin, password);
                
                userManager.signup(username, password);
                cout << "User registered successfully!\n";
                break;
                
            case 3:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                cout << "Enter job title prefix: ";
                getline(cin, prefix);
                displaySuggestions(jobTitleTrie.search(prefix));
                break;
                
            case 4:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                cout << "Enter job title: ";
                getline(cin, title);
                displayJobDetails(jobListings.search(jobListings.root, title));
                break;
                
            case 5:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                cout << "Enter your current job title: ";
                getline(cin, title);
                displayCareerPaths(careerGraph.bfs(title));
                break;
                
            case 6:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                cout << "Enter salary range indices (l r): ";
                cin >> l >> r;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                if (salarySegmentTree) {
                    try {
                        int sum = salarySegmentTree->query(l, r);
                        cout << "Sum of salaries in the specified range: " << sum << endl;
                    } catch (const out_of_range& e) {
                        cerr << "Error: " << e.what() << endl;
                    }
                } else {
                    cout << "Salary segment tree not initialized.\n";
                }
                break;
                
            case 7:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                cout << "Enter job title to apply: ";
                getline(cin, title);
                
                userManager.getUser(currentUser).applicationHistory->applyForJob(title);
                cout << "Applied for " << title << endl;
                break;
                
            case 8:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                cout << "1. Undo\n2. Redo\nEnter choice: ";
                cin >> op;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                
                {  // Create a new block scope to contain the User& reference
                    User& user = userManager.getUser(currentUser);
                    if (op == 1) 
                        user.applicationHistory->undo();
                    else if (op == 2) 
                        user.applicationHistory->redo();
                    
                    cout << "Application History: ";
                    user.applicationHistory->printHistory();
                }  // End of block scope
                break;
                
            case 9:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                displaySortedJobs(jobListings.sortBySalary(), "salary");
                break;
                
            case 10:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                displaySortedJobs(jobListings.sortByExperience(), "experience");
                break;
                
            case 11:
                if (currentUser == "") {
                    cout << "Please log in to use this function" << endl;
                    break;
                }
                
                jobListings.displayAllJobs();
                break;
                
            case 12:
                cout << "Exiting system.\n";
                break;
                
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
    
    void displaySuggestions(const vector<string>& suggestions) const {
        if (suggestions.empty()) {
            cout << "No suggestions found!\n";
        } else {
            cout << "Suggestions:\n";
            for (const string& s : suggestions) {
                cout << "- " << s << endl;
            }
        }
    }
    
    void displayJobDetails(AVLTree::Node* job) const {
        if (job) {
            cout << "\nJob Details:\n";
            cout << "Job Title: " << job->jobTitle << "\n";
            cout << "Company: " << job->company << "\n";
            cout << "Location: " << job->location << "\n";
            cout << "Skills: " << job->skills << "\n";
            cout << "Salary: " << job->salary << "\n";
            cout << "Experience: " << job->experience << " years\n";
            cout << "Remote: " << (job->isRemote ? "Yes" : "No") << "\n";
        } else {
            cout << "Job not found!\n";
        }
    }
    
    void displayCareerPaths(const vector<string>& paths) const {
        if (paths.empty()) {
            cout << "No career path recommendations found for the given job title.\n";
        } else {
            cout << "Career Path Recommendations (BFS):\n";
            for (const string& p : paths) {
                cout << "- " << p << endl;
            }
        }
    }
    
    void displaySortedJobs(const vector<AVLTree::Node*>& jobs, const string& sortBy) const {
        if (jobs.empty()) {
            cout << "No jobs available to display.\n";
        } else {
            cout << "Jobs sorted by " << sortBy << ":\n";
            for (const auto& job : jobs) {
                cout << job->jobTitle << " - Salary: $" << job->salary << ", Experience: " << job->experience << " years\n";
            }
        }
    }
    
    void initializeSampleData() {
        jobTitleTrie.insert("Software Engineer");
        jobTitleTrie.insert("Data Scientist");
        jobTitleTrie.insert("Product Manager");
        jobTitleTrie.insert("Machine Learning Engineer");
        jobTitleTrie.insert("DevOps Engineer");
        
        jobListings.root = jobListings.insert(jobListings.root, "Software Engineer", "Tech Corp", "New York", "C++, Python", 100000, 2, false);
        jobListings.root = jobListings.insert(jobListings.root, "Data Scientist", "Data Inc", "San Francisco", "Python, R", 120000, 3, true);
        jobListings.root = jobListings.insert(jobListings.root, "Product Manager", "Innovate LLC", "Chicago", "Agile, Scrum", 110000, 4, false);
        jobListings.root = jobListings.insert(jobListings.root, "Machine Learning Engineer", "AI Solutions", "Boston", "Python, TensorFlow", 130000, 5, true);
        jobListings.root = jobListings.insert(jobListings.root, "DevOps Engineer", "Cloudify", "Seattle", "AWS, Docker", 90000, 3, true);
        
        careerGraph.addEdge("Software Engineer", "Data Scientist", 1);
        careerGraph.addEdge("Software Engineer", "Product Manager", 2);
        careerGraph.addEdge("Data Scientist", "Machine Learning Engineer", 1);
        careerGraph.addEdge("Product Manager", "DevOps Engineer", 3);
    }
};

int main() {
    JobPortal portal;
    portal.run();
    return 0;
}