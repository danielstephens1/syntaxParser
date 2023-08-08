/*
* This program reads through a file and determines the lexemes and tokens
* and parses them through recursive descent to determine if the provided
* file follows the grammar of DanC
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define KEY 2
#define UNKNOWN 99

/* Token codes */
#define INT_LIT 10
#define IDENT 11

#define ASSIGN_OP 20
#define LESSER_OP 21
#define GREATER_OP 22
#define EQUAL_OP 23
#define NEQUAL_OP 24
#define LEQUAL_OP 25
#define GEQUAL_OP 26

#define ADD_OP 27
#define SUB_OP 28
#define MULT_OP 29
#define DIV_OP 30
#define LEFT_PAREN 31
#define RIGHT_PAREN 32

#define KEY_READ 33
#define KEY_WRITE 34
#define KEY_WHILE 35
#define KEY_DO 36
#define KEY_OD 37
#define SEMICOLON 38

int lex();
int nextToken;


/* Local Variables */
static int charClass;
static char lexeme [100];
static char tokenName [100];
static char nextChar;
static int lexLen;
static FILE *in_fp;

/* Local Function declarations */
static void addChar();
static void getChar();
static void addGet();
static void delChar();
static void getNonBlank();
void P();
void S();
void S2();
void E();
void E2();
void T();
void T2();
void C();
void F();
void error(int errorNum);
void exit(int status);

/******************************************************/
/* main driver */
int main(int argc, char *argv[]) {
    printf("DanC Analyzer\n\n");
    if (argc < 2){
        error(2);
    }
    if (argc == 2){
    /* Open the input data file and process its contents */
        if ((in_fp = fopen(argv[1], "r")) == NULL) {
            error(3);
        } else {
            getChar();
            do {
                lex();
                P();

            } while (nextToken != EOF);
        }
    }
    printf("Syntax Verified\n");
    exit(0);
}

/*****************************************************/
/* lookup - a function to lookup operators and parentheses and return the
 * token */
static int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            strcpy(tokenName,"LEFT_PAREN");
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            strcpy(tokenName,"RIGHT_PAREN");
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            strcpy(tokenName,"ADD_OP");
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            strcpy(tokenName,"SUB_OP");
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            strcpy(tokenName,"MULT_OP");
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            strcpy(tokenName,"DIV_OP");
            break;
        case '=':
            addChar();
            nextToken = EQUAL_OP;
            strcpy(tokenName,"EQUAL_OP");
            break;

        case ';':
            addChar();
            nextToken = SEMICOLON;
            strcpy(tokenName,"SEMICOLON");
            break;

        /* Imbedded switch statements to account for two variable opperands */
        case '<':
        addGet();
        switch (nextChar) {
            case '=':
                addGet();
                nextToken = LEQUAL_OP;
                strcpy(tokenName,"LEQUAL_OP");
                break;
            case '>':
                addChar();
                nextToken = NEQUAL_OP;
                strcpy(tokenName,"NEQUAL_OP");
                break;

            default:
                delChar();
                nextToken = LESSER_OP;
                strcpy(tokenName,"LESSER_OP");
                break;
        }
        break;
        case '>':
            addGet();
            switch (nextChar) {
                case '=':
                    addChar();
                    nextToken = GEQUAL_OP;
                    strcpy(tokenName,"GEQUAL_OP");
                    break;

                default:
                    delChar();
                    nextToken = GREATER_OP;
                    strcpy(tokenName,"GREATER_OP");
                    break;
            }
            break;

        /* : can be either ASSIGN_OP or an unknown lexeme */
        case ':':
            addGet();
            switch (nextChar) {
                case '=':
                    addChar();
                    nextToken = ASSIGN_OP;
                    strcpy(tokenName,"ASSIGN_OP");
                    break;

                default:
                    delChar();
                    nextToken = UNKNOWN;
                    strcpy(tokenName,"UNKNOWN");
                    break;
            }
            break;

        /* Lexemes that are invalid are defaulted to UNKNOWN */
        default:
            addChar();
            nextToken = UNKNOWN;
            strcpy(tokenName,"UNKNOWN");
            break;
    }
    return nextToken;
}

/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
static void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    } else {
        printf("Error - lexeme is too long \n");
    }
}

/*****************************************************/
/* getChar - a function to get the next character of input and determine its
 * character class */
static void getChar() {
    if ((nextChar = getc(in_fp)) != EOF) {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else charClass = UNKNOWN;

    } else {
        charClass = EOF;
    }
}

/*****************************************************/
/* addGet - a function to combine addChar and getChar */
static void addGet() {
    addChar();
    getChar();
}

/*****************************************************/
/* delChar - a function to move the pointer of the character in the input back
 * one space */
static void delChar() {
    ungetc(nextChar, in_fp);
}

/*****************************************************/
/* getNonBlank - a function to call getChar until it returns a non-whitespace
 * character */
static void getNonBlank() {
    while (isspace(nextChar)) getChar();
}

/*****************************************************/
/* lex - a simple lexical analyzer for arithmetic expressions */
int lex() {
    lexLen = 0;
    getNonBlank();

    switch (charClass) {
        /* Parse identifiers */
        case LETTER:
            addGet();
            while (charClass == LETTER) {
                addGet();
            }

            /* if statements test against keywords to find KEY lexemes */
            if (strcmp(lexeme, "read") == 0) {
                nextToken = KEY_READ;
                strcpy(tokenName, "KEY_READ");
                break;
            }
            else if (strcmp(lexeme, "write") == 0) {
                nextToken = KEY_WRITE;
                strcpy(tokenName, "KEY_WRITE");
                break;
            }
            else if (strcmp(lexeme, "while") == 0) {
                nextToken = KEY_WHILE;
                strcpy(tokenName, "KEY_WHILE");
                break;
            }
            else if (strcmp(lexeme, "do") == 0) {
                nextToken = KEY_DO;
                strcpy(tokenName, "KEY_DO");
                break;
            }
            else if (strcmp(lexeme, "od") == 0) {
                nextToken = KEY_OD;
                strcpy(tokenName, "KEY_OD");
                break;
            }
            else {
                nextToken = IDENT;
                strcpy(tokenName, "IDENT");
                break;
            }

        /* Parse integer literals */
        case DIGIT:
            addGet();
            while (charClass == DIGIT) {
                addGet();
            }
            nextToken = INT_LIT;
            strcpy(tokenName,"INT_LIT");
            break;

        /* Parentheses and operators */
        case UNKNOWN:
            lookup(nextChar);
            getChar();
            break;

        /* EOF */
        case EOF:
            nextToken = EOF;
            strcpy(tokenName,"EOF");
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    } /* End of switch */

    //if (nextToken != EOF) printf(" %s\t %s\n", lexeme, tokenName);

    return nextToken;
} /* End of function lex */

/****************************************************************
 * P
 * Parses strings in the language generated by the rule:
 * P ::= S
 */
void P() {
  //printf("Enter <P>\n");
    S();
    /* Checks if the next token is a SEMICOLON followed by EOF and
     * causes error if detected
     */
    if (nextToken != SEMICOLON) {
      if (nextToken != EOF) {
        error(1);
      }
    }
  //printf("Exit <P>\nexit(0)\nSyntax Validated");
  //exit(0);
} /* End of function P */

/****************************************************************
 * S
 * Parses strings in the language generated by the rule:
 * S ::= V := ES2 | read(V)S2 | write(V)S2 | while C do S od S2
 */
void S() {
    //printf("Enter <S>\n");
    if (nextToken == IDENT) {
      lex();
      if (nextToken == ASSIGN_OP) {
        lex();
        E();
        S2();
      }
      else {
        error(1);
      }
    }
    else if (nextToken == KEY_READ || nextToken == KEY_WRITE) {
      lex();
      if (nextToken == LEFT_PAREN) {
        lex();
        if (nextToken == IDENT) {
          lex();
          if (nextToken == RIGHT_PAREN) {
            lex();
            S2();
          }
          else {
            error(1);
          }
        }
        else {
          error(1);
        }
      }
      else {
        error(1);
      }
    }
    else if (nextToken == KEY_WHILE) {
      lex();
      C();
      if (nextToken == KEY_DO) {
        lex();
        S();
        if (nextToken == KEY_OD) {
          lex();
          S2();
        }
        else {
          error(1);
        }
        S2();
      }
      else {
        error(1);
      }
    }
    //printf("Exit <S>\n");
} /* End of function S */

/****************************************************************
 * S'
 * Parses strings in the language generated by the rule:
 * S' ::= ;SS2 | e
 */
void S2() {
    //printf("Enter <S'>\n");
    while (nextToken == SEMICOLON) {
      lex();
      if (nextToken == EOF) {
          error(1);
      }
      if(nextToken == SEMICOLON) {
        error(1);
      }
      else {
        S();
        S2();
      }
    }
    //printf("Exit <S'>\n");
} /* End of function S' */

/****************************************************************
 * C
 * Parses strings in the language generated by the rule:
 * C ::= E < E | E > E | E = E | E <> E | E <= E | E >= E
 * can only be called after "while" KEY
 */
void C() {
    //printf("Enter <C>\n");
      E();
      while (nextToken == LESSER_OP || nextToken == GREATER_OP || nextToken == EQUAL_OP ||
         nextToken == NEQUAL_OP || nextToken == LEQUAL_OP || nextToken == GEQUAL_OP) {
        lex();
        E();
      }
    //printf("Exit <C>\n");
} /* End of function C */

/****************************************************************
 * E
 * Parses strings in the language generated by the rule:
 * E ::= TE2
 */
void E() {
    //printf("Enter <E>\n");
    /* Parse the first term */
    T();
    E2();
    //printf("Exit <E>\n");
} /* End of function E */

/****************************************************************
 * E'
 * Parses strings in the language generated by the rule:
 * E' ::= +TE2 | -TE2 | e
 */
void E2() {
    //printf("Enter <E'>\n");
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
      lex();
      T();
      E2();
    }
    //printf("Exit <E'>\n");
} /* End of function E' */

/****************************************************************
 * T
 * Parses strings in the language generated by the rule:
 * T ::= FT2
 */
void T() {
    //printf("Enter <T>\n");
    /* Parse the first factor */
    F();
    T2();
    //printf("Exit <T>\n");
} /* End of function T */

/****************************************************************
 * T'
 * Parses strings in the language generated by the rule:
 * T' ::= *FT2 | /FT2 | e
 */
void T2() {
    //printf("Enter <T'>\n");
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
      lex();
      F();
      T2();
    }
    //printf("Exit <T'>\n");
}/* End of function T' */

/****************************************************************
 * F
 * Parses strings in the language generated by the rule:
 * F ::= (E) | V | N
 * */
void F() {
    //printf("Enter <F>\n");
    /* Determine which RHS */
    if (nextToken == IDENT || nextToken == INT_LIT) {
        lex();
    }
    else {
        if (nextToken == LEFT_PAREN) {
            lex();
            E();

            if (nextToken == RIGHT_PAREN) {
                lex();
            }
            else {
                error(1);
            }
        }
        else {
            error(1);
        }
    }
    //printf("Exit <F>\n");
} /* End of function F */

/****************************************************************
 * error
 * list of error codes and exit codes when an error occurs
 */
void error(int errorNum) {
    //syntax error
    if (errorNum == 1) {
      printf("Error encounter: The next lexeme was %s and the next token was %s.\n", lexeme, tokenName);
      exit(1);
  }
    //no input file provided
    else if (errorNum == 2) {
      printf("No file was provided for input.\n");
      exit(2);
    }
    //input provided, no match
    else if (errorNum == 3) {
      printf("The file provided could not be found.\n");
      exit(3);
    }
    else {
      printf("Error Unknown\n");
    }
}
