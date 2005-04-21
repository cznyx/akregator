/*
    This file is part of Akregator.

    Copyright (C) 2004 Stanislav Karchebny <Stanislav.Karchebny@kdemail.net>

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
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/
#include "articlesequence.h"
#include "feedgroup.h"
#include "fetchtransaction.h"

#include <qlistview.h>
#include <qdom.h>
#include <qptrvector.h>

#include <kdebug.h>

namespace Akregator {

class FeedGroup::FeedGroupPrivate
{
    public:
        /** List of children */
        QPtrList<TreeNode> children;
        /** caching unread count of children */
        int unread;
        /** whether or not the folder is expanded */
        bool open;
        /** update unread count cache */
};
           
FeedGroup* FeedGroup::fromOPML(QDomElement e)
{
    FeedGroup* fg = new FeedGroup(e.hasAttribute("text") ? e.attribute("text") : e.attribute("title"));
    fg->setOpen( e.attribute("isOpen") == "true" ? true : false );
    fg->setId( e.attribute("id").toUInt() );
    return fg;
}

FeedGroup::FeedGroup(const QString& title) : TreeNode(), d(new FeedGroupPrivate)
{
    d->unread = 0;
    setTitle(title);
} 

FeedGroup::~FeedGroup()
{
    // FIXME: this is a workaround, since iterating with first and next doesn't work together with delete. We won't need that when using QValueList<TreeNode*> instead of QPtrList)
    
    QPtrVector<TreeNode> vec(d->children.count());
    int j = 0;
    for (TreeNode* i = d->children.first(); i; i = d->children.next() )
    {
        vec.insert(j, i);
        j++;
    }

    for (uint i = 0; i < vec.count(); ++i)
       delete vec[i];

    // tell the world that this node is destroyed
    emit signalDestroyed(this);
    delete d;
    d = 0;
}

ArticleSequence FeedGroup::articles()
{
    ArticleSequence seq;
    for (TreeNode* i = d->children.first(); i; i = d->children.next() )
        seq += i->articles();
     
    return seq;
}

QDomElement FeedGroup::toOPML( QDomElement parent, QDomDocument document ) const
{
    QDomElement el = document.createElement( "outline" );
    el.setAttribute( "text", title() );
    parent.appendChild( el );
    el.setAttribute("isOpen", d->open ? "true" : "false");
    el.setAttribute( "id", QString::number(id()) );
    // necessary because of const
    QPtrList<TreeNode> children = d->children;
    
    for (TreeNode* i = children.first(); i; i = children.next() )
        el.appendChild( i->toOPML(el, document) );
        
    return el;
}

QPtrList<TreeNode> FeedGroup::children()
{
    return d->children;
}

void FeedGroup::insertChild(TreeNode* node, TreeNode* after)
{
    int pos = d->children.find(after);
    
    if (pos == -1)
        prependChild(node);
    else 
        insertChild(pos+1, node);
}

void FeedGroup::insertChild(uint index, TreeNode* node)
{
//    kdDebug() << "enter FeedGroup::insertChild(int, node) " << node->title() << endl;
    if (node)
    {    
        d->children.insert(index, node);
        node->setParent(this);
        connect(node, SIGNAL(signalChanged(TreeNode*)), this, SLOT(slotChildChanged(TreeNode*)));
        connect(node, SIGNAL(signalDestroyed(TreeNode*)), this, SLOT(slotChildDestroyed(TreeNode*)));
        updateUnreadCount();    
        emit signalChildAdded(node);
        modified();
    }   
//    kdDebug() << "leave FeedGroup::insertChild(int, node) " << node->title() << endl; 
}

void FeedGroup::appendChild(TreeNode* node)
{
//    kdDebug() << "enter FeedGroup::appendChild() " << node->title() << endl;
    if (node)
    {
        d->children.append(node);
        node->setParent(this);
        connect(node, SIGNAL(signalChanged(TreeNode*)), this, SLOT(slotChildChanged(TreeNode*)));
        connect(node, SIGNAL(signalDestroyed(TreeNode*)), this, SLOT(slotChildDestroyed(TreeNode*)));
        updateUnreadCount();    
        emit signalChildAdded(node);
        modified();
    }    
//    kdDebug() << "leave FeedGroup::appendChild() " << node->title() << endl;
}

void FeedGroup::prependChild(TreeNode* node)
{
//    kdDebug() << "enter FeedGroup::prependChild() " << node->title() << endl;
    if (node)
    {
        d->children.prepend(node);
        node->setParent(this);
        connect(node, SIGNAL(signalChanged(TreeNode*)), this, SLOT(slotChildChanged(TreeNode*)));
        connect(node, SIGNAL(signalDestroyed(TreeNode*)), this, SLOT(slotChildDestroyed(TreeNode*)));
        updateUnreadCount();    
        emit signalChildAdded(node);
        modified();
    }    
//    kdDebug() << "leave FeedGroup::prependChild() " << node->title() << endl;
}

void FeedGroup::removeChild(TreeNode* node)
{
//    kdDebug() << "enter FeedGroup::removeChild() node:" << (node ? node->title() : "null") << endl;
    if (node && d->children.contains(node))
    {    
        node->setParent(0);
        d->children.remove(node);
        updateUnreadCount();    
        emit signalChildRemoved(this, node);
        modified();
    }
//    kdDebug() << "leave FeedGroup::removeChild() node: " << (node ? node->title() : "null") << endl;
}


TreeNode* FeedGroup::firstChild()
{
    return d->children.first();
}            

TreeNode* FeedGroup::lastChild()
{
    return d->children.last();
}
            
bool FeedGroup::isOpen() const
{
    return d->open;
}

void FeedGroup::setOpen(bool open)
{
    d->open = open;
}
            
int FeedGroup::unread() const
{
    return d->unread;
}

int FeedGroup::totalCount() const
{
    int totalCount = 0;
  
    QPtrList<TreeNode> children = d->children;
    
    for (TreeNode* i = children.first(); i; i = children.next() )
        totalCount += i->totalCount();
    
    return totalCount;
}

void FeedGroup::updateUnreadCount()
{
    int unread = 0;
  
    QPtrList<TreeNode> children = d->children;
    
    for (TreeNode* i = children.first(); i; i = children.next() )
        unread += i->unread();
    
    d->unread = unread;
}

void FeedGroup::slotMarkAllArticlesAsRead() 
{
    setNotificationMode(false);
    for (TreeNode* i = d->children.first(); i; i = d->children.next() )
        i->slotMarkAllArticlesAsRead();
    setNotificationMode(true, true);
}
    
void FeedGroup::slotChildChanged(TreeNode* /*node*/)
{
//    kdDebug() << "enter FeedGroup::slotChildChanged child" << node->title() << endl;
    int oldUnread = d->unread;
    updateUnreadCount();    
   
    if (oldUnread != d->unread)
        modified();

//    kdDebug() << "leave FeedGroup::slotChildChanged child"  << node->title() << endl;
}

void FeedGroup::slotChildDestroyed(TreeNode* node)
{
    d->children.remove(node);
    updateUnreadCount();    
    modified();
}

void FeedGroup::slotDeleteExpiredArticles()
{
    setNotificationMode(false);
    for (TreeNode* i = d->children.first(); i; i = d->children.next() )
        i->slotDeleteExpiredArticles();
    setNotificationMode(true, true);
}

void FeedGroup::slotAddToFetchQueue(FetchQueue* queue)
{
    for (TreeNode* i = d->children.first(); i; i = d->children.next() )
        i->slotAddToFetchQueue(queue);
}

TreeNode* FeedGroup::next()
{
    if ( firstChild() )
        return firstChild();

    if ( nextSibling() )
        return nextSibling();
    
    FeedGroup* p = parent();
    while (p)
    {
        if ( p->nextSibling() )
            return p->nextSibling();
        else
            p = p->parent();
    }
    return 0;
}

} // namespace Akregator
#include "feedgroup.moc"
