#include "FileBrowser.h"

#include <algorithm>
#include <ranges>

const std::vector<std::filesystem::directory_entry>& FileBrowser::getFilesFromCurrent()
{
    if (m_LastPath == m_CurrentPath)
    {
        return m_Files;
    }
    
    m_LastPath = m_CurrentPath;

    m_Files.clear();
    for(auto entry : std::filesystem::directory_iterator(m_CurrentPath))
    {
        if (entry.is_directory() || entry.is_regular_file())
        {
            m_Files.push_back(entry);
        }
    }

    

    return m_Files;
}


void FileBrowser::moveTo(const std::filesystem::directory_entry& dir)
{
    if (dir.is_directory())
    {
        m_LastPath = m_CurrentPath;
        m_CurrentPath = dir;
    }
}