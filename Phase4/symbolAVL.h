
// SymbolAVL.h

// Structure for symbols.
struct symbol
{
	char label[7];
	int address;
};

// Structure for nodes.
struct node{
	struct symbol Symbol;
	struct node* left;
	struct node* right;
	
	int height;
};

struct node* newNode(struct symbol);
void insert(struct node**, struct symbol);
void updateHeight(struct node**);
void displayRest(struct node *);
int getHeight(struct node*);
int numLeaves(struct node*);
void Balance(struct node**);
void leftRotation(struct node**);
void rightRotation(struct node**);
void rightLeftRotation(struct node**);
void leftRightRotation(struct node**);
void symbolSymbol();
int max(int, int);
void insertAVL(struct symbol);
void printAVL();
void writeAVL(struct node *,FILE*);
void writeAVLonFile(FILE*);
int countAVL();
int searchSymbol(char[]);
int binaryTreeSearch(struct node*,char[]);
int getValueAVL(char[]);
int getValueOfSymbol(struct node*,char[]);


struct node* root; // Root of the AVLtree
int numElements=0; // Variable for the count of elements on the tree

// Creates a new node.
struct node* newNode(struct symbol x)
{
    struct node* node = (struct node*)malloc(sizeof(struct node));
    strcpy(node->Symbol.label, x.label);
    node->Symbol.address=x.address; // Set the data of the node.
    node->left   = NULL;
    node->right  = NULL;
    node->height = 0;  // new node is initially added at leaf
    return(node);
}

// Recursive routing for inserting
// Insert x into the tree rooted at r
void insert(struct node** r, struct symbol x)
{
	if ((*r)==NULL)
		(*r)=newNode(x); // Add a new node with Symbol x as data.
	else
	{
		if(strcmp(x.label,(*r)->Symbol.label)<0)
			insert(&(*r)->left,x);
		else
			insert(&(*r)->right,x);

		updateHeight(r);
	}

	// Check for inbalances.
	Balance(&(*r));
	return;
}

//Updates the height of a node.
void updateHeight(struct node ** r)
{
	(*r)->height=1+ max(getHeight((*r)->left),getHeight((*r)->right));
}

// Display the information of the nodes in the tree.
void displayRest(struct node * r)
{
	if(r!=NULL)
	{
		//printf("%s %X %d\n", r->Symbol.label, r->Symbol.address, r->height);
		displayRest(r->left);
		printf("%s %X\n", r->Symbol.label, r->Symbol.address);
		//printf("%d\n", r->data);
		displayRest(r->right);
		//printf("%d\n", r->data);
	}
	else
		return;
}

void writeAVL(struct node * r,FILE* fw)
{
	if(r!=NULL)
	{
		fprintf(fw,"%s %X\n", r->Symbol.label, r->Symbol.address);
		writeAVL(r->left, fw);
		writeAVL(r->right,fw);
	}
	else
		return;
}

/* Write all the elements of AVLtree in a file. */
void writeAVLonFile(FILE* fw)
{
	fprintf(fw, "Symbol Table\n\n");
	writeAVL(root,fw);
}

// Binary search on AVLtree.
int binaryTreeSearch(struct node* r,char label[])
{
	//printf("searching\n");
	if (r==NULL)
		return 0;

	if (strcmp(label,r->Symbol.label)<0)
		binaryTreeSearch(r->left, label);	

	else if (strcmp(label,r->Symbol.label)>0)
		binaryTreeSearch(r->right, label);	
	
	else
		return 1;
}

int getValueOfSymbol(struct node* r,char label[])
{
	if (r==NULL)
		return -1;

	if (strcmp(label,r->Symbol.label)<0)
		getValueOfSymbol(r->left, label);	

	else if (strcmp(label,r->Symbol.label)>0)
		getValueOfSymbol(r->right, label);	
	
	else
		return r->Symbol.address;
}

// Get the height of a node.
int getHeight(struct node* r)
{
	if(r==NULL)
		return -1;
	else
		return r->height;
}

// Finds out the numbers of leaves.
int numLeaves(struct node* r)
{
	if(r==NULL)
		return 0;
	if(r->left==NULL&&r->right==NULL)
		return 1;
	else
		return numLeaves(r->left)+numLeaves(r->right);
}

// Check if there are inbalances,
// and if there are inbalances, 
// it will proceed with the correct movements to balance the node.
void Balance(struct node** r)
{
	if(r==NULL)
		return;
	
	if(getHeight((*r)->left)-getHeight((*r)->right)>1) // If the height of the left is higher than right
	{
		if(getHeight((*r)->left->left)>=getHeight((*r)->left->right))
			rightRotation(&(*r));
		else
			leftRightRotation(&(*r));
	}

	else if(getHeight((*r)->right)-getHeight((*r)->left)>1) // If the height of the right is higher than left
	{
		if(getHeight((*r)->right->right)>=getHeight((*r)->right->left))
			leftRotation(&(*r));
		else
			rightLeftRotation(&(*r));
	}
	return;
}

// Performes single left rotation.
void leftRotation(struct node** r)
{
	
	struct node *a =(*r);
	struct node *b=a->right;
	struct node *bl=b->left;
	(*r)=b;
	b->left=a;
	a->right=bl;
	updateHeight(&a);
	updateHeight(&b);
	return;
}

// Performes single right rotation.	
void rightRotation(struct node** r)
{
	struct node *a =(*r);
	struct node *b=a->left;
	struct node *br=b->right;
	(*r)=b;
	b->right=a;
	a->left=br;
	updateHeight(&a);
	updateHeight(&b);
	return;
}

// Performes double left rotation.
void rightLeftRotation(struct node** r)
{
	rightRotation(&(*r)->right);
	leftRotation(&(*r));
	return;
}
	
// Performes double right rotation.
void leftRightRotation(struct node** r)
{
	leftRotation(&(*r)->left);
	rightRotation(&(*r));
	return;
}
	
// Start the AVLtree by setting the root to null.
void symbolAVL()
{
	root=NULL;
}

// Return the higher number of two ints.
int max(int a, int b)
{
    return (a > b)? a : b;
}

// Insert the symbol into AVLtree.
void insertSymbol(struct symbol x)
{
	insert(&root,x);
	numElements++;
}

// Display all elements in AVLtree.
void printAVL()
{
	displayRest(root);
}

// Returns the numbers of elements on AVLtree.
int countAVL()
{
	return numElements;
}

// Return true if label is in AVLtree.
int searchSymbol(char label[])
{
	return binaryTreeSearch(root,label);
}

int getValueAVL (char label[])
{
	return getValueOfSymbol(root,label);
}