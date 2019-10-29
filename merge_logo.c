#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>

typedef struct {
    int32_t width;
    int32_t height;
    int32_t offset;
    int32_t filesize;
    char    filename[PATH_MAX];
} ITEM;

static int str_starts_with(const char *str1, const char *str2)
{
    return !memcmp(str1, str2, strlen(str2));
}

static int str_ends_with(const char *str1, const char *str2)
{
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if (len1 < len2) return 0;
    return !memcmp(str1 + len1 - len2, str2, len2);
}

static int get_file_size(const char *file)
{
    FILE *fp = fopen(file, "rb");
    int   len;
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fclose(fp);
    return len;
}

static void parse_width_height(const char *str, int *w, int *h)
{
    char temp[PATH_MAX];
    int  i;

    strncpy(temp, str, sizeof(temp));
    for (i=0; i<sizeof(temp) && temp[i]; i++) {
        if (temp[i] < '0' || temp[i] > '9') temp[i] = ' ';
    }
    sscanf(temp, "%d %d", w, h);
}

int main(int argc, char *argv[])
{
    char    path  [PATH_MAX] = ".";
    char    prefix[PATH_MAX] = "bootlogo";
    char    file  [PATH_MAX] = "";
    ITEM    items [256]      = {};
    int32_t offset = 0, n = 0, i, j;
    DIR    *pdir;
    struct  dirent *pent;
    FILE   *fp;

    if (argc > 2) strncpy(prefix, argv[2], sizeof(prefix));
    if (argc > 1) strncpy(path  , argv[1], sizeof(path  ));
//  printf("path   = %s\n", path  );
//  printf("prefix = %s\n", prefix);

    pdir = opendir(path);
    if (!pdir) {
        printf("failed to open dir: %s\n", path);
        return -1;
    }
    while ((pent = readdir(pdir)) != NULL) {
        if (pent->d_type == DT_REG || pent->d_type == DT_LNK) {
//          printf("pent->d_name = %s\n", pent->d_name);
            if (str_starts_with(pent->d_name, prefix) && str_ends_with(pent->d_name, ".bmp")) {
                if (n < 256) {
                    parse_width_height(pent->d_name, &items[n].width, &items[n].height);
                    snprintf(items[n].filename, sizeof(items[n].filename), "%s/%s", path, pent->d_name);
                    items[n].filesize = get_file_size(items[n].filename);
                    n++;
                }
            }
        }
    }
    closedir(pdir);

    if (n == 0) {
        printf("can't find %s*.bmp files !\n", prefix);
        return -1;
    }

    offset = sizeof(int32_t) + n * sizeof(int32_t) * 3;
    for (i=0; i<n; i++) {
        items[i].offset = offset;
        offset += items[i].filesize;
//      printf("width: %d, height: %d, offset: %d, filesize: %d, filename: %s\n",
//          items[i].width, items[i].height, items[i].offset, items[i].filesize, items[i].filename);
    }

    snprintf(file, sizeof(file), "%s/%s.bin", path, prefix);
    fp = fopen(file, "wb");
    if (!fp) {
        printf("failed to open output file: %s !\n", file);
        return -1;
    }

    fwrite(&n, sizeof(n), 1, fp);
    for (i=0; i<n; i++) {
        fwrite(&items[i].width , sizeof(items[i].width ), 1, fp);
        fwrite(&items[i].height, sizeof(items[i].height), 1, fp);
        fwrite(&items[i].offset, sizeof(items[i].offset), 1, fp);
    }

    for (i=0; i<n; i++) {
        FILE *srcfp = fopen(items[i].filename, "rb");
        if (srcfp) {
            for (j=0; j<items[i].filesize; j++) {
                fputc(fgetc(srcfp), fp);
            }
            fclose(srcfp);
        } else {
            fseek(fp, items[i].filesize, SEEK_CUR);
        }
    }

    fclose(fp);
    printf("pack logo bmp files ok !\n");
    return 0;
}
