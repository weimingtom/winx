/* 7zMain.c - Test application for 7z Decoder
2009-04-04 : Igor Pavlov : Public domain */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../7zCrc.h"
#include "../../7zFile.h"
#include "../../7zVersion.h"

#include "7zAlloc.h"
#include "7zExtract.h"
#include "7zIn.h"

#ifndef USE_WINDOWS_FILE
/* for mkdir */
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <errno.h>
#endif
#endif

#ifdef _WIN32
#define CHAR_PATH_SEPARATOR '\\'
#else
#define CHAR_PATH_SEPARATOR '/'
#endif

static WRes MyCreateDir(const char *name)
{
  #ifdef USE_WINDOWS_FILE
  return CreateDirectoryA(name, NULL) ? 0 : GetLastError();
  #else
  #ifdef _WIN32
  return _mkdir(name)
  #else
  return mkdir(name, 0777)
  #endif
  == 0 ? 0 : errno;
  #endif
}


static void ConvertNumberToString(UInt64 value, char *s)
{
  char temp[32];
  int pos = 0;
  do
  {
    temp[pos++] = (char)('0' + (int)(value % 10));
    value /= 10;
  }
  while (value != 0);
  do
    *s++ = temp[--pos];
  while (pos > 0);
  *s = '\0';
}

#define PERIOD_4 (4 * 365 + 1)
#define PERIOD_100 (PERIOD_4 * 25 - 1)
#define PERIOD_400 (PERIOD_100 * 4 + 1)

static void ConvertFileTimeToString(const CNtfsFileTime *ft, char *s)
{
  unsigned year, mon, day, hour, min, sec;
  UInt64 v64 = ft->Low | ((UInt64)ft->High << 32);
  Byte ms[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  unsigned temp;
  UInt32 v;
  v64 /= 10000000;
  sec = (unsigned)(v64 % 60);
  v64 /= 60;
  min = (unsigned)(v64 % 60);
  v64 /= 60;
  hour = (unsigned)(v64 % 24);
  v64 /= 24;

  v = (UInt32)v64;

  year = (unsigned)(1601 + v / PERIOD_400 * 400);
  v %= PERIOD_400;

  temp = (unsigned)(v / PERIOD_100);
  if (temp == 4)
    temp = 3;
  year += temp * 100;
  v -= temp * PERIOD_100;

  temp = v / PERIOD_4;
  if (temp == 25)
    temp = 24;
  year += temp * 4;
  v -= temp * PERIOD_4;

  temp = v / 365;
  if (temp == 4)
    temp = 3;
  year += temp;
  v -= temp * 365;

  if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
    ms[1] = 29;
  for (mon = 1; mon <= 12; mon++)
  {
    unsigned s = ms[mon - 1];
    if (v < s)
      break;
    v -= s;
  }
  day = (unsigned)v + 1;
  sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);
}

void PrintError(char *sz)
{
  printf("\nERROR: %s\n", sz);
}

int MY_CDECL main(int numargs, char *args[])
{
  CFileInStream archiveStream;
  CLookToRead lookStream;
  CSzArEx db;
  SRes res;
  ISzAlloc allocImp;
  ISzAlloc allocTempImp;
  char *temp = NULL;
  size_t tempSize = 0;

  printf("\n7z ANSI-C Decoder " MY_VERSION_COPYRIGHT_DATE "\n\n");
  if (numargs == 1)
  {
    printf(
      "Usage: 7zDec <command> <archive_name>\n\n"
      "<Commands>\n"
      "  e: Extract files from archive (without using directory names)\n"
      "  l: List contents of archive\n"
      "  t: Test integrity of archive\n"
      "  x: eXtract files with full paths\n");
    return 0;
  }
  if (numargs < 3)
  {
    PrintError("incorrect command");
    return 1;
  }

  if (InFile_Open(&archiveStream.file, args[2]))
  {
    PrintError("can not open input file");
    return 1;
  }

  
  FileInStream_CreateVTable(&archiveStream);
  LookToRead_CreateVTable(&lookStream, False);
  
  lookStream.realStream = &archiveStream.s;
  LookToRead_Init(&lookStream);

  allocImp.Alloc = SzAlloc;
  allocImp.Free = SzFree;

  allocTempImp.Alloc = SzAllocTemp;
  allocTempImp.Free = SzFreeTemp;

  CrcGenerateTable();

  SzArEx_Init(&db);
  res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
  if (res == SZ_OK)
  {
    char *command = args[1];
    int listCommand = 0, testCommand = 0, extractCommand = 0, fullPaths = 0;
    if (strcmp(command, "l") == 0) listCommand = 1;
    else if (strcmp(command, "t") == 0) testCommand = 1;
    else if (strcmp(command, "e") == 0) extractCommand = 1;
    else if (strcmp(command, "x") == 0) { extractCommand = 1; fullPaths = 1; }

    if (listCommand)
    {
      UInt32 i;
      for (i = 0; i < db.db.NumFiles; i++)
      {
        const CSzFileItem *f = db.db.Files + i;
        char s[32], t[32];
        ConvertNumberToString(f->Size, s);
        if (f->MTimeDefined)
          ConvertFileTimeToString(&f->MTime, t);
        else
          strcpy(t, "                   ");

        printf("%s %10s  %s", t, s, f->Name);
        if (f->IsDir)
          printf("/");
        printf("\n");
      }
    }
    else if (testCommand || extractCommand)
    {
      UInt32 i;

      /*
      if you need cache, use these 3 variables.
      if you use external function, you can make these variable as static.
      */
      UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
      Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
      size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */

      for (i = 0; i < db.db.NumFiles; i++)
      {
        size_t offset;
        size_t outSizeProcessed;
        const CSzFileItem *f = db.db.Files + i;
        if (f->IsDir && !fullPaths)
          continue;
        printf(testCommand ?
            "Testing   ":
            "Extracting");
        printf(" %s", f->Name);
        if (f->IsDir)
          printf("/");
        else
        {
          res = SzAr_Extract(&db, &lookStream.s, i,
              &blockIndex, &outBuffer, &outBufferSize,
              &offset, &outSizeProcessed,
              &allocImp, &allocTempImp);
          if (res != SZ_OK)
            break;
        }
        if (!testCommand)
        {
          CSzFile outFile;
          size_t processedSize;
          size_t j, nameLen = strlen(f->Name);
          const char *destPath;
          if (nameLen + 1 > tempSize)
          {
            SzFree(NULL, temp);
            tempSize = nameLen + 1;
            temp = (char *)SzAlloc(NULL, tempSize);
            if (temp == 0)
            {
              res = SZ_ERROR_MEM;
              break;
            }
          }
          destPath = temp;
          strcpy(temp, f->Name);
          for (j = 0; j < nameLen; j++)
            if (temp[j] == '/')
            {
              if (fullPaths)
              {
                temp[j] = 0;
                MyCreateDir(temp);
                temp[j] = CHAR_PATH_SEPARATOR;
              }
              else
                destPath = temp + j + 1;
            }
    
          if (f->IsDir)
          {
            MyCreateDir(destPath);
            printf("\n");
            continue;
          }
          else if (OutFile_Open(&outFile, destPath))
          {
            PrintError("can not open output file");
            res = SZ_ERROR_FAIL;
            break;
          }
          processedSize = outSizeProcessed;
          if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 ||
              processedSize != outSizeProcessed)
          {
            PrintError("can not write output file");
            res = SZ_ERROR_FAIL;
            break;
          }
          if (File_Close(&outFile))
          {
            PrintError("can not close output file");
            res = SZ_ERROR_FAIL;
            break;
          }
        }
        printf("\n");
      }
      IAlloc_Free(&allocImp, outBuffer);
    }
    else
    {
      PrintError("incorrect command");
      res = SZ_ERROR_FAIL;
    }
  }
  SzArEx_Free(&db, &allocImp);
  SzFree(NULL, temp);

  File_Close(&archiveStream.file);
  if (res == SZ_OK)
  {
    printf("\nEverything is Ok\n");
    return 0;
  }
  if (res == SZ_ERROR_UNSUPPORTED)
    PrintError("decoder doesn't support this archive");
  else if (res == SZ_ERROR_MEM)
    PrintError("can not allocate memory");
  else if (res == SZ_ERROR_CRC)
    PrintError("CRC error");
  else
    printf("\nERROR #%d\n", res);
  return 1;
}