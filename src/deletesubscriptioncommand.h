/*
    This file is part of Akregator.

    Copyright (C) 2008 Frank Osterfeld <osterfeld@kde.org>

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

#ifndef AKREGATOR_DELETESUBSCRIPTIONCOMMAND_H
#define AKREGATOR_DELETESUBSCRIPTIONCOMMAND_H

#include "command.h"

#include <boost/weak_ptr.hpp>

namespace Akregator
{

class FeedList;

class DeleteSubscriptionCommand : public Command
{
    Q_OBJECT
public:
    explicit DeleteSubscriptionCommand(QObject *parent = Q_NULLPTR);
    ~DeleteSubscriptionCommand();

    void setSubscription(const boost::weak_ptr<FeedList> &feedList, int subId);

    int subscriptionId() const;
    boost::weak_ptr<FeedList> feedList() const;

private:
    void doStart();
    void doAbort();

private:
    class Private;
    Private *const d;
    Q_PRIVATE_SLOT(d, void startDelete())
    Q_PRIVATE_SLOT(d, void jobFinished())
};

}

#endif // AKREGATOR_DELETESUBSCRIPTIONCOMMAND_H
