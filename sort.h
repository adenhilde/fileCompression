

//struct for letter and frequency linked list
typedef struct CharInfo{
	int letter;
	int frequency;
	struct CharInfo* next;
}CharInfo;

//struct for node in tree with the letter, frequency, and left/right children
typedef struct HTnode{
	CharInfo nodeChar;
	struct HTnode* left;
	struct HTNode* right;
}HTnode;
