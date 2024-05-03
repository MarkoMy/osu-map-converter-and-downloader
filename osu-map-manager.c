#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <Windows.h>

void print_help() {
    printf("Ez az osu map converter!\n\n");
    printf("- Ha szeretned a mapjaidat lementeni txt-be, akkor -m2t\n");
    printf("- Ha szeretned, mar meglévo txt mentesbol beimportalni a mapjaidat, akkor -t2m\n");
    printf("\nRoviden:\n- Map to Text: -m2t\n- Text to Map: -t2m\n\n");
}

char * getPath() {
    char* path;
    printf("Add meg az osu mappa eleresi utvonalat: ");
    scanf("%s", path);
    return path;
}


void mapsToText(char* path) {
    DIR* dir;
    FILE *file;
    struct dirent *entry;
    char name[1024];
    char *deskpath;
    int length;

    deskpath = getenv("USERPROFILE");
    if (deskpath == NULL) { fprintf(stderr, "Hiba a felhasznalo konytaranak lekerdezesenel!\n"); exit(1); }
    strcat(deskpath,"\\Desktop\\maps.txt");

    printf("### filepath: %s\n", deskpath);
    printf("### dirpath: %s\n", path);

    file = fopen(deskpath, "w");
    if (file == NULL) { fprintf(stderr, "Fajl nem nyithato meg!\n"); exit(1);}

    dir = opendir(path);
    if (dir == NULL) { fprintf(stderr, "Hiba a mappa megnyitasa soran!\n"); exit(1); }

    while ((entry = readdir(dir)) != NULL) {
        printf("\n''%s''   ", entry->d_name);
        if (isdigit(entry->d_name[0])) {
            char* space_pos = strstr(entry->d_name, " ");
            if (space_pos == NULL) { length = strlen(entry->d_name); }
            else                   { length = space_pos - entry->d_name; }
            strncpy(name, entry->d_name, length);
            name[length] = '\0';
            fprintf(file, "%s\n", name);
            printf("### %s\n", name);
        }
    }
    closedir(dir);
    fclose(file);

    printf("\n\nmaps.txt letrehozva (%s)\n\n", deskpath);
}

void download_map(int current) {
    char command[1000];
    sprintf(command, "curl https://beatconnect.io/b/%d/ -o %d.osz\0", current, current);
    system(command);
}

void textToMaps(char* path) {
    FILE* file;
    int current, last;
    char line[50];

    char *deskpath = getenv("USERPROFILE");
    if (deskpath == NULL) { fprintf(stderr, "Hiba a felhasznalo konytaranak lekerdezesenel!\n"); exit(1); }
    strcat(deskpath,"\\Desktop\\maps.txt");

    file = fopen(deskpath, "r");
    if (file == NULL) { fprintf(stderr, "Fajl nem nyithato meg!\n"); exit(1);}

    chdir(path);
    printf("### pazh: %s\n",path);

    HANDLE thread;
    DWORD thread_id;
    while (fscanf(file, "%d", &current) == 1) {
        printf("### %d\n",current);

        thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)download_map, (LPVOID)current, 0, &thread_id);
        if (thread == NULL) { fprintf(stderr, "Szal letrehozása sikertelen!\n"); exit(1); }
        //WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);

        last = current;
    }
    fclose(file);

    sprintf(line,"%d.osz\0", last);
    system(line);
}


int main(int argc, char* argv[])
{
    int mode = -1;
    if (argc != 2)           { print_help();  exit(1); }
    if (strcmp(argv[1], "-m2t") == 0)      { mode = 1; }
    else if (strcmp(argv[1], "-t2m") == 0) { mode = 0; }
    else                      { print_help(); exit(1); }

    char* path = getPath();

    if (mode == -1) { print_help(); exit(1); }
    else if (mode)  { printf("### m2t\n"); mapsToText(path); }
    else if (!mode) { printf("### t2m\n"); textToMaps(path); }

    return 0;
}
