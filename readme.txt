merge_logo is a tool for pack logo files into one file.


how to use
----------
merge_logo file_path file_prefix
will pack all file_prefix*.bmp file under file_path directory.

the lcd width and height comes from file name, example:
bootlogo_800x480.bmp  is used for 800x480  lcd
bootlogo_1280x720.bmp is used for 1280x720 lcd


file format
-----------
typedef struct {
    int32_t width;   // lcd width
    int32_t height;  // lcd height
    int32_t offset;  // logo data offset in file
} logo_item_t;

type         name
uint32_t     logo_item_num (max value of logo_item_num is 256)
logo_item_t  logo item1
logo_item_t  logo item2
...
logo_item_t  logo itemn (n is logo_item_num)
binary       logo data1
binary       logo data2
...
binary       logo datan



---------
rockcarry
2019-11-5

