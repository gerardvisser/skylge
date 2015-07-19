#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commandLineArgs.h"
#include "file.h"
#include "fileInfo.h"
#include "stringBuilder.h"

void testCommandLineArgs (int argc, char** args);
void testFile (int argc, char** args);
void testFileInfo (int argc, char** args);
void testFileSublist (void);
void testStringBuilder (void);
void testStringList (void);

int main (int argc, char** args, char** env) {
  testFile (argc, args);

  return 0;
}

void printBoolOptie (char optkey, bool value) {
  printf ("%c: %s\n", optkey, value ? "true" : "false");
}

void printDate (time_t time) {
  struct tm cal;
  localtime_r (&time, &cal);
  printf ("%d-%02d-%02d %02d:%02d:%02d", cal.tm_year + 1900,
                                         cal.tm_mon + 1,
                                         cal.tm_mday,
                                         cal.tm_hour,
                                         cal.tm_min,
                                         cal.tm_sec);
}

/* Deletes the list after printing. */
void printFileList (file_t* const entries) {
  file_t* entry = entries;
  struct tm cal;
  while (entry != NULL) {
    const char* colour;
    switch (file_type (entry)) {
    case FILE_TYPE_DIRECTORY:
      colour = "\x1B[38;5;19m";
      break;

    case FILE_TYPE_SYMLINK:
      colour = "\x1B[38;5;37m";
      break;

    default:
      colour = "\x1B[0m";
    }
    printf ("  %s(", colour);
    printDate (file_modificationTime (entry));
    printf (") %s (%s) (%s)\x1B[0m\n", file_name (entry), file_extension (entry), file_fullName (entry));
    entry = file_next (entry);
  }
  file_delete (entries);
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
  --argc;
  ++args;
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

void testFile (int argc, char** args) {
  commandLineArgs_t* commandLineArgs = commandLineArgs_new (argc - 1, args + 1, NULL);
  stringList_t* mainArgs = commandLineArgs_getMainArgs (commandLineArgs);
  if (stringList_length (mainArgs) == 0) {
    errors_printMessageAndExit ("At least one filename expected");
  }
  while (stringList_length (mainArgs) > 0) {
    printf ("\nPrinting list for file or directory '%s':\n", mainArgs->value);
    printFileList (file_new (mainArgs->value));
    mainArgs = mainArgs->next;
  }
  commandLineArgs_delete (commandLineArgs);
}

void testFileInfo (int argc, char** args) {
  int i;
  fileInfo_t info;
  for (i = 1; i < argc; ++i) {
    printf ("%s: ", args[i]);
    if (!fileInfo_read (&info, args[i])) {
      printf ("\x1B[7msize:\x1B[27m %ld byte(s), \x1B[7mlast modification:\x1B[27m ", info.size);
      printDate (info.modificationTime);
      printf (", \x1B[7mtype:\x1B[27m ");
      switch (info.fileType) {
      case FILE_TYPE_DIRECTORY:
        printf ("directory\n");
        break;

      case FILE_TYPE_REGULAR:
        printf ("regular\n");
        break;

      case FILE_TYPE_SYMLINK:
        printf ("symbolic link\n");
        break;

      default:
        printf ("unknown\n");
        break;
      }
    } else {
      printf ("file does not exist\n");
    }
  }
}

void testFileSublist (void) {
  file_t* const wd = file_new (".");
  file_t* cfiles = NULL;
  file_t* file = wd;
  while (file != NULL) {
    if (strcmp (file_extension (file), "c") == 0) {
      cfiles = file_append (cfiles, file);
    }
    file = file_next (file);
  }
  file_delete (wd);

  cfiles = file_firstEntry (cfiles);
  printf ("\nPrinting list of c-files in working directory:\n");
  printFileList (cfiles);
}

void testStringBuilder (void) {
  stringBuilder_t* sb = stringBuilder_new (8);

  stringBuilder_append (sb, "Wie");
  char* s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_appendChar (sb, ' ');
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_append (sb, "komm");
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_appendChar (sb, ' ');
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_append (sb, "ich");
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_append (sb, " am B");
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_append (sb, "esten den Berg hinan?");
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_clear (sb);
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_append (sb, "Steig nur hinauf");
  s = stringBuilder_toString (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));
  free (s);

  stringBuilder_appendChar (sb, ' ');
  stringBuilder_appendChar (sb, 'u');
  stringBuilder_appendChar (sb, 'n');
  stringBuilder_appendChar (sb, 'd');
  stringBuilder_appendChar (sb, ' ');
  s = (char*) stringBuilder_getBuffer (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));

  stringBuilder_append (sb, "denk nicht dran!");
  s = (char*) stringBuilder_getBuffer (sb);
  printf ("s = \"%s\", l = %d (actual: %ld)\n", s, stringBuilder_length (sb), strlen (s));

  stringBuilder_delete (sb);
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
