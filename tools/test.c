#include <stdio.h>
#include <stdlib.h>
#include "commandLineArgs.h"
#include "dirEntry.h"

void testCommandLineArgs (int argc, char** args);
void testDirEntry (int argc, char** args);
void testStringList (void);

int main (int argc, char** args, char** env) {
  testDirEntry (argc, args);

  return 0;
}

void printBoolOptie (char optkey, bool value) {
  printf ("%c: %s\n", optkey, value ? "true" : "false");
}

void printDirectory (const char* dirName) {
  printf ("\nPrinting the contents of directory '%s':\n", dirName);
  dirEntry_t* const entries = dirEntry_readDir (dirName);
  dirEntry_t* entry = entries;
  while (entry != NULL) {
    const char* colour;
    switch (dirEntry_fileType (entry)) {
    case FILE_TYPE_DIRECTORY:
      colour = "\x1B[38;5;19m";
      break;

    case FILE_TYPE_SYMLINK:
      colour = "\x1B[38;5;37m";
      break;

    default:
      colour = "\x1B[0m";
    }
    printf ("  %s%s (%s)\x1B[0m\n", colour, dirEntry_name (entry), dirEntry_extension (entry));
    entry = dirEntry_next (entry);
  }
  dirEntry_delete (entries);
}

void printList (stringList_t* list) {
  const int len = stringList_length (list);
  printf ("\nLengte: %d\n", len);
  int i;
  for (i = 0; i < len; ++i) {
    printf ("  %d: %s\n", i, list->value);
    list = list->next;
  }
}

void printListReverse (stringList_t* list) {
  printf ("\nReverse print.\n");
  while (list != NULL) {
    printf ("  %s\n", list->value);
    list = list->previous;
  }
}

void printStringOptie (char optkey, stringList_t* values) {
  const int len = stringList_length (values);
  printf ("%c: ", optkey);
  if (len > 0) {
    printf ("\x1B[7m%s\x1B[27m\n", values->value);
    int i;
    for (i = 1; i < len; ++i) {
      values = values->next;
      printf ("   \x1B[7m%s\x1B[27m\n", values->value);
    }
  } else {
    printf ("NULL\n");
  }
}

void testCommandLineArgs (int argc, char** args) {
  commandLineArgs_t* commandLineArgs = commandLineArgs_new (argc, args, commandLineArgs_option_newBoolOption ('a'),
                                                                        commandLineArgs_option_newBoolOption ('b'),
                                                                        commandLineArgs_option_newBoolOption ('c'),
                                                                        commandLineArgs_option_newBoolOption ('d'),
                                                                        commandLineArgs_option_newStringOption ('e', "default voor optie e"),
                                                                        commandLineArgs_option_newStringOption ('f', "default voor optie f"),
                                                                        commandLineArgs_option_newStringOption ('g', "default voor optie g"),
                                                                        commandLineArgs_option_newStringOption ('h', NULL),
                                                                        NULL);

  printBoolOptie ('a', commandLineArgs_getBoolOptionValue (commandLineArgs, 'a'));
  printBoolOptie ('b', commandLineArgs_getBoolOptionValue (commandLineArgs, 'b'));
  printBoolOptie ('c', commandLineArgs_getBoolOptionValue (commandLineArgs, 'c'));
  printBoolOptie ('d', commandLineArgs_getBoolOptionValue (commandLineArgs, 'd'));
  printStringOptie ('e', commandLineArgs_getStringOptionValue (commandLineArgs, 'e'));
  printStringOptie ('f', commandLineArgs_getStringOptionValue (commandLineArgs, 'f'));
  printStringOptie ('g', commandLineArgs_getStringOptionValue (commandLineArgs, 'g'));
  printStringOptie ('h', commandLineArgs_getStringOptionValue (commandLineArgs, 'h'));

  stringList_t* mainArgs = commandLineArgs_getMainArgs (commandLineArgs);
  const int len = stringList_length (mainArgs);
  if (len > 0) {
    printf ("Main args: \x1B[7m%s\x1B[27m", mainArgs->value);
    int i;
    for (i = 1; i < len; ++i) {
      mainArgs = mainArgs->next;
      printf (", \x1B[7m%s\x1B[27m", mainArgs->value);
    }
    printf ("\n");
  } else {
    printf ("No main args specified.\n");
  }

  commandLineArgs_delete (commandLineArgs);
}

void testDirEntry (int argc, char** args) {
  commandLineArgs_t* commandLineArgs = commandLineArgs_new (argc, args, NULL);
  stringList_t* mainArgs = commandLineArgs_getMainArgs (commandLineArgs);
  if (stringList_length (mainArgs) == 0) {
    errors_printMessageAndExit ("At least one directory name expected");
  }
  while (stringList_length (mainArgs) > 0) {
    printDirectory (mainArgs->value);
    mainArgs = mainArgs->next;
  }
  commandLineArgs_delete (commandLineArgs);
}

void testStringList (void) {
  stringList_t* list = NULL;
  printList (list);
  list = stringList_append (list, "een");
  stringList_t* listHead = list;
  printList (list);
  list = stringList_append (list, "twee");
  list = stringList_append (list, "drie");
  list = stringList_append (list, "vier");
  printList (list);
  list = stringList_append (listHead, "vijf");
  printList (list);
  printListReverse (list);
  list = stringList_firstElement (list);
  printList (list);

  stringList_delete (list);
}
