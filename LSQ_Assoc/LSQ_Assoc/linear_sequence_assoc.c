#include <assert.h>
#include <stdio.h>
#include "linear_sequence_assoc.h"

#define ITR(T) ((TypeIterator*)(T))
#define SEQ(T) ((TypeTree*)(T))

typedef struct Node{
	struct Node* parent;
	struct Node* left;
	struct Node* right;
	LSQ_IntegerIndexT key;
	LSQ_BaseTypeT value;
	int height;
} TypeNode;

typedef struct{
	TypeNode *root;
	int size;
} TypeTree;

typedef enum{
	IS_BEFOR_FIRST,
	IS_PAST_REAR,
	IS_NORMAL
}TypeStateIterator;

typedef struct{
	TypeStateIterator state;
	TypeNode *node;
	TypeTree *tree;
}TypeIterator;

typedef enum{
	INSERT,
	DELETE,
	GET
}TypeAction;


static int maxHeight(int a, int b){
	return(a > b)? a: b;
}

static int GetNodeHeight(TypeNode *node){
	return(node == NULL)? 0: node->height;
}

static int NodeBalanceFactor(TypeNode* node){
	return(GetNodeHeight(node->right)- GetNodeHeight(node->left));
}

static int UpdateNodeHeight(TypeNode *node){
	node->height = 1 + maxHeight(GetNodeHeight(node->left), GetNodeHeight(node->right));
}

static void NodeRotateLeft(TypeTree *tree, TypeNode *node){
	TypeNode *node2 = node->right;
	if(node == NULL)
		return;
	if(node->parent != NULL)
		if(node->parent->left == node)
			node->parent->left = node2;
		else
			node->parent->right = node2;
	else
		tree->root = node2;
	node2->parent = node->parent;
	
	node->right = node2->left;
	if(node2->left != NULL)
		node2->left->parent = node;

	node->parent = node2;
	node2->left = node;

	UpdateNodeHeight(node);
	UpdateNodeHeight(node2);
}

static void NodeRotateRight(TypeTree *tree, TypeNode *node){
	TypeNode *node2 = node->left;

	if(node == NULL)
		return;
	if(node->parent != NULL)
		if(node->parent->left == node)
			node->parent->left = node2;
		else
			node->parent->right = node2;
	else
		tree->root = node2;
	node2->parent = node->parent;

	node->left = node2->right;
	if(node2->right != NULL)
		node2->right->parent = node;

	node->parent = node2;
	node2->right = node;

	UpdateNodeHeight(node);
	UpdateNodeHeight(node2);
}

static void BalanceTree(TypeTree *tree, TypeNode* node, TypeAction stop_criterion){
	TypeNode* pnode = node;
	int balanceFactor = 0;

	while(node != NULL){
		UpdateNodeHeight(node);
		balanceFactor = NodeBalanceFactor(node);
		if((TypeAction)abs(balanceFactor) == stop_criterion)
			return;
		if(balanceFactor == 2){
			if(NodeBalanceFactor(node->right) < 0)
				NodeRotateRight(tree, node->right);
			NodeRotateLeft(tree,node);
		}
		else
			if(balanceFactor == -2){
				if(NodeBalanceFactor(node->left) > 0)
                NodeRotateLeft(tree, node->left);
            NodeRotateRight(tree, node);
			}
		node = node->parent;
	}
}

/*������� ��������� ����, � ��������� �����������, ���������� ��������� �� ����*/
static TypeNode* CreateNode(TypeNode *parant, LSQ_IntegerIndexT key, LSQ_BaseTypeT value){
	TypeNode *node = (TypeNode*)malloc(sizeof(TypeNode));
	
	if(node == NULL)
		return(NULL);
	node->key = key;
	node->left = NULL;
	node->parent = parant;
	node->right = NULL;
	node->value = value;
	node->height = 1;
	return(node);
}
/*������� ��������� ��� ���������� � ��� ����*/
static void FreeTreeNode(TypeNode* node){
	if(node == NULL)
		return;
	FreeTreeNode(node->left);
	FreeTreeNode(node->right);
	free(node);
}
/*�������, ������������ ������ �� ��������� ������� ����*/
static TypeNode* GetNodeByKey(TypeTree* handle, LSQ_IntegerIndexT key, TypeAction operation){
	TypeNode *node = NULL, *pnode = NULL;
	if(handle == NULL)
		return(NULL);

	node = (handle)->root;
	while(node != NULL){
		if(operation == INSERT)
			pnode = node;
		if(key < node->key)
			node = node->left;
		else
			if(key > node->key)
				node = node->right;
			else
				break;
	}

	if(operation == INSERT && node == NULL){
		node = CreateNode(pnode, key, 0);
		handle->size++;
		if(key < pnode->key)
			pnode->left = node;
		else
			pnode->right = node;
	}
	return(node);
}

/*�������, ������������ ��������� �� ����� ����� ����*/
static TypeNode* GetLeftMostNode(TypeNode *root){
	TypeNode *node = root;

	if(root == NULL)
		return(NULL);
	while(node->left != NULL)
		node = node->left;
	return(node);
}

/*�������, ������������ ��������� �� ����� ������ ����*/
static TypeNode* GetRightMostNode(TypeNode *root){
	TypeNode *node = root;

	if(root == NULL)
		return(NULL);
	while(node->right != NULL)
		node = node->right;
	return(node);
}

/* �������, ��������� ������ ���������. ���������� ����������� ��� ���������� */
LSQ_HandleT LSQ_CreateSequence(void){
	TypeTree* tree = (TypeTree*)malloc(sizeof(TypeTree));
	
	if(tree == NULL)
		return(LSQ_HandleInvalid);
	tree->root = NULL;
	tree->size = 0;
	return(tree);
}

/* �������, ������������ ��������� � �������� ������������. ����������� ������������� ��� ������ */
void LSQ_DestroySequence(LSQ_HandleT handle){
	if(handle == NULL)
		return;
	FreeTreeNode(SEQ(handle)->root);
	free(handle);
}

/* �������, ������������ ������� ���������� ��������� � ���������� */
LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
	return(handle == NULL)? 0:(SEQ(handle)->size);
}

/* �������, ������������, ����� �� ������ �������� ���� ����������� */
int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
	return(iterator != NULL && ITR(iterator)->state == IS_NORMAL);
}

/* �������, ������������, ��������� �� ������ �������� �� �������, ��������� �� ��������� � ���������� */
int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
	return(iterator != NULL && ITR(iterator)->state == IS_PAST_REAR);
}

/* �������, ������������, ��������� �� ������ �������� �� �������, �������������� ������� � ���������� */
int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
	return(iterator != NULL && ITR(iterator)->state == IS_BEFOR_FIRST);
}

/* ������� ���������������� ��������. ���������� ��������� �� �������� ��������, �� ������� ��������� ������ �������� */
LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
	return(!LSQ_IsIteratorDereferencable(iterator))? NULL: &(ITR(iterator)->node->value); 
}

/* ������� ���������������� ��������. ���������� ��������� �� ���� ��������, �� ������� ��������� ������ �������� */
LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator){
	return(!LSQ_IsIteratorDereferencable(iterator))? 0: ITR(iterator)->node->key;
}

/* �������, ������������ ��������, ����������� �� ������� � ��������� ������. ���� ������� � ������ ������  *
 * ����������� � ����������, ������ ���� ��������� �������� PastRear.*/
LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
	TypeIterator *iterator = NULL;
	TypeNode *node = NULL;
	
	if(handle == NULL)
		return(NULL);

	node = GetNodeByKey(SEQ(handle), index, GET);
	if(node == NULL)
		return(LSQ_GetPastRearElement(handle));

	iterator = (TypeIterator*)malloc(sizeof(TypeIterator));
	if(iterator == NULL)
		return(NULL);

	iterator->node = node;
	iterator->state = IS_NORMAL;
	iterator->tree = SEQ(handle);
	return(iterator);
}

/* �������, ������������ ��������, ����������� �� ������ ������� ���������� */
LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
	return(handle != NULL && SEQ(handle)->root != NULL)? LSQ_GetElementByIndex(handle, (GetLeftMostNode(SEQ(handle)->root))->key): LSQ_GetPastRearElement(handle);
}

/* �������, ������������ ��������, ����������� �� ��������� �������, ��������� �� ��������� ��������� ���������� */
LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
	TypeIterator *iterator = NULL;

	if(handle == NULL)
		return(NULL);

	iterator = (TypeIterator*)malloc(sizeof(TypeIterator));
	if(iterator == NULL)
		return(NULL);
	
	iterator->node = NULL;
	iterator->state = IS_PAST_REAR;
	iterator->tree = SEQ(handle);
	return(iterator);
}

/* �������, ������������ �������� � �������� ������������ � ������������� ������������� ��� ������ */
void LSQ_DestroyIterator(LSQ_IteratorT iterator){
	if(iterator == NULL)
		return;
	free(iterator);
}

/* �������, ������������ �������� �� ���� ������� ������ */
void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
	TypeNode *node = NULL, *pnode = NULL;

	if(iterator == NULL || ITR(iterator)->tree == NULL || ITR(iterator)->state == IS_PAST_REAR)
		return;

	if(ITR(iterator)->state == IS_BEFOR_FIRST){
		if(LSQ_GetSize(ITR(iterator)->tree) == 0)
			ITR(iterator)->state = IS_PAST_REAR;
		else{
			ITR(iterator)->node = GetLeftMostNode(ITR(iterator)->tree->root);
			ITR(iterator)->state = IS_NORMAL;
		}
		return;
	}

	if(ITR(iterator)->node == NULL)
		return;

	node = ITR(iterator)->node;
	pnode = NULL;

	
	if(node->right != NULL)
		ITR(iterator)->node = GetLeftMostNode(node->right);
	else	
	if(node->parent != NULL){
		if(node->parent->right == node){
			while(node->parent != NULL && node->right == pnode){
				pnode = node;
				node = node->parent;
			}
			if(node->parent == NULL && node->right == pnode){
				ITR(iterator)->node = NULL;
				ITR(iterator)->state = IS_PAST_REAR;
			}
			else
				ITR(iterator)->node = node;
		}
		else
			if(node->parent->left = node)
				ITR(iterator)->node = node->parent;
	}
	else
	{
		ITR(iterator)->node = NULL;
		ITR(iterator)->state = IS_PAST_REAR;
	}
}

/* �������, ������������ �������� �� ���� ������� ����� */
void LSQ_RewindOneElement(LSQ_IteratorT iterator){
	TypeNode *node = NULL, *pnode = NULL;

	if(iterator == NULL || ITR(iterator)->tree == NULL || ITR(iterator)->state == IS_BEFOR_FIRST)
		return;

	if(ITR(iterator)->state == IS_PAST_REAR){
		if(LSQ_GetSize(ITR(iterator)->tree) == 0)
			ITR(iterator)->state = IS_BEFOR_FIRST;
		else{
			ITR(iterator)->node = GetRightMostNode(ITR(iterator)->tree->root);
			ITR(iterator)->state = IS_NORMAL;
		}
		return;
	}

	if(ITR(iterator)->node == NULL)
		return;

	node = ITR(iterator)->node;
	pnode = NULL;

	if(node->left != NULL)
		ITR(iterator)->node = GetRightMostNode(node->left);
	else	
	if(node->parent != NULL){
		if(node->parent->left == node){
			while(node->parent != NULL && node->left == pnode){
				pnode = node;
				node = node->parent;
			}
			if(node->parent == NULL && node->left == pnode){
				ITR(iterator)->node = NULL;
				ITR(iterator)->state = IS_BEFOR_FIRST;
			}
			else
				ITR(iterator)->node = node;
		}
		else
			if(node->parent->right = node)
				ITR(iterator)->node = node->parent;
	}
		else{
			ITR(iterator)->node = NULL;
			ITR(iterator)->state = IS_BEFOR_FIRST;
		}
}

/* �������, ������������ �������� �� �������� �������� �� ������ */
void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift){
	if(iterator == NULL || shift == 0)
		return;
	if(LSQ_GetSize(ITR(iterator)->tree) == 0)
		return;

	if(shift > 0){
		for(shift; shift > 0 && ITR(iterator)->state != IS_PAST_REAR; shift--)
			LSQ_AdvanceOneElement(iterator);
	}
	else{
		for(shift; shift < 0 && ITR(iterator)->state != IS_BEFOR_FIRST; shift++)
			LSQ_RewindOneElement(iterator);
	}
}

/* �������, ��������������� �������� �� ������� � ��������� ������� */
void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
	TypeNode *node = NULL;

	if(iterator == NULL)
		return;
	if(ITR(iterator)->tree == NULL || LSQ_GetSize(ITR(iterator)->tree))
		return;
	node = GetNodeByKey(ITR(iterator)->tree, pos, GET);
	if(node == NULL)
		return;

	ITR(iterator)->node = node;
	ITR(iterator)->state = IS_NORMAL;
}

/* �������, ����������� ����� ���� ����-�������� � ���������. ���� ������� � ������ ������ ����������,  *
 * ��� �������� ����������� ���������.                                                                  */
void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value){
	TypeNode *node = NULL;
	if(handle == NULL)
		return;

	if(LSQ_GetSize(handle) == 0){
		node = CreateNode(NULL,key,value);
		if(node == NULL)
			return;
		node->height = 1;
		SEQ(handle)->root = node;
		SEQ(handle)->size++;
		return;
	}

	node = GetNodeByKey(SEQ(handle), key, INSERT);
	node->value = value;
	
	BalanceTree(SEQ(handle), node, INSERT);
}

/* �������, ��������� ������ ������� ���������� */
void LSQ_DeleteFrontElement(LSQ_HandleT handle){
	if(handle == NULL)
		return;
	LSQ_DeleteElement(handle,  GetLeftMostNode(SEQ(handle)->root)->key);
}

/* �������, ��������� ��������� ������� ���������� */
void LSQ_DeleteRearElement(LSQ_HandleT handle){
	if(handle == NULL)
		return;
	LSQ_DeleteElement(handle, (GetRightMostNode(SEQ(handle)->root)->key));
}

/* �������, ��������� ������� ����������, ����������� �������� ������. */
void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key){
	TypeNode *node = NULL, *pnode = NULL, *parent = NULL;
	if(handle == NULL || LSQ_GetSize(handle) == 0)
		return;
	node = GetNodeByKey(SEQ(handle), key, GET);

	if(node == NULL)
		return;
	parent = node->parent;

	if(node->left != NULL && node->right != NULL){
		pnode = GetRightMostNode(node->left);
		//������� ������ �����
		if(pnode->parent != NULL)
			if(pnode->parent->left == pnode)
				if(pnode->left != NULL){
					pnode->parent->left = pnode->left;
					pnode->left->parent = pnode->parent;
				}
				else
					pnode->parent->left = NULL;
			else
				if(pnode->left != NULL){
					pnode->parent->right = pnode->left;
					pnode->left->parent = pnode->parent;
				}
				else
					pnode->parent->right = NULL;
		else{
			pnode->parent = NULL;
			SEQ(handle)->root = pnode;
		}

		node->key = pnode->key;
		node->value = pnode->value;
		free(pnode);
	}
	else
		if(node->left == NULL && node->right == NULL){
			if(node->parent != NULL)
				if(node->parent->left == node)
					node->parent->left = NULL;
				else
					node->parent->right = NULL;
			else
				SEQ(handle)->root = NULL;
			free(node);
		}
		else
			if(node->left == NULL){
				if(node->parent != NULL){
					if(node->parent->left == node)
						node->parent->left = node->right;
					else
						node->parent->right = node->right;
					node->right->parent = node->parent;
				}
				else{
					node->right->parent = NULL;
					SEQ(handle)->root = node->right;
				}
				free(node);
			}
			else{
				if(node->right == NULL)
					if(node->parent != NULL){
						if(node->parent->right == node)
							node->parent->right = node->left;
						else
							node->parent->left = node->left;
						node->left->parent = node->parent;
					}
					else{
						node->left->parent = NULL;
						SEQ(handle)->root = node->left;
					}
				free(node);
			}
	SEQ(handle)->size--;
	if(parent == NULL)
		BalanceTree(SEQ(handle), SEQ(handle)->root, DELETE);
	else
		BalanceTree(SEQ(handle), parent, DELETE);
}
