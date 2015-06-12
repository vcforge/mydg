#include "StdAfx.h"
#include "config.h"

Config * Config::m_instance = NULL;

void config_err(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args); 
}
void config_warn(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

void Config::create()
{
  if (m_initialized) return; 
  m_initialized = TRUE;
//  addConfigOptions(this);
}

bool &Config::getBool(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_Bool)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of boolean type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigBool *)opt)->valueRef();
}

QCString &Config::getString(const char *fileName,int num,const char *name) const
{
  ConfigOption *opt = m_dict->find(name);
  if (opt==0) 
  {
    config_err("%s<%d>: Internal error: Requested unknown option %s!\n",fileName,num,name);
    exit(1);
  }
  else if (opt->kind()!=ConfigOption::O_String)
  {
    config_err("%s<%d>: Internal error: Requested option %s not of string type!\n",fileName,num,name);
    exit(1);
  }
  return *((ConfigString *)opt)->valueRef();
}
