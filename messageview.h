#pragma once

#include <QWebEngineView>

class MessageView : public QWebEngineView
{
	Q_OBJECT
public:
	MessageView(QWidget *parent = nullptr);

	void setContentFile(const QString& fname);

};
