#ifndef OPML_H
#define OPML_H

#include "subscriptionlist.h"
#include "aggregator.h"

class QFile;
class QApplication;
class QDomDocument;

namespace feedsync
{

class Opml: public Aggregator
{
    Q_OBJECT

 public:
    Opml(const KConfigGroup& configgroup);
    ~Opml();
    SubscriptionList * getSubscriptionList() const;
    void load();
    void add(SubscriptionList * list);
    void update(SubscriptionList * list);
    void remove(SubscriptionList * list);

 private:
    SubscriptionList * _subscriptionList;
    QFile * _xmlFile;
    QDomDocument * _xmlDoc;
    bool _loaded;

 public Q_SLOTS:
    void sendSignalLoadDone();

 signals:
    void addDone();
    void updateDone();
    void removeDone();
    void loadDone();
};

}

#endif