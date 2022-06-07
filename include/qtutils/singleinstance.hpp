#pragma once

#include <QLockFile>
#include <memory>

namespace qu
{

class SingleInstance
{
public:
    SingleInstance(const QString &applicationName);

    bool isAnotherInstanceRunning();

    const QString &error() const;

private:
    Q_DISABLE_COPY(SingleInstance)

    std::unique_ptr<QLockFile> m_lockFile;
    QString m_errorMessage;
};

} // namespace qu
