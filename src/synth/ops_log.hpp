#pragma once

#ifndef OPS_LOG_HPP
#define OPS_LOG_HPP

#if defined(OPS_GUI)

#include "resource.h"
#include <string>
#include <fstream>

#include "resource.h"
#include "ops_util.hpp"

#if defined(_DEBUG)
// TODO: handle debug log locations for supported platforms
#define DBG_LOG(x) OpsLog::Logger::Log("C:\\temp\\ops_debug.log", x);
#else
#define DBG_LOG(x)
#endif

namespace OpsLog
{

  class Logger
  {
  public:

    Logger(const std::string logger_name, const std::string log_filename)
      : m_name(logger_name), m_filename(log_filename)
    {
      m_active = false;
      // Check that the directories in the log file path exist or create if they do not.
      try
      {
        auto logFilename = OpsUtil::getDirectoryName(log_filename);
        if (!logFilename.empty())
        {
          OpsUtil::createDirectories(logFilename);
          m_logfile.open(log_filename, std::ios::out | std::ios::app);
          if (m_logfile.is_open())
          {
            m_active = true;
          }
        }
      }
      catch (const std::runtime_error)
      {
        // Can't create directories to log to, exit silently
      }
    }

    void Log(const std::string &msg)
    {
      if (!m_active || !m_logfile.is_open())
      {
        return;
      }

      m_logfile << msg << std::endl;
    }

    //----------------------------------------------------------------------------
    // A naive and slow logging function for logging without a logger object.
    // Will attempt to create the directories and the log file if they do not exist,
    // exiting silently if any step fails.
    static void Log(const std::string &filename, const std::string &msg)
    {
      std::string log_base_path = OpsUtil::getDirectoryName(filename);

      try
      {
        // Check that the path exists and create if it does not
        OpsUtil::createDirectories(log_base_path);
      }
      catch (const std::runtime_error)
      {
        // Can't create directories to log to, exit
        return;
      }

      std::ofstream log_file;
      log_file.open(filename, std::ios::out | std::ios::app);
      if (log_file.is_open())
      {
        log_file << msg << std::endl;
        log_file.close();
      }
    }

  private:
    std::ofstream m_logfile;
    bool m_active;
    std::string m_name;
    std::string m_filename;
  };

}

#endif

#endif