AlgoRecruit is a C++-based job recruitment system designed to streamline job hunting through advanced data structures and algorithms. By integrating core DSA concepts, it addresses inefficiencies in traditional job portals, offering a fast, secure, and user-friendly experience. The system uses a Trie for real-time autocomplete suggestions (e.g., "Sof" → "Software Engineer") and an AVL Tree for efficient job storage, enabling quick insertion, retrieval, and sorting by salary or experience. Career guidance is powered by a Graph using BFS (broad career paths) and Dijkstra’s algorithm (shortest skill-based transitions). Users can track applications with a doubly linked list and stack-based undo/redo, while a Segment Tree accelerates salary range queries (e.g., 
80(k–80k–120k). Secure password hashing (std::hash) ensures data safety, and a menu-driven CLI simplifies navigation for sorting, filtering, and analysis. Built with STL libraries, AlgoRecruit bridges theoretical DSA with real-world recruitment challenges, offering scalability for future enhancements.


Key Features
Trie-Based Autocomplete: Instantly suggests job titles as users type.
AVL Tree Storage: Self-balancing tree for fast job management and sorting.
Graph-Powered Career Paths:
BFS for broad role transitions.
Dijkstra’s for shortest skill-based paths.
Undo/Redo Functionality: Doubly linked list + stacks for application history tracking.
Segment Tree Salary Analysis: Compute salary ranges in O(log n) time.
Secure Authentication: Password hashing with std::hash and unordered_map.
CLI Interface: Intuitive menu for sorting (salary/experience) and filtering (location/remote).
