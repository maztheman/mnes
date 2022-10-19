#pragma once

#include <filesystem>


class FileBrowser
{
public:
    FileBrowser()
    : m_CurrentPath(std::filesystem::current_path())
    {

    }

    std::filesystem::path getCurrentPath() const
    {
        return m_CurrentPath;
    }

    const std::vector<std::filesystem::directory_entry>& getFilesFromCurrent();

    void moveTo(const std::filesystem::directory_entry& dir);


private:

    std::filesystem::path m_CurrentPath;
    std::filesystem::path m_LastPath;
    std::vector<std::filesystem::directory_entry> m_Files;
};