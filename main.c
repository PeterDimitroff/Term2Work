#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct msg_s
{
    char *date;
    char *client;
    char *protocol;
    char *user;
    char *text;
    struct msg_s *next;
} msg;

msg *Menu(msg *head);
void memfree(msg *head);
msg *readTextFile(msg *head);
char *getline(msg *head);
char *readline(FILE *fp, char endC, msg *head);
void printAll(msg *head);
void errexit(msg *head);

void main()
{
    msg *head = NULL;
    for(;;)
        head = Menu(head);
    memfree(head);
}

void errexit(msg *head)
{
    printf("Error.\n");
    memfree(head);
    exit(1);
}

char *getline(msg *head)
{
    char *string;
    size_t len = 5;
    int cnt = 0;
    char c;

    if(!( string = (char*) malloc(5) ))
        errexit(head);

    while((c = getchar()) != '\n')
    {
        if(cnt >= len -1)
            string = (char *) realloc(string, (len += 5));
            string[cnt++] = c;
    }
    string[cnt] = '\0';
    return string;
}

msg *Menu(msg *head)
{
    char choice = ' ';
    printf("\n***\nEnter:\n1 - to read the file.\n");
    printf("2 - to exit.\n");
    do
    {
        choice = getchar();
        fflush(stdin);
    }
    while(choice < '1' || choice > '2' );

    switch(choice)
    {
    case '1':
        memfree(head);
        head = readTextFile(head);
        break;
    case '2':
        memfree(head);
        exit(0);
    }

    return head;
}

void printAll(msg *head)
{
    printf("\n***\n");
    while(head != NULL)
    {
        printf("[%s][%s.%s]%s:%s\n", head->date, head->client, head->protocol, head->user, head->text);
        head = head->next;
    }
        system("PAUSE");
}

msg *readTextFile(msg *head)
{
    msg *node = NULL;
    FILE *fp;
    char *fname;
    char *date;
    char *client;
    char *protocol;
    char *user;
    char *text;
    memfree(head);
    printf("Enter file name:\n");
    fname = getline(head);
    if( !(fp = fopen(fname, "r")) )
        errexit(head);
    free(fname);
    while(1)
    {
        if(fseek(fp, 1, SEEK_CUR))
            errexit(head);

        date = readline(fp, ']', head);
        if(!date)
            break;

        if(fseek(fp, 1, SEEK_CUR))
            errexit(head);

        client = readline(fp, '.', head);
        protocol = readline(fp, ']', head);
        user = readline(fp, ':', head);
        text = readline(fp, '\n', head);

        if(head == NULL)
        {
            node = (msg*) malloc(sizeof(msg));
            head = node;
        }
        else
        {
            node->next = (msg*) malloc(sizeof(msg));
            node = node->next;
        }
        node->date = date;
        node->client = client;
        node->protocol = protocol;
        node->user = user;
        node->text = text;
        node->next = NULL;

    }
    fclose(fp);
    fp = NULL;
    printAll(head);
    return head;
}

char *readline(FILE *fp, char endC, msg *head)
{
    char *string;
    size_t len = 10;
    int cnt = 0;
    char c;
    if(!( string = (char*) malloc(len) ))
    {
        errexit(head);
    }

    while((c = fgetc(fp)) != endC)
    {
        if(c == EOF)
        {
            free(string);
            return NULL;
        }

        if(cnt>=len-1)
            string = (char *) realloc(string, (len += 10));
        string[cnt++] = c;
    }

    string[cnt] = '\0';
    return string;
}

void memfree(msg *head)
{
    msg *p = head;
    while(head != NULL)
    {
        head = head->next;
        free(p->client);
        free(p->protocol);
        free(p->user);
        free(p->text);
        free(p);
        p = head;
    }
}
