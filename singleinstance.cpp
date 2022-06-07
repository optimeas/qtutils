#include "qtutils/singleinstance.hpp"

#include <QDir>
#include <QStandardPaths>
#include <QDebug>

namespace qu
{

SingleInstance::SingleInstance(const QString &applicationName)
{
    // The "TempLocation" is important for embedded devices:
    // - It has the advantage that everything definitely starts from the beginning when booting, and no remanences remain.
    // - Also to avoid time problems if RTC buffering fails and there is no internet connection to synchronize the clock.

    QString filename = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::TempLocation)) + QDir::separator() + applicationName + ".lock";
    m_lockFile = std::make_unique<QLockFile>(filename);

    if(!m_lockFile->tryLock(200))
    {
        if(m_lockFile->error() == QLockFile::LockFailedError)
        {
            // The lock file exist, but can not be locked
            qint64 pid;
            QString hostname;
            QString appname;

            if(m_lockFile->getLockInfo(&pid, &hostname, &appname))
                m_errorMessage = QString("another instance is running: PID %1, for details see lock file %2").arg(QString::number(pid), filename);

            else
                m_errorMessage = "another instance is running, but getLockInfo() failed to retrieve informations from lock file " + filename;
        }
        else
        {
            // The lock file can not be created etc.
            m_errorMessage = "failed to create lock file " + filename + " error: ";

            if(m_lockFile->error() == QLockFile::PermissionError)
                m_errorMessage.append("PermissionError");
            else
                m_errorMessage.append("UnknownError");
        }
    }
    else
        m_errorMessage = "No Error (only our instance is running)";
}

bool SingleInstance::isAnotherInstanceRunning()
{
    return !m_lockFile->isLocked();
}

const QString &SingleInstance::error() const
{
    return m_errorMessage;
}

} // namespace qu
