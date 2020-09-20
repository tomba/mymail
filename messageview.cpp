#include "messageview.h"
#include <QFile>

MessageView::MessageView(QWidget* parent)
	: QWebEngineView(parent)
{

}

void MessageView::setContentFile(const QString& fname)
{
	QFile f(fname);
	f.open(QIODevice::ReadOnly);
	QByteArray content = f.readAll();

	setContent(content);
}
