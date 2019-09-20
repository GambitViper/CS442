#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "SymTab.h"

/* Author:    Zachary Baklund
   Created:   2/6/19
   Resources: https://syntaxdb.com/ref/c/ternary
              https://syntaxdb.com/ref/c/pointers
              https://syntaxdb.com/ref/c/structures
              Evan Kirsch
              David Klein
*/

/* The symbol table entry structure proper.
*/
struct SymEntry {
  struct SymTab * table;
  char * name;
  int attrKind;
  void * attributes;
  struct SymEntry * next;
};

/* The symbol table structure proper. The hash table array Contents
   is dynamically allocated according to size
*/
struct SymTab {
  struct SymTab * parent;
  char * scopeName;
  int size;
  int count;
  struct SymEntry ** contents;
};

struct SymTab *
CreateSymTab(int size, char * scopeName, struct SymTab * parentTable) {
  //sanity check to disallow creation of size 0 SymTab
  if(size < 1){
    return NULL;
  }
  //creating space for table allocated and entries hash map
  struct SymTab * table = malloc(sizeof(struct SymTab));
  struct SymEntry ** entries = malloc(size * sizeof(struct SymEntry *));

  table->size = size;
  table->count = 0;
  table->contents = entries;

  //filling the contents area with NULL values to instantiate
  int i = 0;
  while(i < size) {
    table->contents[i] = NULL;
    i++;
  }

  //checking for parentTable if given and assigning to created table
  if(parentTable){
    table->parent = parentTable;
  }else{
    table->parent = NULL;
  }
  //checking for scopeName if given and assigning to created table
  if(scopeName) {
    table->scopeName = strdup(scopeName);
  }else {
    table->scopeName = NULL;
  }

  return table;
}

struct SymTab *
DestroySymTab(struct SymTab *aTable) {
  //checking for aTable if given
  if(aTable == NULL){
    return NULL;
  }
  //save parent symbol table to return
  struct SymTab * parent = aTable->parent;

  int i;
  for(i = 0; i < aTable->size; i++){
    struct SymEntry * entry = aTable->contents[i];
    while(entry){
      //temporary pointer to free entry structs
      struct SymEntry * curr = entry->next;
      free(entry->name);
      free(entry);
      entry = curr;
    }
  }
  //free no longer needed memory
  free(aTable->contents);
  free(aTable->scopeName);
  free(aTable);

  return parent;
}

unsigned int
HashName(int size, const char *name) {
  //sum of character integers mod size to stay within contents range
  int i = 0;
  int sum = 0;
  while(name[i] != '\0'){
    sum = sum + name[i];
    i++;
  }
  return sum % size;
}

struct SymEntry *
FindHashedName(struct SymTab *aTable, int hashValue, const char *name) {
  //start search at initial possible place of hashed name
  struct SymEntry * entry = aTable->contents[hashValue];
  while(entry){
    if(entry->name && strcmp(entry->name, name) == 0){
      return entry;
    }
    entry = entry->next;
  }
  //returns null if the name was not found in this table
  return NULL;
}

struct SymEntry *
LookupName(struct SymTab *aTable, const char * name) {
  //sanity check for disallowing NULL parameters
  if(aTable == NULL || name == NULL){
    return NULL;
  }
  //checking all possible parent tables within lookup
  do {
    int size = aTable->size;
    unsigned int hashValue = HashName(size, name);
    struct SymEntry * lookup = FindHashedName(aTable, hashValue, name);
    if(lookup){
      return lookup;
    }
    aTable = aTable->parent;
  } while(aTable);
  return NULL;
}

struct SymEntry *
EnterName(struct SymTab *aTable, const char *name) {
  //lookup to see if entry already exists within the table
  struct SymEntry * lookup = LookupName(aTable, name);

  //if lookup returns an entry check to see if that entry is in the right table
  if(lookup && lookup->table == aTable){
    return lookup;
  }

  //initialize the values for the new SymEntry
  struct SymEntry * entry = malloc(sizeof(struct SymEntry));
  SetAttr(entry, 0, NULL);
  entry->table = aTable;
  entry->name = strdup(name);

  int size = aTable->size;
  int hashValue = HashName(size, name);

  //placing the new entry in the start of the linked list of the hash map
  entry->next = aTable->contents[hashValue];
  aTable->contents[hashValue] = entry;
  aTable->count = aTable->count + 1;

  return entry;
}

void
SetAttr(struct SymEntry *anEntry, int kind, void *attributes) {
  if(anEntry != NULL){
    anEntry->attrKind = kind;
    anEntry->attributes = attributes;
  }
}

int
GetAttrKind(const struct SymEntry *anEntry) {
  return anEntry ? anEntry->attrKind : -1;
}

void *
GetAttr(const struct SymEntry *anEntry) {
  return anEntry ? anEntry->attributes : NULL;
}

const char *
GetName(const struct SymEntry *anEntry) {
  return anEntry ? anEntry->name : NULL;
}

struct SymTab *
GetTable(const struct SymEntry *anEntry) {
  return anEntry ? anEntry->table : NULL;
}

const char *
GetScopeName(const struct SymTab *aTable) {
  return aTable ? aTable->scopeName : NULL;
}

char *
GetScopeNamePath(const struct SymTab *aTable) {
  if(aTable != NULL){
    if(aTable->parent){
      //recursive case if the current table still has a parent table
      char *scopePath = strdup(GetScopeName(aTable));
      char *parentPath = GetScopeNamePath(aTable->parent);

      //constructs a char * pointer large enough to accept both strings and delimiter
      char *concatPath = malloc(sizeof(char) * (strlen(scopePath) + strlen(parentPath) + 2));

      strcpy(concatPath, parentPath);
      strcat(concatPath, ">");
      strcat(concatPath, scopePath);

      //free no longer needed memory
      free(scopePath);
      free(parentPath);

      return concatPath;
    }else{
      //base case of recursive scopePath
      return strdup(GetScopeName(aTable));
    }
  }
  return NULL;
}

struct SymTab *
GetParentTable(const struct SymTab *aTable) {
  return aTable ? aTable->parent : NULL;
}

struct SymEntry ** entryArray = NULL;
int entryArraySize = 0;

void ProvisionArray(struct SymTab * aTable, bool includeParents) {
  //calculating requested size based on entry counts in table since
  //GetEntries "flattens" the contents array of the table(s)
  int reqSize = 0;
  struct SymTab * curr = aTable;
  do{
    reqSize = reqSize + curr->count;
    curr = curr->parent;
  }while(curr && includeParents);
  reqSize = reqSize + 1;

  // sum table sizes to get requested size
  if (entryArraySize < reqSize) {
    entryArray = realloc(entryArray,reqSize * sizeof(struct SymEntry *));
    entryArraySize = reqSize;
  }

  // initialize the entryArray to NULL
  int i = 0;
  while(i < entryArraySize){
    entryArray[i] = NULL;
    i++;
  }
}

void
CollectEntries(struct SymTab * aTable, bool includeParents, entryTestFunc testFunc) {
  // enumerate table collecting SymEntry pointers, if testFunc provided used to
  // select entries, null terminate the array

  //sanity check if aTable is NULL entries cannot be collected
  if(aTable == NULL){
    return;
  }

  // setup the entryArray sizes
  ProvisionArray(aTable, includeParents);

  int idx = 0;
  struct SymTab * curr = aTable;
  do{
    for(int j = 0; j < curr->size; j++){
      struct SymEntry * entry = curr->contents[j];
      while(entry){
        //testing against a "filter" function if it is present
        //if not present it still adds all the entries
        if(testFunc == NULL || testFunc(entry)){
          entryArray[idx] = entry;
          idx++;
        }
        entry = entry->next;
      }
    }
    curr = curr->parent;
  }while (curr && includeParents);

  return;
}

struct SymEntry **
GetEntries(struct SymTab * aTable, bool includeParents, entryTestFunc testFunc) {
  CollectEntries(aTable, includeParents, testFunc);
  return entryArray;
}

int
GetEntryCount(struct SymEntry* head){
  //helper function to determine length of linked lists in the contents array
  if(head){
    int count = 0;
    struct SymEntry * curr = head;
    while(curr){
      count++;
      curr = curr->next;
    }
    return count;
  }
  return 0;
}

struct Stats *
Statistics(struct SymTab *aTable) {
  //checking for aTable if given
  if(aTable == NULL){
    return NULL;
  }

  //initialize struct Stats
  struct Stats * statistics = malloc(sizeof(struct Stats));
  statistics->minLen = INT_MAX;
  statistics->maxLen = 0;
  statistics->avgLen = 0;
  statistics->entryCnt = 0;

  //iterate hashtable
  int i = 0;
  while(i < aTable->size){
    struct SymEntry * entry = aTable->contents[i];
    int count = GetEntryCount(entry);
    statistics->entryCnt = statistics->entryCnt + count;
    if(count > statistics->maxLen){
      statistics->maxLen = count;
    }
    if(count < statistics->minLen){
      statistics->minLen = count;
    }
    i++;
  }

  statistics->avgLen = statistics->entryCnt / aTable->size;

  return statistics;
}
