#include "RootController.h"

#include "BuildOptions.h"
#include "DeclarativeView.h"

#include <QDebug>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>

RootController::RootController()
{
	
}

void RootController::presentQml(const QUrl& url)
{
	DeclarativeView *view = new DeclarativeView(url);
	view->setAutoReload(DEVELOPER_MODE);
	presentWidget(view, true);
	view->setResizeMode(QDeclarativeView::SizeRootObjectToView);
	view->engine()->rootContext()->setContextProperty("rootController", this);
}

int RootController::presentDialog(QDialog *dialog)
{
	setDismissable(false);
	constrain(dialog);
	const int ret = dialog->exec();
	setDismissable(true);
	return ret;
}

void RootController::presentWidget(QWidget *widget, bool owns)
{
	m_ownership[widget] = owns;
	QWidget *prev = m_stack.size() ? m_stack.top() : 0;
	m_stack.push(widget);
	constrain(widget);
	if(prev) widget->move(prev->pos());
	present(widget);
	if(prev) prev->hide();
}

void RootController::dismissWidget()
{
	if(!m_dismissable) return;
	QWidget *widget = m_stack.pop();
	QWidget *next = m_stack.size() ? m_stack.top() : 0;
	if(next) next->move(widget->pos());
	present(next);
	widget->hide();
	if(m_ownership.value(widget)) widget->deleteLater();
	m_ownership.remove(widget);
}

void RootController::dismissAllWidgets()
{
	if(!m_dismissable) return;
	while(m_stack.size() > 1) dismissWidget();
}

const unsigned int RootController::depth() const
{
	return m_stack.size();
}

void RootController::setDismissable(bool dismissable)
{
	m_dismissable = dismissable;
}

bool RootController::isDismissable() const
{
	return m_dismissable;
}

void RootController::constrain(QWidget *widget)
{
	widget->setMinimumSize(320, 240);
	widget->setMaximumSize(320, 240);
}

void RootController::present(QWidget *widget)
{
	if(!widget) return;
	widget->show();
	widget->raise();
}