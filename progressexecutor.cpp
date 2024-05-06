#include "progressexecutor.h"

ProgressExecutor::ProgressExecutor(QObject *parent) : Executor(parent)
{
    pprd = new ProgressReportDialog();
    connect(this,SIGNAL(started()),pprd,SLOT(show()));
    connect(this,SIGNAL(finished()),pprd,SLOT(hide()));
}

ProgressExecutor::~ProgressExecutor()
{
    delete pprd;
}
