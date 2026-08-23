#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<stdio.h>

#define MAX 256

typedef struct trie{
	uint32_t value;
	struct trie* children[MAX];
	bool isleaf;
}trie;



trie* maketrie(){
	trie* newnode=(trie*)malloc(sizeof(trie));
	for(unsigned int i=0;i<MAX;i++){
		newnode->children[i]=NULL;
	} 
	newnode->isleaf=false;
	return newnode;
}

void freenode(trie* node){
	if(!node){
		return;
	}
	for(unsigned int i=0;i<MAX;i++){
		if(node->children[i]!=NULL){
			freenode(node->children[i]);
		}
		else continue;
	}
	free(node);
}

void insertchar(trie* root,unsigned char ch,uint32_t val){
	uint32_t idx=ch;

	if(root->children[idx]==NULL)
		root->children[idx]=maketrie();

	trie* temp=root->children[idx];
	temp->isleaf=true;
	temp->value=val;
}



bool searchchar(trie* root, unsigned char ch, trie** node) {
	uint32_t idx=ch;

	trie* temp=root->children[idx];
	if(temp && temp->isleaf){
		*node=temp;
		return true;
	}
	return false;
}

