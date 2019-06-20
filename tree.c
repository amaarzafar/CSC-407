/*-------------------------------------------------------------------------*
 *---									---*
 *---		tree.c							---*
 *---									---*
 *---	    This file defines struct(s) and functions for counting	---*
 *---	randomly-generated integers in an unbalanced binary tree.	---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"header.h"

//  PURPOSE:  To hold elements of a tree that holds counts of integers.
struct		TreeElement
{
  int			item_;
  int			count_;
  struct TreeElement*	leftPtr_;
  struct TreeElement*	rightPtr_;
};


//  PURPOSE:  To count the occurrence of 'numNums' numbers generated by
//	'getNextNumber()'.  Returns binary tree with counts.
struct TreeElement*
		generateTree	(int	numNums
				)
{
  struct TreeElement*	toReturn	= NULL;
  struct TreeElement*	prev;
  struct TreeElement*	run;
  int	 		i;

  for  (i = 0;  i < numNums;  i++)
  {
    int	nextNum	= getNextNumber();

    for  (prev=NULL, run=toReturn;  run!=NULL; )
    {
      if  (run->item_ == nextNum)
      {
	run->count_++;
        break;
      }

      prev	= run;

      if  (run->item_ > nextNum)
        run = run->leftPtr_;
      else
        run = run->rightPtr_;

    }

    if  (run == NULL)
    {
      struct TreeElement* newItemPtr = (struct TreeElement*)
      	     		  	       malloc(sizeof(struct TreeElement));

      newItemPtr->item_		= nextNum;
      newItemPtr->count_	= 1;
      newItemPtr->leftPtr_	= NULL;
      newItemPtr->rightPtr_	= NULL;

      if  (prev == NULL)
        toReturn	= newItemPtr;
      else
      if  (prev->item_ > nextNum)
	prev->leftPtr_	= newItemPtr;
      else
	prev->rightPtr_	= newItemPtr;
    }

  }

  return(toReturn);
}


//  PURPOSE:  To print the subtree of counts pointed to by 'treePtr'.  No
//	return value.
void		printTree	(struct TreeElement*	treePtr
				)
{
  if  (treePtr != NULL)
  {
    printTree(treePtr->leftPtr_);
    printf("%d: %d time(s)\n",treePtr->item_,treePtr->count_);
    printTree(treePtr->rightPtr_);
  }
}


//  PURPOSE:  To free the subtree pointed to by 'treePtr'.  No return value.
void		freeTree	(struct TreeElement*	treePtr
				)
{
  if  (treePtr != NULL)
  {
    freeTree(treePtr->leftPtr_);
    freeTree(treePtr->rightPtr_);
    free(treePtr);
  }
}


//  PURPOSE:  To count the occurrence of 'numNums' numbers generated by
//	'getNextNumber()' and put the counts in an unbalanced binary tree.
//	Prints this tree, and then free()s it.  No return value.
void		countWithTree	(int	numNums
				)
{
  struct TreeElement*	treePtr	= generateTree(numNums);

  printTree(treePtr);
  freeTree (treePtr);
}	  