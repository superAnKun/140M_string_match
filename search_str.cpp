#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct Node {
	unsigned char val;
	int cnt;
	int flag, ind;
	struct Node *child, *brother;
} Node;

typedef struct StrNode {
	int len, size;
	Node **str_arr;
} StrNode;

Node *getNewNode(unsigned char val, int ind) {
	Node *node = (Node*)malloc(sizeof(Node));
	node->val = val;
	node->cnt = 0;
	node->flag = 0;
	node->ind = ind;
	node->child = NULL;
	node->brother = NULL;
	return node;
}

StrNode* str_arr_init(int size) {
	Node **str_arr = (Node**)malloc(sizeof(Node*) * size);
	StrNode *node = (StrNode*)malloc(sizeof(StrNode));
	node->str_arr = str_arr;
	node->len = 0;
	node->size = size;
	for (int i = 0; i < size; i++) {
		str_arr[i] = NULL;
	}
	str_arr[0] = getNewNode(0, 0);
	return node;
}

Node *insert_brother(Node *brother, Node *innode, Node** temp2, StrNode *node_str) {
	if (brother == NULL) {
		*temp2 = innode;
		return innode;
	}
	if (brother->val < innode->val) {
		brother->brother = insert_brother(brother->brother, innode, temp2, node_str);
	} else if (brother->val > innode->val) {
		innode->brother = brother;
		*temp2 = innode;
		return innode;
	} else {
		*temp2 = brother;
		node_str->len--;
		free(innode);
	}

	return brother;
}

void expand(StrNode *node_arr) {
	Node **temp = (Node**)malloc((size_t)(sizeof(Node*) * node_arr->size * 2));
	memcpy(temp, node_arr->str_arr, sizeof(Node**) * node_arr->size);
	free(node_arr->str_arr);
	node_arr->str_arr = temp;
	node_arr->size *= 2;
}

void insert(unsigned char *str, StrNode *node_arr) {
	Node *temp = node_arr->str_arr[0];
	while (str[0]) {
		if (node_arr->len >= node_arr->size - 1) {
			expand(node_arr);
		}
		Node *innode = node_arr->str_arr[++node_arr->len] = getNewNode(str[0], node_arr->len);
		Node *temp2;
		temp->child = insert_brother(temp->child, innode, &temp2, node_arr);  
		temp = temp2;
		str++;
	}
	temp->flag = 1;
}

Node *final_brother(Node *node, int *count) {
	if (node == NULL) return node;
	while (node->brother) {
		node = node->brother;
		*count += 1;
	}
	*count += 1;
	return node;
}

void swap(int a, int b, StrNode *strnode) {
	Node *temp = strnode->str_arr[a];
	strnode->str_arr[a] = strnode->str_arr[b];
	strnode->str_arr[b] = temp;
	strnode->str_arr[a]->ind = a;
	strnode->str_arr[b]->ind = b;
}

void BFS(StrNode *strnode) {
	Node *head = strnode->str_arr[0], *tail;
	head->brother = head->child;
	tail = head->child;
	int ind = 0, count = 0;
	while (head) {
		tail = final_brother(tail, &count);   //count兄弟节点的数量
		head->cnt = count;
		if (ind != head->ind) swap(ind, head->ind, strnode);
		head = head->brother;
		if (head) {
			tail->brother = head->child;
			count = -1;
		}
		ind++;
	}
}

void findChildIndex(StrNode *strnode) {
	Node *head = strnode->str_arr[0];
	int ind = 0;
	while (head) {
		if (head->child) head->ind = head->child->ind;
		else head->ind = 0;
		head = head->brother;
	}
}

void print(Node *root, StrNode *node_arr) {
	for (int i = 0; i <= node_arr->len; i++) {
	    printf("count: %d ind: %d val: %c\n", node_arr->str_arr[i]->cnt, node_arr->str_arr[i]->ind, node_arr->str_arr[i]->val);
	}
}

void clear(StrNode *node_arr) {
	for (int i = 0; i <= node_arr->len; i++) {
		free(node_arr->str_arr[i]);
	}
	free(node_arr);
}

Node *binary_search(Node **node_arr, unsigned char c_str, Node *temp) {
	int head = temp->ind, tail = temp->ind + temp->cnt - 1;
	while (head <= tail) {
		int mid = (head + tail) >> 1;
		if (node_arr[mid]->val == c_str) {
			return node_arr[mid];
		} else if (node_arr[mid]->val > c_str) {
			tail = mid - 1;
		} else {
			head = mid + 1;
		}
	}
	return NULL;
}

int match_string(unsigned char *str, Node **node_arr) {
	Node *temp = node_arr[0];
	int len = strlen((const char*)str);
	int cnt = 0;
	for (int i = 0; i < len; i++) {
		unsigned char *c_str = str + i;
		temp = node_arr[0];
		while (c_str[0]) {
			temp = binary_search(node_arr, c_str[0], temp);
			if (temp == NULL) {
				//temp = node_arr[0];
				break;
			} else if (temp->flag) {
				cnt++;
			}
			c_str++;
		}
	}
	return cnt;
}


int main() {
	StrNode *node_arr = str_arr_init(10000000);
	unsigned char *arr = (unsigned char*)malloc(sizeof(unsigned char) * 1000000);
	int cnt = 0;
	FILE *fp = fopen("./company.txt", "r");
	
	while (fscanf(fp, "%[^\n\t]\t", arr) != EOF) {
		if (arr[0] >= '0' && arr[0] <= '9') continue;
		insert(arr, node_arr);
	//	printf("insert: %s \n", arr);
		cnt++;
	}

	BFS(node_arr);
	findChildIndex(node_arr);
	printf("插入完毕 cnt = %d\n", cnt);
	fflush(stdout);
	fclose(fp);
	
	cnt = 0;
	fp = fopen("./corpus1.txt", "r");
//	printf("please input:\n");
//	fflush(stdout);
	while (fscanf(fp, "%s", arr) != EOF) {
	    cnt += match_string(arr, node_arr->str_arr);
	}

	printf("匹配成功 cnt : %d \n", cnt);
	free(arr);
	clear(node_arr);
	return 0;
}
