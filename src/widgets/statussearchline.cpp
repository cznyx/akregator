/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "statussearchline.h"
#include <KLocalizedString>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QDebug>

using namespace Akregator;

StatusSearchLine::StatusSearchLine(QWidget *parent)
    : KLineEdit(parent),
      mSearchLineStatusAction(Q_NULLPTR)
{
    initializeHash();
    //initializeActions();
}

StatusSearchLine::~StatusSearchLine()
{

}

void StatusSearchLine::initializeHash()
{
    const QIcon iconAll = QIcon::fromTheme(QStringLiteral("system-run"));
    const QIcon iconNew = QIcon::fromTheme(QStringLiteral("mail-mark-unread-new"));
    const QIcon iconUnread = QIcon::fromTheme(QStringLiteral("mail-mark-unread"));
    const QIcon iconKeep = QIcon::fromTheme(QStringLiteral("mail-mark-important"));
    mHashStatus.insert(AllArticles, iconAll);
    mHashStatus.insert(NewArticles, iconNew);
    mHashStatus.insert(UnreadArticles, iconUnread);
    mHashStatus.insert(ImportantArticles, iconKeep);
}

void StatusSearchLine::setStatus(StatusSearchLine::Status status)
{
    updateStatusIcon(status);
}

void StatusSearchLine::initializeActions()
{
    QIcon iconAll = QIcon::fromTheme(QStringLiteral("system-run"));
    //const QIcon iconNew = QIcon::fromTheme(QStringLiteral("mail-mark-unread-new"));
    //const QIcon iconUnread = QIcon::fromTheme(QStringLiteral("mail-mark-unread"));
    //const QIcon iconKeep = QIcon::fromTheme(QStringLiteral("mail-mark-important"));
    mSearchLineStatusAction = addAction(iconAll, QLineEdit::LeadingPosition);
    connect(mSearchLineStatusAction, &QAction::triggered, this, &StatusSearchLine::showMenu);
}

void StatusSearchLine::showMenu()
{
    QMenu p(this);
    p.addAction(i18n("Read"));
    p.exec(mapToGlobal(QPoint(0, height())));
}

void StatusSearchLine::updateStatusIcon(StatusSearchLine::Status status)
{
    switch(status) {
    case AllArticles:
        break;
    case NewArticles:
        break;
    case UnreadArticles:
        break;
    case ImportantArticles:
        break;
    }
}
