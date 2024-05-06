#ifndef PROGRESSEXECUTOR_H
#define PROGRESSEXECUTOR_H

#include "db/executor.h"
#include "progressreportdialog.h"

class ProgressExecutor : public Executor
{
    Q_OBJECT
public:
    ProgressExecutor(QObject *parent);
    ~ProgressExecutor();
private:
    ProgressReportDialog *pprd;
};

#endif // PROGRESSEXECUTOR_H
