#include <stdio.h>              ///[DD.MM.YYYY HH:MM:SS] [<Chat Client>.<protocol>]<UserName>: <Message>
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
void printMsgList(msg *head);
void writeBin(msg *head);
void errexit(msg *head);
void writeMsg(FILE *fp, msg *node, msg *head);

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
    printf("2 - to create binary file.\n3 - to print all messages.\n");
    printf("4 - to exit.\n");
    do
    {
        choice = getchar();
        fflush(stdin);
    }
    while(choice < '1' || choice > '4' );

    switch(choice)
    {
    case '1':
        memfree(head);
        head = readTextFile(head);
        break;
    case '2':
        writeBin(head);
        break;
    case '3':
        printMsgList(head);
        break;
    case '4':
        memfree(head);
        exit(0);
    }

    return head;
}

void printMsgList(msg *head)
{
    printf("\n***\n");
    if(!head)
    {
        printf("No messages found in memory.\n");
        system("PAUSE");
        return NULL;
    }
    while(head != NULL)
    {
        printf("[%s] [%s.%s] %s:%s\n", head->date, head->client, head->protocol, head->user, head->text);
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

        if(fseek(fp, 2, SEEK_CUR))
            errexit(head);

        client = readline(fp, '.', head);
        protocol = readline(fp, ']', head);
        if(fseek(fp, 1, SEEK_CUR))
            errexit(head);
        user = readline(fp, ':', head);
        if(fseek(fp, 1, SEEK_CUR))
            errexit(head);
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

void writeBin(msg *head)
{
    FILE *fp;
    msg *node = head;

    if(!head)
    {
        printf("No messages found in memory.\n");
        system("PAUSE");
        return NULL;
    }

    if((fp = fopen("history.db", "wb"))==NULL)
        errexit(head);

    while(node)
    {
        writeMsg(fp, node, head);
        node = node->next;
    }
    fclose(fp);
    printf("\nhistory.db created successfuly!");
}

void writeMsg(FILE *fp, msg *node, msg *head)
{
    size_t len;

    len = strlen(node->date);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit(head);
    if(fwrite(node->date, len, 1, fp)!=1)
        errexit(head);

    len = strlen(node->client);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit(head);
    if(fwrite(node->client, len, 1, fp)!=1)
        errexit(head);

    len = strlen(node->protocol);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit(head);
    if(fwrite(node->protocol, len, 1, fp)!=1)
        errexit(head);

    len = strlen(node->user);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit(head);
    if(fwrite(node->user, len, 1, fp)!=1)
        errexit(head);

    len = strlen(node->text);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit(head);
    if(fwrite(node->text, len, 1, fp)!=1)
        errexit(head);
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
