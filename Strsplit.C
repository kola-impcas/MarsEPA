#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int SplitString(char *pszSrc,char *pszSplit,char *pszDest){
  
  int iPos;
  char *pszTmp;

  pszTmp = strstr(pszSrc,pszSplit);
  iPos = strlen(pszSrc)-strlen(pszTmp);
  strncpy(pszDest,pszSrc,iPos);
  pszDest[iPos] = '\0';
  printf("%s\n",pszDest);

  return 0;
  
}
