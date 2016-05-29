///[DD.MM.YYYY HH:MM:SS] [<Chat Client>.<protocol>]<UserName>: <Message>
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

typedef struct usr_s
{
    char *name;
    int symbols;
    int messages;
    int happy;
    int sad;
    double mood;
    struct usr_s *next;
} usr;

msg *mHead = NULL;
usr *uHead = NULL;

void Menu();
void memfree();
void readTextFile();
char *getline();
char *readline(FILE *fp, char endC);
void printMsgList();
void writeBin();
FILE *writeMsg(FILE *fp, msg *node);
void scanUsers();
void errexit();
void setmood(usr *node, char *text);
void printUserMood();
void mostMessages();
void mostSymbols();
void deleteUser();
FILE *compareWriting(FILE *tmp, char *date, char *client, char *protocol, char *name, char *text, char *searchedName);

void main()
{
    int i;
    for(i=0;;i++)
    {
        if(i)
            system("PAUSE");
        Menu();
    }
    memfree();
}

void errexit()
{
    printf("Error.\n");
    memfree();
    exit(1);
}

char *getline()
{
    char *string;
    size_t len = 5;
    int cnt = 0;
    char c;

    if(!( string = (char*) malloc(5) ))
        errexit();

    while((c = getchar()) != '\n')
    {
        if(cnt >= len -1)
            string = (char *) realloc(string, (len += 5));
            string[cnt++] = c;
    }
    string[cnt] = '\0';

    return string;
}

void Menu()
{
    char choice = ' ';
    system("cls");
    printf("\n***\nEnter:\n1 - to read the file.\n2 - to create binary file.\n3 - to print all messages.\n");
    printf("4 - to read history.db and scan users.\n5 - to see a user's mood.\n");
    printf("6 - to see the user with most messages.\n7 - to see the user with most symbols.\n");
    printf("8 - to delete a user's messages from history.db.\n9 - to exit.\n");
    do
    {
        choice = getchar();
        fflush(stdin);
    }
    while(choice < '1' || choice > '9' );

    switch(choice)
    {
    case '1':
        memfree();
        readTextFile();
        break;
    case '2':
        writeBin();
        break;
    case '3':
        printMsgList();
        break;
    case '4':
        scanUsers();
        break;
    case '5':
        printUserMood();
        break;
    case '6':
        mostMessages();
        break;
    case '7':
        mostSymbols();
        break;
    case '8':
        deleteUser();
        break;
    case '9':
        memfree();
        exit(0);
    }
}

void printMsgList()
{
    msg *node = mHead;
    printf("\n***\n");
    if(!node)
    {
        printf("No messages found in memory.\n");
        return NULL;
    }
    while(node)
    {
        printf("[%s] [%s.%s] %s:%s\n", node->date, node->client, node->protocol, node->user, node->text);
        node = node->next;
    }
}

void printUserMood()
{
    usr *node = uHead;
    char * searchedName;
    if(!uHead)
    {
        printf("***No users found in memory! Scan history.db first!***\n");
        return NULL;
    }
    printf("Enter user to search for:\n");
    searchedName = getline();
    while(node && strcmp(node->name, searchedName))
    {
        node = node->next;
    }
    if(!node)
    {
        printf("***No such user found!***\n");
        return NULL;
    }
    if(!node->mood)
    {
        if(node->happy)
            printf("Mood - HAPPY!. %s has used %d happy faces and NO sad faces.\n", node->name, node->happy);
        if(node->sad)
            printf("Mood - SAD!. %s has used %d sad faces and NO happy faces.\n", node->name, node->sad);
        if(!node->happy && !node->sad)
            printf("No sad or happy faces used at all.\n");
    }
    else
    {
        if(node->mood > 1.5)
            printf("Mood - happy! (%g)\n", node->mood);
        if(node->mood < 0.5)
            printf("Mood - sad. (%g)\n", node->mood);
        if(node->mood <= 1.5 && node->mood >= 0.5)
            printf("Mood - neutral. (%g)\n", node->mood);
    }
}

void readTextFile()
{
    msg *node = NULL;
    FILE *fp;
    char *fname;
    char *date;
    char *client;
    char *protocol;
    char *user;
    char *text;

    memfree();
    printf("Enter file name:\n");
    fname = getline();
    if( !(fp = fopen(fname, "r")) )
    {
        printf("***File not found!***\n");
        return NULL;
    }
    while(1)
    {
        if(fseek(fp, 1, SEEK_CUR))
            errexit();

        date = readline(fp, ']');
        if(!date)
            break;

        if(fseek(fp, 2, SEEK_CUR))
            errexit();

        client = readline(fp, '.');
        protocol = readline(fp, ']');
        if(fseek(fp, 1, SEEK_CUR))
            errexit();
        user = readline(fp, ':');
        if(fseek(fp, 1, SEEK_CUR))
            errexit();
        text = readline(fp, '\n');

        if(!mHead)
        {
            node = (msg*) malloc(sizeof(msg));
            mHead = node;
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
    printf("%s successfully read.\n", fname);
    free(fname);
}

char *readline(FILE *fp, char endC)
{
    char *string;
    size_t len = 10;
    int cnt = 0;
    char c;
    if(!( string = (char*) malloc(len) ))
        errexit();

    while((c = fgetc(fp)) != endC)
    {
        if(c == EOF)
        {
            if(!cnt)
            {
                free(string);
                return NULL;
            }
            else
            {
                if(cnt>=len-1)
                    string = (char *) realloc(string, (len += 10));
                string[cnt++] = c;
                string[cnt] = '\0';
                return string;
            }
        }

        if(cnt>=len-1)
            string = (char *) realloc(string, (len += 10));
        string[cnt++] = c;
    }

    string[cnt] = '\0';
    return string;
}

void writeBin()
{
    FILE *fp;
    msg *node = mHead;

    if(!mHead)
    {
        printf("No messages found in memory.\n");
        return NULL;
    }

    if((fp = fopen("history.db", "wb"))==NULL)
        errexit();

    while(node)
    {
        fp = writeMsg(fp, node);
        node = node->next;
    }
    fclose(fp);
    printf("\nhistory.db created successfuly!\n");
}

FILE *writeMsg(FILE *fp, msg *node)
{
    size_t len;

    len = strlen(node->date);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit();
    if(fwrite(node->date, len, 1, fp)!=1)
        errexit();

    len = strlen(node->client);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit();
    if(fwrite(node->client, len, 1, fp)!=1)
        errexit();

    len = strlen(node->protocol);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit();
    if(fwrite(node->protocol, len, 1, fp)!=1)
        errexit();

    len = strlen(node->user);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit();
    if(fwrite(node->user, len, 1, fp)!=1)
        errexit();

    len = strlen(node->text);
    if(fwrite(&len, sizeof(size_t), 1, fp)!=1)
        errexit();
    if(fwrite(node->text, len, 1, fp)!=1)
        errexit();
    return fp;
}

void scanUsers()    ///reads history.db FORMAT: |int|string|int|string|int|string|int|string|int|string|
{
    FILE *fp;
    size_t len;
    usr *node;
    char *name;
    char *text;

    memfree();
    if(!(fp = fopen("history.db", "rb")))
    {
        printf("Can't open history.db.\n");
        return NULL;
    }
    while(1)
    {
        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            {
                if(ferror(fp))
                    errexit();
                break;
            }
        if(fseek(fp, len, SEEK_CUR))
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(fseek(fp, len, SEEK_CUR))
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(fseek(fp,len, SEEK_CUR))
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(!(name = (char*) calloc(1, len+1)))
           errexit();
        if(fread(name, len, 1, fp)!=1)
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(!(text = (char*) calloc(1, len+1)))
           errexit();
        if(fread(text, len, 1, fp)!=1)
            errexit();

        if(!uHead)
        {
            node = (usr *) malloc(sizeof(usr));
            uHead = node;
            uHead->next = NULL;
            node->name = name;
            node->sad = node->happy = node->mood = node->messages = node->symbols = 0;
        }
        else
        {
            node = uHead;
            while(node && strcmp(node->name, name))
                node = node->next;
            if(!node)
            {
                node = (usr *) malloc(sizeof(usr));
                node->next = uHead;
                uHead = node;
                node->name = name;
                node->sad = node->happy = node->mood = node->messages = node->symbols = 0;
            }
        }
        node->messages++;
        node->symbols += strlen(text);
        setmood(node, text);
    }
    fclose(fp);
    printf("history.db scanned successfully.\n");
}

void setmood(usr *node, char *text) ///reads a message and adjusts the mood.
{
    int i;
    int fl = 0;
    for(i = 0; i < strlen(text); i++)
    {
        switch(text[i])
        {
            case ':':
                fl = 1;
                break;
            case '\'':
                if(fl == 1)
                    fl = 3;
                else
                    fl = 0;
                break;
            case '=':
                fl = 2;
                break;
            case ')':
                if(fl == 1)
                    node->happy++;
                fl = 0;
                break;
                case 'd':
                if(fl == 1)
                    node->happy++;
                fl = 0;
                break;
            case 'D':
                if(fl == 1)
                    node->happy++;
                fl = 0;
                break;
            case 'P':
                if(fl == 1)
                    node->happy++;
                fl = 0;
                break;
            case 'p':
                if(fl == 1)
                    node->happy++;
                fl = 0;
                break;
            case ']':
                if(fl == 2)
                    node->happy++;
                fl = 0;
                break;
            case '(':
                if(fl == 1 || fl == 2 || fl == 3)
                    node->sad++;
                fl = 0;
                break;
            default:
                fl = 0;
                break;
        }
    }
    if(node->happy && node->sad)
        node->mood = (double) node->happy/node->sad;
}

void mostMessages()
{
    usr *node = uHead;
    int maxMsg = 0;
    char *maxUser;
    if(!uHead)
    {
        printf("***No users found in memory! Scan history.db first!***\n");
        return NULL;
    }
    while(node)
    {
        if(maxMsg < node->messages)
        {
            maxMsg = node->messages;
            maxUser = node->name;
        }
        node = node->next;
    }
    printf("%s - %d messages!\n", maxUser, maxMsg);
}

void mostSymbols()
{
    usr *node = uHead;
    int maxSymbols = 0;
    char *maxUser;
    if(!uHead)
    {
        printf("***No users found in memory! Scan history.db first!***\n");
        return NULL;
    }
    while(node)
    {
        if(maxSymbols < node->symbols)
        {
            maxSymbols = node->symbols;
            maxUser = node->name;
        }
        node = node->next;
    }
    printf("%s - %d symbols!\n", maxUser, maxSymbols);
}

void deleteUser()   ///from history.db
{
    FILE *fp;
    FILE *tmp;
    char *searchedName;
    char *date;
    char *client;
    char *protocol;
    char *name;
    char *text;
    size_t len;

    if(!(fp = fopen("history.db", "rb")))
    {
        printf("Can't open history.db.\n");
        return NULL;
    }
    printf("User to delete - ");
    searchedName = getline();
    if((tmp = fopen("tmp.db", "wb"))==NULL)
        errexit();
    while(1)
    {
        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            {
                if(ferror(fp))
                    errexit();
                break;
            }
        if(!(date = (char*) calloc(1, len+1)))
           errexit();
        if(fread(date, len, 1, fp)!=1)
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(!(client = (char*) calloc(1, len+1)))
           errexit();
        if(fread(client, len, 1, fp)!=1)
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(!(protocol = (char*) calloc(1, len+1)))
           errexit();
        if(fread(protocol, len, 1, fp)!=1)
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(!(name = (char*) calloc(1, len+1)))
           errexit();
        if(fread(name, len, 1, fp)!=1)
            errexit();

        if(fread(&len, sizeof(size_t), 1, fp)!=1)
            errexit();
        if(!(text = (char*) calloc(1, len+1)))
           errexit();
        if(fread(text, len, 1, fp)!=1)
            errexit();

        tmp = compareWriting(tmp, date, client, protocol, name, text, searchedName);

        free(date);
        free(client);
        free(protocol);
        free(name);
        free(text);
    }
    fclose(fp);
    fclose(tmp);
    remove("history.db");
    rename("tmp.db", "history.db");
    printf("%s's messages successfully deleted.\n");
    free(searchedName);
    scanUsers();
}

FILE *compareWriting(FILE *tmp, char *date, char *client, char *protocol, char *name, char *text, char *searchedName)
{
    size_t len;
    if(strcmp(name, searchedName))
    {
        len = strlen(date);
        if(fwrite(&len, sizeof(size_t), 1, tmp)!=1)
            errexit();
        if(fwrite(date, strlen(date), 1, tmp)!=1)
            errexit();
        len = strlen(client);
        if(fwrite(&len, sizeof(size_t), 1, tmp)!=1)
            errexit();
        if(fwrite(client, strlen(client), 1, tmp)!=1)
            errexit();
        len = strlen(protocol);
        if(fwrite(&len, sizeof(size_t), 1, tmp)!=1)
            errexit();
        if(fwrite(protocol, strlen(protocol), 1, tmp)!=1)
            errexit();
        len = strlen(name);
        if(fwrite(&len, sizeof(size_t), 1, tmp)!=1)
            errexit();
        if(fwrite(name, strlen(name), 1, tmp)!=1)
            errexit();
        len = strlen(text);
        if(fwrite(&len, sizeof(size_t), 1, tmp)!=1)
            errexit();
        if(fwrite(text, strlen(text), 1, tmp)!=1)
            errexit();
    }
    return tmp;
}

void memfree()
{
    msg *p = mHead;
    usr *q = uHead;

    while(mHead != NULL)
    {
        mHead = mHead->next;
        free(p->date);
        free(p->client);
        free(p->protocol);
        free(p->user);
        free(p->text);
        free(p);
        p = mHead;
    }
    mHead = NULL;

    while(uHead != NULL)
    {
        uHead = uHead->next;
        free(q->name);
        free(q);
        q = uHead;
    }
    uHead = NULL;
}
