#ifndef ABCTEMPORARYFILE_H
#define ABCTEMPORARYFILE_H

#include <QTemporaryFile>

class AbcTemporaryFile : public QTemporaryFile
{
    Q_OBJECT
public:
    AbcTemporaryFile();

    bool open() { resize(0); return open(QIODevice::WriteOnly|QIODevice::Truncate); };

protected:
    bool open(OpenMode mode) { return QTemporaryFile::open(mode);};
};

#endif // ABCTEMPORARYFILE_H
