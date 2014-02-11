/*
* 二叉树查找,使用平衡二叉检索树
*/

template<typename T>
typedef struct _tagBinaryTreeNode
{
	T value;
	BinaryTreeNode *lchild;
	BinaryTreeNode *rchild;
}BinaryTreeNode;

template<typename T>
inline BinaryTreeNode *createTree()
{
	BinaryTreeNode *node = new BinaryTreeNode;
	node->
}

template<typename T>
inline void insertNode(BinaryTreeNode *tree, const T value)
{

}

template<typename T>
inline void deleteNode(BinaryTreeNode *tree, const T value)
{

}

/// @brief 二叉树查找
/// @param[in] list[] 待查找的数组
/// @param[in] length 待查找数组的长度
/// @param[in] keyNum 需要查找的关键词
/// @return 返回查找到的索引值，如果没有找到就返回-1
template<typename T>
inline BinaryTreeNode serachBinaryTree(const BinaryTreeNode *tree, const T keyNum)
{
	assert(length > 0);
	
	return NULL;
}