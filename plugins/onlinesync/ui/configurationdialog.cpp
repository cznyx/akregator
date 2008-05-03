#include "ui_configurationdialog.h"
#include "configurationdialog.h"
#include "configurationdialogadd.h"

#include <kdebug.h>
#include <QTimer>

namespace feedsync
{

ConfigurationDialog::ConfigurationDialog( QWidget *parent)
{
    kDebug();

    // UI setup
    QWidget *widget = new QWidget( this );
    ui = new Ui::ConfigurationDialog();
    ui->setupUi(widget);
    setMainWidget( widget );

    setCaption( i18n("Online reader") );

    // Read config file
    refresh();

    // Init
    ui->list_readerList->setColumnCount(2);
    QStringList deleteTags;
        deleteTags.append( i18n("Nothing") );
        deleteTags.append( i18n("Category") );
        deleteTags.append( i18n("Feed") );
        deleteTags.append( i18n("Ask") );
    ui->cb_deleteFeeds->addItems(deleteTags);
    QStringList title;
        title.append( i18n("Type") );
        title.append( i18n("Description") );
    ui->list_readerList->setHeaderLabels(title);

    // Read config
    KConfig config("akregator_feedsyncrc");
    KConfigGroup generalGroup( &config, "FeedSyncConfig" );
    if (ui->cb_deleteFeeds->findText( generalGroup.readEntry( "RemovalPolicy", QString() ) ) < 0) {
        ui->cb_deleteFeeds->setCurrentIndex( 0 );
    } else {
        ui->cb_deleteFeeds->setCurrentIndex( ui->cb_deleteFeeds->findText( generalGroup.readEntry( "RemovalPolicy", QString() ) ) );
    }

    // Slots
    connect( ui->b_add, SIGNAL( clicked() ), this, SLOT( slotButtonAddClicked() ) );
    connect( ui->b_update, SIGNAL( clicked() ), this, SLOT( slotButtonUpdateClicked() ) );
    connect( ui->b_remove, SIGNAL( clicked() ), this, SLOT( slotButtonRemoveClicked() ) );
    connect( this, SIGNAL( finished() ), this, SLOT( slotFinished() ) );

    QTimer::singleShot( 0, this, SLOT(slotDelayedInit()) );
}

ConfigurationDialog::~ConfigurationDialog()
{
    kDebug();
    delete ui;
}

void ConfigurationDialog::refresh()
{
    kDebug();

    // Clear
    ui->list_readerList->clear();

    // Read configuration
    KConfig config("akregator_feedsyncrc");
    QList<QTreeWidgetItem *> items;
    foreach ( const QString& groupname, config.groupList() ) {
        if (groupname.left(15)=="FeedSyncSource_") {
            kDebug() << groupname;
            KConfigGroup generalGroup( &config, groupname );
            QStringList line;
            line.append( generalGroup.readEntry( "AggregatorType", QString() ) );
            line.append( generalGroup.readEntry( "Identifier", QString() ) );
            line.append( groupname );
            items.append( new QTreeWidgetItem((QTreeWidget*)0,line) );
        }
        ui->list_readerList->insertTopLevelItems(0, items);
    }
}

// SLOT

void ConfigurationDialog::slotFinished()
{
    kDebug();
    deleteLater();
}

void ConfigurationDialog::slotButtonUpdateClicked()
{
    kDebug();
    QList<QTreeWidgetItem *> m_items = ui->list_readerList->selectedItems();
    if (m_items.count()>0) {
        kDebug() << m_items.at(0)->text(2);
        KConfig config("akregator_feedsyncrc");
        KConfigGroup configgroup( &config, m_items.at(0)->text(2) );
        ConfigurationDialogAdd * m_dlg = new ConfigurationDialogAdd();
        m_dlg->show();
        m_dlg->load( configgroup );
        connect( m_dlg, SIGNAL( finished() ), this, SLOT( refresh() ) );
    }
}

void ConfigurationDialog::slotButtonAddClicked()
{
    kDebug();
    ConfigurationDialogAdd * addDlg = new ConfigurationDialogAdd();
    addDlg->show();
    connect( addDlg, SIGNAL( finished() ), this, SLOT( refresh() ) );
}

void ConfigurationDialog::slotButtonRemoveClicked()
{
    // kDebug();

    QList<QTreeWidgetItem *> m_items = ui->list_readerList->selectedItems();
    if (m_items.count()>0) {
        kDebug() << m_items.at(0)->text(2);
        KConfig config("akregator_feedsyncrc");
        config.deleteGroup(m_items.at(0)->text(2));
    }

    refresh();
}

void ConfigurationDialog::slotDelayedInit()
{
    kDebug();
}

void ConfigurationDialog::slotButtonClicked(int button) {
    if (button == KDialog::Ok) {
        kDebug();
        // Save the removal policy
        KConfig config("akregator_feedsyncrc");
        KConfigGroup generalGroup( &config, "FeedSyncConfig" );
        generalGroup.writeEntry( "RemovalPolicy", ui->cb_deleteFeeds->itemText( ui->cb_deleteFeeds->currentIndex() ) );
        generalGroup.config()->sync();
        accept();

    } else {
        KDialog::slotButtonClicked(button);
    }
}

}