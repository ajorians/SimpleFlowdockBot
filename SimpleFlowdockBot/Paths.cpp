#include "Paths.h"

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __linux__ 
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#endif

#ifndef NULL
#define NULL   0
#endif

std::string GetSelfPath()
{
#ifdef WIN32
   char szPath[MAX_PATH];
   GetModuleFileNameA(NULL, (LPSTR)&szPath, MAX_PATH);
   return szPath;
#else
   char result[ PATH_MAX ];
   ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
   return std::string( result, (count > 0) ? count : 0 );
#endif
}

std::string GetSelfDirectory()
{
   std::string strPath = GetSelfPath();
#ifdef WIN32
   std::string strPathDirectory = strPath.substr(0, strPath.find_last_of('\\'));
   strPathDirectory += "\\";
#else
   std::string strPathDirectory = strPath.substr(0, strPath.find_last_of('/'));
   strPathDirectory += "/";
#endif
   return strPathDirectory;
}

