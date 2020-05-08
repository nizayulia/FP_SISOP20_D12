#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

char buf[512];

// dapatkan nama file
char *getFileName(char *asal) 
{
	char *filename = asal;
	char *temp = asal;
	int i;
	for (i = strlen(temp); i >= 0; i--) {
		if (temp[i] == '/') {
			filename = &temp[i+1];
			break;
		}
	}
	return filename;
}

// fungsi untuk menggabungkan 2 string
char* strcat(char *tujuan,char *asal)
{
    char *temp=tujuan;
    while(*tujuan) ++tujuan;
    while(*asal) *tujuan++=*asal++;
    *tujuan=0;
    return temp;
}

// cek jika asal adalah directory
int isDirectory(char *dir) {
	struct stat st;
	int result;
	int fd = open(dir, O_RDONLY);
	fstat(fd, &st);
	if (st.type == T_DIR) 
		result = 1;
	else
		result = 0;
	close(fd);
	return result;
}

void copy_file(char *asal, char *tujuan) 
{
  
  int n;
	if (!isDirectory(asal)) 
	{
		char *tujuanBaru = (char *) malloc(strlen(getFileName(asal)) + strlen(tujuan) + 2);
		strcpy(tujuanBaru, tujuan);
		// cek tujuan merupakan folder atau bukan
		if (isDirectory(tujuan)) 
		{
			if (tujuan[strlen(tujuan) - 1] != '/') 
			strcat(tujuanBaru, "/");
			strcat(tujuanBaru, getFileName(asal));
		}
		else if (tujuan[strlen(tujuan)-1] == '/') 
		{
			printf(1, "cp: %s is not a directory\n", tujuan);
			return;
		}

    int fd0 = open(asal, O_RDONLY);
		if (fd0 < 0) 
		{
			printf(1, "cp: cannot open %s\n", asal);
			return;
		}
    int fd1 = open(tujuanBaru,O_RDWR | O_CREATE );
		if (fd1 < 0) 
		{
			printf(1, "cp: cannot open %s\n", tujuan);
			return;
		}

		while ((n = read(fd0, buf, sizeof(buf))) > 0) 
			write(fd1, buf, n);
	close(fd0);
	close(fd1);
	}
}

void copy_R(char *asal, char *tujuan) 
{
  char *p;
  char buf[512];
  char buf1[512];
  struct dirent de;
  
  if (isDirectory(asal))
  {  
    int fd = open(asal, O_RDONLY);
    if(fd < 0)
    {
      printf(1, "cp: cannot open %s\n", asal);
      return;
    }

    strcpy(buf1, tujuan);
    if (tujuan[strlen(tujuan) - 1] != '/') 
      strcat(buf1, "/");
    strcat(buf1, getFileName(asal));
    
    if (mkdir(buf1) < 0) 
    {
      mkdir(tujuan);
      strcpy(buf1, tujuan);
    }
    strcpy(buf, asal);

    p = buf + strlen(buf);
    *p = '/';
    p++;

    while(read(fd, &de, sizeof(de)) == sizeof(de)) 
    {
      if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0 )
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if (isDirectory(buf))
        copy_R(buf, buf1);
      else
        copy_file(buf, buf1);
    }
  close(fd);
  }

}

void copy_all(char *tujuan) 
{
  copy_R(".", tujuan);
}

int main(int argc, char *argv[]) 
{

  if (argc <= 2 ) 
  {
    printf(2, "cp: missing destination file operand after %s\n", argv[1]);
    exit();
  }
  if(isDirectory(argv[1]))
    printf(1,"copy folder pakai -R\n");

  if (strcmp(argv[1], "*") == 0)
    copy_all(argv[2]);
  else if (strcmp(argv[1], "-r") == 0)
    { 
	  copy_R(argv[2], argv[3]);
    }
  else
	  copy_file(argv[1], argv[2]);
  exit();
}