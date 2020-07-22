/* Solution to comp10002 Assignment 1, 2019 semester 2.

   Authorship Declaration:

   (1) I certify that the program contained in this submission is completely
   my own individual work, except where explicitly noted by comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students,
   or by non-students as a result of request, solicitation, or payment,
   may not be submitted for assessment in this subject.  I understand that
   submitting for assessment work developed by or in collaboration with
   other students or non-students constitutes Academic Misconduct, and
   may be penalized by mark deductions, or by other penalties determined
   via the University of Melbourne Academic Honesty Policy, as described
   at https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will
   I do so until after the marks are released. I understand that providing
   my work to other students, regardless of my intention or any undertakings
   made to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring
   service, or drawing the attention of others to such services and code
   that may have been made available via such a service, may be regarded
   as Student General Misconduct (interfering with the teaching activities
   of the University and/or inciting others to commit Academic Misconduct).
   I understand that an allegation of Student General Misconduct may arise
   regardless of whether or not I personally make use of such solutions
   or sought benefit from such actions.

   Signed by: Gabriel George Jackson 983339
   Dated:     22/09/2019

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXCHARS 999 /* Max chars per line, and thus per word  */
#define MAXWORDS 2000 /* Unfortunately I can't hold an array bigger than this */
#define STARTINGPOINT 4 /* Default margin */
#define LINELEN 50 /* Default line length */

#define FALSE 0
#define TRUE 1

#define PWO 2 /* Previous word offset */
#define THISWO 1 /* This word offset */

/* Command definitions */
#define UNKNOWNCMD 1 /* Unknown command */
#define BREAKLN 2 /* Break line */
#define PARAG 3 /* Paragraph */
#define LMARGIN 4 /* Line margin */
#define LWIDTH 5 /* Line width */
#define CENTER 6 /* Center code */
#define TBCENTERED 7 /* Word to be centered */
#define HEADING 8 /* Add heading */
#define TBHEADING 9 /* Word to be in heading */

#define HEADINGS 5 /* Max sub headings */

#define TENSDIGIT 10 /* Value of ten's digit */
#define TWODIGITS 2 /* Max digits */

/* Define a variable type that is an array of chars */
typedef char word_t[MAXCHARS+1];

/* Define a struct that contains an array of word, and properties of the word */
struct Word {
    word_t w;
    /* cmd - command */
    int line_num, cmd, length, cmd_val, header[HEADINGS];
};

/* Function to get the words in input, and put it into the Word struct */
int getWord(struct Word word, struct Word *all_words, int limit, int* line_num,
        int* total_words);
/* Removes any word, and pushes all proceeding words forward */
void blacklist(struct Word *all_words, int* total_words, int i);
/* Print and format all of the words */
void format(struct Word *all_words, int* margin, int* line_len,
        int* total_words, int* header);
/* Goes to next line, and goes to the margin */
int newLineMargin(int* curr_line_len, int margin);

int
main(int argc, char *argv[]) {
    /* Initialise struct word, and the array of word structs*/
    struct Word word, all_words[MAXWORDS];
    /* Variables to keep track of total words, line, last black listed line,
     * last centered line, the num of words to be centered, the length of the
     * total words to be centered, the default margin and line length, the
     * last line to be a heading, and the headings and sub headings*/
    int total_words = 0, line_num = 0, i, bl_line = -1, c_line = -1, c_num = 0,
    c_len = 0, margin = STARTINGPOINT, line_len = LINELEN, h_line = -1,
    header[HEADINGS];
    /* Initialise header for any HEADINGS */
    for (i = 0; i < HEADINGS; ++i) {
        header[i] = 0;
    }
    /* While not at the end of file, get the words */
    while (getWord(word, all_words, MAXCHARS + 1, &line_num, &total_words)
    != EOF) {
    }
    /* Assess all the commands for each word */
    for (i = 0; i < total_words; i++) {
        /* Make sure it is a word with a command */
        if (all_words[i].cmd || all_words[i].line_num == bl_line ||
            all_words[i].line_num == c_line || all_words[i].line_num == h_line){
            /* If it's .l or .w */
            if (all_words[i].cmd == LWIDTH || all_words[i].cmd == LMARGIN) {
                /* Check if it being assigned a 2 digit num, else 1 digit num */
                if (all_words[i + 1].length == TWODIGITS) {
                    /* Get the 2 digit number and assign it to the .l or .w */
                    char ten_char = all_words[i + 1].w[0],
                            one_char = all_words[i + 1].w[1];
                    int ten_digit = ten_char - '0', one_digit = one_char - '0';
                    all_words[i].cmd_val = TENSDIGIT * ten_digit + one_digit;
                } else {
                    /* Get the 1 digit number and assign it to the .l or .w */
                    char one_char = all_words[i + 1].w[0];
                    int one_digit = one_char - '0';
                    all_words[i].cmd_val = one_digit;
                }
                /* Get rid of nn following .w and .l */
                blacklist(all_words, &total_words, i + 1);
            /* Get rid of any words following .p or .b */
            } else if (all_words[i].cmd == PARAG || all_words[i].cmd ==
                BREAKLN) {
                bl_line = all_words[i].line_num;
            /* If there is another command not accounted for (mostly for part
             * 1/2) or there is a black listed line, get rid of it. */
            } else if (all_words[i].cmd == UNKNOWNCMD || all_words[i].line_num
                == bl_line) {
                bl_line = all_words[i].line_num;
                blacklist(all_words, &total_words, i);
                /* As everything is being pushed back 1, need to account for
                 * that */
                i--;
            /* If the command is .c, reset all previous .c information */
            } else if (all_words[i].cmd == CENTER) {
                c_len = 0;
                c_num = 0;
                c_line = all_words[i].line_num;
            /* Make every word on the .c be centered in format(); and get the
             * length of the line */
            } else if (all_words[i].line_num == c_line) {
                c_len += all_words[i].length;
                c_num++;
                /* Assign line length to the .c word */
                all_words[i - c_num].cmd_val = c_len + c_num - 1;
                all_words[i].cmd = TBCENTERED;
            /* Add the necessary sub heading / reset a sub heading if needed */
            } else if (all_words[i].cmd == HEADING) {
                /* Get the num of sub heading */
                char one_char = all_words[i + 1].w[0];
                int one_digit = one_char - '0', j;
                /* Associate .h line to all following words on line */
                h_line = all_words[i].line_num;
                /* Assign the sub heading according to one_digit, and reset
                 * all following sub heading information */
                for (j = 0; j < HEADINGS; j++) {
                    if (one_digit - 1 == j) {
                        if (j == 0) {
                            all_words[i].cmd_val = TRUE;
                        }
                        header[j]++;
                    } else if (one_digit - 1 < j){
                        header[j] = 0;
                    }
                    /* Assign sub heading information to word */
                    all_words[i].header[j] = header[j];
                }
                /* Delete the nn following .h */
                blacklist(all_words, &total_words, i + 1);
            /* Assigns words to be part of heading if on .h line */
            } else if (all_words[i].line_num == h_line) {
                all_words[i].cmd = TBHEADING;
            }
        }
    }
    /* If there are floating .p and .b after code, get rid of them, until it
     * gets to a non .p or .b word and quit loop */
    for (i = total_words - 2; i >= 0 && (all_words[i].cmd == PARAG ||
        all_words[i].cmd == BREAKLN); i--) {
        blacklist(all_words, &total_words, i);
    }
    /* Format all of the words */
    format(all_words, &margin, &line_len, &total_words, header);
    return 0;
}

int
getWord(struct Word word, struct Word *all_words, int limit, int* line_num,
         int* total_words) {
    /* Initialise characters, and store word len, and store of last char was
     * a space */
    int c, len = 0, word_len = 1, lst_c_space = FALSE;
    /* Initialise word command to FALSE, and word line num to line num */
    word.cmd = FALSE;
    word.line_num = *line_num;
    *total_words = *total_words + 1;
    /* Loop through all chars in input, skipping and space, \n, \t and \r */
    while ((c = getchar()) != EOF && ((char)c == ' ' || (char)c == '\n' ||
        (char)c == '\t' || (char)c == '\r')) {
        /* Reset last space if looped past a space and increment lin num*/
        lst_c_space = FALSE;
        if ((char)c == '\n') {
            *line_num = *line_num + 1;
        } else if ((char)c == ' ') {
            lst_c_space = TRUE;
        }
    }
    if (c == EOF) {
        return EOF;
    }
    /* Assign line num to word, and generate full word */
    all_words[*total_words - THISWO].line_num = *line_num;
    word.w[len++] = c;
    while (len < limit && (c = getchar()) != EOF && !((char)c == ' ' ||
        (char)c == '\n' || (char)c == '\t' || (char)c == '\r')) {
        word.w[len++] = c;
        word_len++;
    }
    /* Assign necessary command making sure there was no preceding space,
     * as is on a new line*/
    if (word.w[0] == '.' && ((all_words[*total_words-PWO].line_num <
        all_words[*total_words-THISWO].line_num && !lst_c_space) ||
        *total_words == 1)) {
        if (word.w[1] == 'b') {
            all_words[*total_words - THISWO].cmd = BREAKLN;
        } else if (word.w[1] == 'p') {
            all_words[*total_words - THISWO].cmd = PARAG;
        } else if (word.w[1] == 'l') {
            all_words[*total_words - THISWO].cmd = LMARGIN;
        } else if (word.w[1] == 'w') {
            all_words[*total_words - THISWO].cmd = LWIDTH;
        } else if (word.w[1] == 'c') {
            all_words[*total_words - THISWO].cmd = CENTER;
        } else if (word.w[1] == 'h') {
            all_words[*total_words - THISWO].cmd = HEADING;
        } else {
            all_words[*total_words - THISWO].cmd = UNKNOWNCMD;
        }
    }
    if ((char)c == '\n') {
        *line_num = *line_num + 1;
    }
    /* Finish word, and return */
    all_words[*total_words - THISWO].length = word_len;
    word.w[len] = '\0';
    strcpy(all_words[*total_words - THISWO].w, word.w);
    return 0;
}

void
blacklist(struct Word *all_words, int* total_words, int i) {
    /* Remove word at position i and move all words back 1 */
    int j;
    *total_words = *total_words - 1;
    for (j = i; j < *total_words; j++) {
        all_words[j] = all_words[j + 1];
    }
}

/* Very messy, could definitely be reworked for general 'betterness',
 * knowing when to add lines just became trial and error by the end, and will
 * almost certainly be a problem . */
void
format(struct Word *all_words, int* margin, int* line_len, int* total_words,
        int* header) {
    /* Initialise line len, var to track if last word was a command, and
     * var to check if a new line was made */
    int curr_line_len = *margin, i, lst_wrd_cmd = FALSE, lst_ln_new_ln = TRUE;
    for (i = 0; i < *margin; i++) {
        printf(" ");
    }
    for (i = 0; i < *total_words - 1; i++) {
        /* If the word is not a command, increment line len and print word */
        if (!all_words[i].cmd) {
            curr_line_len += all_words[i].length;
            printf("%s", all_words[i].w);
            /* If the next word will go over width, start new line */
            if (all_words[i + 1].length && curr_line_len +
                all_words[i + 1].length + 1 > *line_len + *margin) {
                /* I use this a lot, I don't know how to make this into a
                 * function, and google isn't useful, but it makes sure
                 * that the next line isn't a .l command otherwise it will
                 * increment the margin by the previous margin. I would be
                 * interested to know how to do it, just pointer things are
                 * a pain in my *** */
                if (all_words[i + 1].cmd != LMARGIN){
                    newLineMargin(&curr_line_len, *margin);
                } else {
                    newLineMargin(&curr_line_len, 0);
                }
                lst_ln_new_ln = TRUE;
            /* Check if it's the end of array */
            } else if (!all_words[i + 1].length) {
                printf("\n");
            /* otherwise just add a space */
            } else {
                lst_ln_new_ln = FALSE;
                curr_line_len++;
                printf(" ");
            }
            lst_wrd_cmd = FALSE;
        /* If it makes it here, it's a command */
        } else {
            /* Do break line things, making sure wasn't preceded by .b*/
            if (all_words[i].cmd == BREAKLN && lst_wrd_cmd != BREAKLN) {
                if (all_words[i + 1].cmd != LMARGIN) {
                    newLineMargin(&curr_line_len, *margin);
                } else {
                    newLineMargin(&curr_line_len, 0);
                }
            /* Same for .p */
            } else if (all_words[i].cmd == PARAG && lst_wrd_cmd != PARAG) {
                if (!lst_ln_new_ln) {
                    printf("\n");
                }
                if (all_words[i + 1].cmd != LMARGIN){
                    newLineMargin(&curr_line_len, *margin);
                } else {
                    newLineMargin(&curr_line_len, 0);
                }
            /* Set new margin and set the margin, being careful of previous .w
             * or .l and not adding unnecessary lines */
            } else if (all_words[i].cmd == LMARGIN) {
                *margin = all_words[i].cmd_val;
                /* Check for previous .l and .w */
                if (lst_wrd_cmd != LWIDTH && lst_wrd_cmd != LMARGIN) {
                    if (!lst_ln_new_ln) {
                        printf("\n");
                    }
                    if (i != 0) {
                        newLineMargin(&curr_line_len, *margin);
                    }
                } else {
                    /* Add margin */
                    int k;
                    curr_line_len = *margin;
                    for (k = 0; k < *margin; k++) {
                        printf(" ");
                    }
                }
            /* Similar for .w, set new width and add lines where needed */
            } else if (all_words[i].cmd == LWIDTH) {
                *line_len = all_words[i].cmd_val;
                if (lst_wrd_cmd != LWIDTH && lst_wrd_cmd != LMARGIN) {
                    if (!lst_ln_new_ln) {
                        printf("\n");
                    }
                    if (i != 0) {
                        /* Here it is again! */
                        if (all_words[i + 1].cmd != LMARGIN){
                            newLineMargin(&curr_line_len, *margin);
                        } else {
                            newLineMargin(&curr_line_len, 0);
                        }
                    }
                }
            /* .c things */
            } else if (all_words[i].cmd == CENTER) {
                /* Make sure there is something to format */
                if (!all_words[i].cmd_val) {
                    lst_ln_new_ln = newLineMargin(&curr_line_len, *margin);
                } else {
                    if (!lst_ln_new_ln && lst_wrd_cmd != PARAG) {
                        newLineMargin(&curr_line_len, *margin);
                    }
                    /* Increment necessary spaces based on line lengths */
                    int j;
                    for (j = 0; j < (*line_len - all_words[i].cmd_val)/2; j++) {
                        printf(" ");
                    }
                }
            /* Any word to be centered here */
            } else if (all_words[i].cmd == TBCENTERED) {
                printf("%s", all_words[i].w);
                /* Check if next word is to be centered, else next line */
                if (all_words[i + 1].cmd != TBCENTERED) {
                    if (all_words[i + 1].cmd != LMARGIN &&
                    all_words[i + 1].length){
                        newLineMargin(&curr_line_len, *margin);
                    } else {
                        newLineMargin(&curr_line_len, 0);
                    }
                    lst_ln_new_ln = TRUE;
                } else {
                    printf(" ");
                }
            /* Very similar for heading */
            } else if (all_words[i].cmd == HEADING) {
                /* Just line spacing things */
                if (lst_wrd_cmd != PARAG) {
                    if (!lst_ln_new_ln) {
                        printf("\n");
                    }
                    lst_ln_new_ln = newLineMargin(&curr_line_len, *margin);
                }
                /* Add ------ line if needed */
                if (all_words[i].cmd_val) {
                    int j;
                    for (j = 0; j < *line_len; j++) {
                        printf("-");
                    }
                    newLineMargin(&curr_line_len, *margin);
                }
                /* Print the primary header number */
                printf("%d", all_words[i].header[0]);
                int j, num_sub = 1;
                /* Check for when to terminate heading numbers */
                for (j = 1; j < HEADINGS; j++) {
                    if (all_words[i].header[j]){
                        num_sub = j + 1;
                    }
                }
                /* Up until latest sub heading, print numbers */
                for (j = 1; j < num_sub; j++) {
                    printf(".%d", all_words[i].header[j]);
                }
                printf(" ");
            /* Any word in the heading line */
            } else if (all_words[i].cmd == TBHEADING) {
                printf("%s", all_words[i].w);
                /* Check if next word is in heading */
                if (all_words[i + 1].cmd != TBHEADING) {
                    /* If so add necessary lines */
                    printf("\n");
                    if (all_words[i + 1].cmd != LMARGIN){
                        newLineMargin(&curr_line_len, *margin);
                    } else {
                        newLineMargin(&curr_line_len, 0);
                    }
                    lst_ln_new_ln = TRUE;
                    all_words[i].cmd = PARAG;
                }  else {
                    printf(" ");
                }
            }
            /* Remember the command */
            lst_wrd_cmd = all_words[i].cmd;
        }
    }
}

int newLineMargin(int* curr_line_len, int margin) {
    /* Make a new line, and increment to margin */
    int k;
    printf("\n");
    *curr_line_len = margin;
    for (k = 0; k < margin; k++) {
        printf(" ");
    }
    return TRUE;
}

/* Algorithms are fun! */