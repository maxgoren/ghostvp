class Node {
    let data;
    let left;
    let right;
}
def insert(let tree, let key) {
    if (tree == nil) {
        tree := new Node;
        tree.data := key;
        return tree;
    }
    if (key < tree.data) {
        tree.left := insert(tree.left, key);
    } else {
        tree.right := insert(tree.right, key);
    }
    return tree;
}
def inorder(let tree) {
    if (tree != nil) {
        inorder(tree.left);
        println tree.data;
        inorder(tree.right);
    }
}
let root := nil;
root := insert(root, 11);
root := insert(root, 34);
root := insert(root, 66);
root := insert(root, 5);
inorder(root);