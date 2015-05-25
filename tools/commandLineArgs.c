/*
   Author:  Gerard Visser
   e-mail:  visser.gerard(at)gmail.com

   Copyright (C) 2015 Gerard Visser.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "commandLineArgs.h"
#include "errors.h"

#define MAX_OPTIONS (MAX_OPT_KEY + 1 - MIN_OPT_KEY)
#define MAX_OPT_KEY 0x7E
#define MIN_OPT_KEY 0x21

#define validateOptionKey(key) \
  if (key < MIN_OPT_KEY | key > MAX_OPT_KEY) { \
    errors_printMessageAndExit ("\x1B[7mError in function '%s': invalid option key (valid range: 0x%02X - 0x%02X)\x1B[27m", __func__, MIN_OPT_KEY, MAX_OPT_KEY); \
  }

typedef enum {
  COMMAND_LINE_ARGS_OPTION_VALUE_TYPE_BOOL,
  COMMAND_LINE_ARGS_OPTION_VALUE_TYPE_STRING
} commandLineArgs_option_valueType_t;

struct commandLineArgs_struct {
  commandLineArgs_option_t** options;
  stringList_t* mainArgs;
};

struct commandLineArgs_option_struct {
  union {
    bool b;
    stringList_t* list;
  } value;

  const char* defaultValue;
  commandLineArgs_option_valueType_t valueType;
  char key;
};

static commandLineArgs_t* commandLineArgs_alloc (void) {
  commandLineArgs_t* result = malloc (sizeof (commandLineArgs_t));
  result->options = malloc (MAX_OPTIONS * sizeof (void*));
  int i;
  for (i = 0; i < MAX_OPTIONS; ++i) {
    result->options[i] = NULL;
  }
  result->mainArgs = NULL;
  return result;
}

static commandLineArgs_option_t* commandLineArgs_parseOptionArg (commandLineArgs_t* this, const char* arg) {
  const int arglen = strlen (arg);
  commandLineArgs_option_t* option = NULL;
  int i = 0;
  while (i < arglen) {
    char optkey = arg[i];
    if (optkey >= MIN_OPT_KEY & optkey <= MAX_OPT_KEY) {
      option = this->options[optkey - MIN_OPT_KEY];
    }
    if (option == NULL) {
      errors_printMessageAndExit ("Invalid option '%c'", optkey);
    }
    if (option->valueType == COMMAND_LINE_ARGS_OPTION_VALUE_TYPE_STRING) {
      if (arglen > i + 1) {
        option->value.list = stringList_append (option->value.list, arg + i + 1);
        option = NULL;
      }
      i = arglen;
    } else {
      option->value.b = true;
      option = NULL;
      ++i;
    }
  }
  return option;
}

void commandLineArgs_delete (commandLineArgs_t* this) {
  int i;
  for (i = 0; i < MAX_OPTIONS; ++i) {
    if (this->options[i] != NULL) {
      commandLineArgs_option_delete (this->options[i]);
    }
  }
  stringList_delete (this->mainArgs);
  free (this->options);
  free (this);
}

bool commandLineArgs_getBoolOptionValue (commandLineArgs_t* this, char optKey) {
  validateOptionKey (optKey);
  commandLineArgs_option_t* option = this->options[optKey - MIN_OPT_KEY];
  if (option == NULL) {
    errors_printMessageAndExit ("\x1B[7mError in function '%s': unknown option 0x%02X\x1B[27m", __func__, optKey);
  }
  return option->value.b;
}

stringList_t* commandLineArgs_getMainArgs (commandLineArgs_t* this) {
  return this->mainArgs;
}

stringList_t* commandLineArgs_getStringOptionValue (commandLineArgs_t* this, char optKey) {
  validateOptionKey (optKey);
  commandLineArgs_option_t* option = this->options[optKey - MIN_OPT_KEY];
  if (option == NULL) {
    errors_printMessageAndExit ("\x1B[7mError in function '%s': unknown option 0x%02X\x1B[27m", __func__, optKey);
  }
  return option->value.list;
}

/* A segmentation fault might occur when the last var-args argument is not a null pointer. */
commandLineArgs_t* commandLineArgs_new (int argc, char** args, ...) {
  commandLineArgs_t* result = commandLineArgs_alloc ();

  va_list argp;
  va_start (argp, args);
  commandLineArgs_option_t* option = va_arg (argp, commandLineArgs_option_t*);
  while (option != NULL) {
    result->options[option->key - MIN_OPT_KEY] = option;
    option = va_arg (argp, commandLineArgs_option_t*);
  }
  va_end (argp);

  int i = 1;
  while (i < argc) {
    if (args[i][0] == '-') {
      if (args[i][1] != 0) {
        option = commandLineArgs_parseOptionArg (result, args[i] + 1);
      } else {
        errors_printMessageAndExit ("Option expected after '-'");
      }
    } else if (option != NULL) {
      option->value.list = stringList_append (option->value.list, args[i]);
      option = NULL;
    } else {
      result->mainArgs = stringList_append (result->mainArgs, args[i]);
    }
    ++i;
  }

  if (option != NULL) {
    errors_printMessageAndExit ("Value expected for option '%c'", option->key);
  }

  for (i = 0; i < MAX_OPTIONS; ++i) {
    if (result->options[i] != NULL && result->options[i]->valueType == COMMAND_LINE_ARGS_OPTION_VALUE_TYPE_STRING) {
      if (result->options[i]->value.list == NULL & result->options[i]->defaultValue != NULL) {
        result->options[i]->value.list = stringList_new (result->options[i]->defaultValue);
      } else {
        result->options[i]->value.list = stringList_firstElement (result->options[i]->value.list);
      }
    }
  }
  result->mainArgs = stringList_firstElement (result->mainArgs);

  return result;
}

void commandLineArgs_option_delete (commandLineArgs_option_t* this) {
  if (this->valueType == COMMAND_LINE_ARGS_OPTION_VALUE_TYPE_STRING) {
    stringList_delete (this->value.list);
  }
  free (this);
}

commandLineArgs_option_t* commandLineArgs_option_newBoolOption (char key) {
  validateOptionKey (key);
  commandLineArgs_option_t* result = malloc (sizeof (commandLineArgs_option_t));
  result->valueType = COMMAND_LINE_ARGS_OPTION_VALUE_TYPE_BOOL;
  result->value.b = false;
  result->key = key;
  return result;
}

commandLineArgs_option_t* commandLineArgs_option_newStringOption (char key, const char* defaultValue) {
  validateOptionKey (key);
  commandLineArgs_option_t* result = malloc (sizeof (commandLineArgs_option_t));
  result->valueType = COMMAND_LINE_ARGS_OPTION_VALUE_TYPE_STRING;
  result->defaultValue = defaultValue;
  result->value.list = NULL;
  result->key = key;
  return result;
}
