#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int count_words(char *, int, int);
int reverse_string(char *, int, int);
int print_words(char *, int, int);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    // -2 return code to check if user_str is null or is an empty string
    if (user_str == NULL) {
        return -2;
    }

    char *s = user_str;
    char *b = buff;
    int rv = 0;

    // Skipping beginning white spaces or tabs
    while (*s != '\0' && (*s == ' ' || *s == '\t')) {
        s++;
    }

    // Checks if the string is empty & if it is return -2
    if (*s == '\0') {
        return -2;
    }

    while (*s != '\0' && rv < len) {
        if ((*s == ' ' || *s == '\t') && (*(s - 1) != ' ' && *(s - 1) != '\t')) {
            *b = ' ';
            b++;
            rv++;
        } else if (*s != ' ') {
            *b = *s;
            b++;
            rv++;
        }
        s++;
    }

    // Error check to see if s > BUFFER_SZ
    if (rv == len && *s != '\0') {
        return -1;
    } else if (rv == len && *s == '\0') {
        return rv;
    }

    // Last word will always be a ' ', so in order to counteract that just decrement.
    if (rv > 0 && *(b - 1) == ' ') {
        b--;
        rv--;
    }

    int filler = len - rv;
    for (int i = 0; i < filler; i++) {
        *b = '.';
        b++;
    }
    return rv;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar(']');
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    // Return exit value of -1 is a checker if string was not properly copied over to the buffer;
    if (str_len < 0 || str_len > len) {
        return -1;
    }

    // in_word: bool to check if current char is in word
    // loop essentially checks if there is a space or if buff[i] = '.' to count word
    int rv = 0;
    int in_word = 1;
    for(int i = 0; i <= str_len; i++) {
        if (i == str_len) {
            rv++;
        }

        if (buff[i] == ' ') {
            if (!in_word) {
                rv++;
                in_word = 1;
            } 
        } else {
            in_word = 0;
        }
    }
    return rv;
}

int reverse_string(char *buff, int len, int str_len) {
    // Return exit value of -1 is a checker if string was not properly copied over to the buffer;
    if (str_len < 0 || str_len > len) {
        return - 1;
    }
    int l = 0;
    int r = str_len - 1;

    // Uses two pointers to reverse strings 
    while (l < r) {
        char temp = buff[l];
        buff[l] = buff[r];
        buff[r] = temp;
        l++;
        r--;
    }

    return 0;
}


int print_words(char *buff, int len, int str_len) {
    // Return exit value of -1 is a checker if string was not properly copied over to the buffer;
    if (str_len < 0 || str_len > len) {
        return -1;
    }

    printf("Word Print\n----------\n");
    
    char *ptr = buff;
    int word_num = 1;
    int char_count = 0;
    int in_word = 0;
    
    // Loop through the buffer to print each word and its character count
    for (int i = 0; i < str_len; i++) {
        if (*ptr != ' ') {
            if (!in_word) {
                // Start new word
                printf("%d. ", word_num++);
                in_word = 1;
                char_count = 0;
            }
            putchar(*ptr);
            char_count++;
        } else if (in_word) {
            // End of word
            printf("(%d)\n", char_count);
            in_word = 0;
        }
        ptr++;
    }
    
    // Handle the last word if it exists
    if (in_word) {
        printf("(%d)\n", char_count);
    }

    printf("\nNumber of words returned: %d\n", --word_num);
    return 0;
}


//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      This is safe because it first checks if the argument count is less than 2 => there is no flag
    //      The second check is checking if the flag is properly places with a '-' in it's beginning
    //      If the second argument doesn't exist then the if-statment would print the proper usage in the terminal
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      The purpose of this if-statment is to check if the user put in a string as it's third argument
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ);
    if(buff == NULL) {
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        exit(2);
    }

    switch (opt){
        // rc = -2 represents whether or not buffer was actually successfully copied through
        // From previously
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d\n", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            rc = reverse_string(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error reversing string, rc = %d\n", rc);
                exit(2);
            }
            break;
        case 'w':
            rc = print_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error printing words, rc = %d\n", rc);
                exit(2);
            }
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          Buffer overflow prevention is accounted for when having an explicit buffer parameter. 
//          This allows for developers to not accidentally write beyond array bounds.
//          It is also good for code reusability as it taks away any hard-coded assumptions about the buffer.
