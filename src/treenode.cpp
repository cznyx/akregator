/*
    This file is part of Akregator.

    Copyright (C) 2004 Frank Osterfeld <osterfeld@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "treenode.h"
#include "folder.h"
#include "articlejobs.h"
#include "article.h"

#include <QPoint>
#include <QString>
#include <QList>

#include "akregator_debug.h"

#include <cassert>

using namespace Akregator;

class TreeNode::TreeNodePrivate
{
public:
    TreeNodePrivate();
    bool doNotify = false;
    bool nodeChangeOccurred = false;
    bool articleChangeOccurred = false;
    QString title;
    Folder *parent = nullptr;
    uint id;
    bool signalDestroyedEmitted = false;
    QPoint scrollBarPositions;
};

TreeNode::TreeNodePrivate::TreeNodePrivate() : doNotify(true)
    , nodeChangeOccurred(false)
    , articleChangeOccurred(false)
    , title()
    , parent(nullptr)
    , id(0)
    , signalDestroyedEmitted(false)
{
}

TreeNode::TreeNode()
    : QObject(nullptr)
    , d(new TreeNodePrivate)
{
}

void TreeNode::emitSignalDestroyed()
{
    if (!d->signalDestroyedEmitted) {
        if (parent()) {
            parent()->removeChild(this);
        }
        Q_EMIT signalDestroyed(this);
        d->signalDestroyedEmitted = true;
    }
}

TreeNode::~TreeNode()
{
    Q_ASSERT(d->signalDestroyedEmitted || !"TreeNode subclasses must call emitSignalDestroyed in their destructor");
    delete d;
    d = nullptr;
}

QString TreeNode::title() const
{
    return d->title;
}

void TreeNode::setTitle(const QString &title)
{
    if (d->title != title) {
        d->title = title;
        nodeModified();
    }
}

TreeNode *TreeNode::nextSibling()
{
    if (!d->parent) {
        return nullptr;
    }
    const QList<TreeNode *> children = parent()->children();
    const int idx = children.indexOf(this);

    return (idx + 1 < children.size()) ? children.at(idx + 1) : nullptr;
}

const TreeNode *TreeNode::nextSibling() const
{
    if (!d->parent) {
        return nullptr;
    }
    const QList<const TreeNode *> children = parent()->children();
    const int idx = children.indexOf(this);

    return (idx + 1 < children.size()) ? children.at(idx + 1) : nullptr;
}

TreeNode *TreeNode::prevSibling()
{
    if (!d->parent) {
        return nullptr;
    }
    const QList<TreeNode *> children = parent()->children();

    const int idx = children.indexOf(this);
    return (idx > 0) ? children.at(idx - 1) : nullptr;
}

const TreeNode *TreeNode::prevSibling() const
{
    if (!d->parent) {
        return nullptr;
    }
    const QList<const TreeNode *> children = parent()->children();
    const int idx = children.indexOf(this);
    return (idx > 0) ? children.at(idx - 1) : nullptr;
}

const Folder *TreeNode::parent() const
{
    return d->parent;
}

Folder *TreeNode::parent()
{
    return d->parent;
}

QList<const TreeNode *> TreeNode::children() const
{
    return QList<const TreeNode *>();
}

QList<TreeNode *> TreeNode::children()
{
    return QList<TreeNode *>();
}

const TreeNode *TreeNode::childAt(int pos) const
{
    Q_UNUSED(pos)
    return nullptr;
}

TreeNode *TreeNode::childAt(int pos)
{
    Q_UNUSED(pos)
    return nullptr;
}

void TreeNode::setParent(Folder *parent)
{
    d->parent = parent;
}

void TreeNode::setNotificationMode(bool doNotify)
{
    if (doNotify && !d->doNotify) { // turned on
        d->doNotify = true;
        if (d->nodeChangeOccurred) {
            Q_EMIT signalChanged(this);
        }
        if (d->articleChangeOccurred) {
            doArticleNotification();
        }
        d->nodeChangeOccurred = false;
        d->articleChangeOccurred = false;
    } else if (!doNotify && d->doNotify) { //turned off
        d->nodeChangeOccurred = false;
        d->articleChangeOccurred = false;
        d->doNotify = false;
    }
}

uint TreeNode::id() const
{
    return d->id;
}

void TreeNode::setId(uint id)
{
    d->id = id;
}

void TreeNode::nodeModified()
{
    if (d->doNotify) {
        Q_EMIT signalChanged(this);
    } else {
        d->nodeChangeOccurred = true;
    }
}

void TreeNode::articlesModified()
{
    if (d->doNotify) {
        doArticleNotification();
    } else {
        d->articleChangeOccurred = true;
    }
}

void TreeNode::doArticleNotification()
{
}

QPoint TreeNode::listViewScrollBarPositions() const
{
    return d->scrollBarPositions;
}

void TreeNode::setListViewScrollBarPositions(const QPoint &pos)
{
    d->scrollBarPositions = pos;
}

ArticleListJob *TreeNode::createListJob()
{
    return new ArticleListJob(this);
}
