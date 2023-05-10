import threading     # Importing the threading module to create threads
import random        # Importing the random module to generate random integers

# Define the TreeNode class
class TreeNode:
    def __init__(self, val):
        self.val = val
        self.left = None
        self.right = None

# Define the BinaryTree class
class BinaryTree:
    def __init__(self):
        self.root = None                          # Initialize the root node to None
        self.lock = threading.Lock()              # Create a lock to synchronize access to the tree
        self.search_thread = threading.Thread(target=self._search_thread_func)  # Create a thread for searching
        self.search_queue = []                     # Initialize the search queue as an empty list
        self.search_results = {}                    # Initialize the search results dictionary as an empty dictionary

    def start_search_thread(self):
        self.search_thread.start()                 # Start the search thread

    def _search_thread_func(self):
        while True:
            if len(self.search_queue) > 0:          # If there are search requests in the queue
                val, request_id = self.search_queue.pop(0)     # Get the first search request
                with self.lock:
                    result = self._search(val)             # Search the tree for the value
                    self.search_results[request_id] = result      # Add the result to the search results dictionary

    def search(self, val):
        request_id = threading.get_ident()          # Get the ID of the current thread
        with self.lock:
            self.search_queue.append((val, request_id))   # Add the search request to the search queue
        while True:
            with self.lock:
                if request_id in self.search_results:
                    result = self.search_results[request_id]       # Get the result from the search results dictionary
                    del self.search_results[request_id]             # Remove the result from the search results dictionary
                    return result

    def _search(self, val):
        curr = self.root                   # Start the search from the root node
        while curr is not None:
            if val == curr.val:
                return True                 # If the value is found, return True
            elif val < curr.val:
                curr = curr.left              # If the value is less than the current node's value, go left
            else:
                curr = curr.right             # If the value is greater than the current node's value, go right
        return False

    def insert(self, val):
        node = TreeNode(val)                # Create a new node with the given value
        if self.root is None:
            self.root = node                # If the tree is empty, make the new node the root
        else:
            curr = self.root                # Start at the root node
            while True:
                if val < curr.val:
                    if curr.left is None:
                        curr.left = node      # If the value is less than the current node's value and the left child is None, insert the new node as the left child
                        break
                    else:
                        curr = curr.left        # If the value is less than the current node's value but the left child is not None, go left
                else:
                    if curr.right is None:
                        curr.right = node        # If the value is greater than or equal to the current node's value and the right child is None, insert the new node as the right child
                        break
                    else:
                        curr = curr.right         # If the value is greater than or equal to the current node's value but the right child is not None, go right

    def preorder_traversal(self, node=None, result=[]):
       


    def preorder_traversal(self, node=None, result=[]):
        if node is None:
            node = self.root
        if node is not None:
            result.append(node.val)
            self.preorder_traversal(node.left, result)
            self.preorder_traversal(node.right, result)
        return result

    def inorder_traversal(self, node=None, result=[]):
        if node is None:
            node = self.root
        if node is not None:
            self.inorder_traversal(node.left, result)
            result.append(node.val)
            self.inorder_traversal(node.right, result)
        return result

    def postorder_traversal(self, node=None, result=[]):
        if node is None:
            node = self.root
        if node is not None:
            self.postorder_traversal(node.left, result)
            self.postorder_traversal(node.right, result)
            result.append(node.val)
        return result

def simulate_insertions(tree, num_insertions):
    for i in range(num_insertions):
        val = random.randint(1, 100)
        tree.insert(val)

def simulate_searches(tree, num_searches):
    for i in range(num_searches):
        val = random.randint(1, 100)
        result = tree.search(val)
        if result:
            print(f"{val} was found in the tree")
        else:
            print(f"{val} was not found in the tree")

# create an instance of BinaryTree
tree = BinaryTree()

# start the search thread
tree.start_search_thread()

# simulate inserting 50 values
simulate_insertions(tree, 50)

# simulate searching for 10 values
simulate_searches(tree, 10)
