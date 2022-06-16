/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionPlayerLoad;
    QAction *actionPlayerSave;
    QAction *actionQuit;
    QAction *actionPlayerPrint;
    QAction *actionRoundCreate;
    QAction *actionRoundFinish;
    QAction *actionBackup;
    QAction *actionPlayerAdd;
    QAction *actionRoundPrint;
    QAction *actionSettings;
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_0;
    QTabWidget *tabWidget;
    QWidget *tabPlayer;
    QVBoxLayout *verticalLayout_1;
    QTreeView *tvPlayerList;
    QWidget *tabSites;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *tlSiteCnt;
    QLineEdit *leSiteCnt;
    QPushButton *pbSetSiteCnt;
    QSpacerItem *horizontalSpacer_2;
    QTreeView *tvSiteList;
    QWidget *tabRound;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *tlRound;
    QComboBox *cmbRound;
    QSpacerItem *horizontalSpacer;
    QTreeView *tvMatchList;
    QWidget *tabPlayerResult;
    QVBoxLayout *verticalLayout_4;
    QTreeView *tvPlayerResultList;
    QWidget *tabTeamResult;
    QVBoxLayout *verticalLayout_5;
    QTreeView *tvTeamResultList;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuPlayer;
    QMenu *menuRound;
    QMenu *menuHilfe;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(620, 600);
        actionPlayerLoad = new QAction(MainWindow);
        actionPlayerLoad->setObjectName(QString::fromUtf8("actionPlayerLoad"));
        actionPlayerSave = new QAction(MainWindow);
        actionPlayerSave->setObjectName(QString::fromUtf8("actionPlayerSave"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionPlayerPrint = new QAction(MainWindow);
        actionPlayerPrint->setObjectName(QString::fromUtf8("actionPlayerPrint"));
        actionRoundCreate = new QAction(MainWindow);
        actionRoundCreate->setObjectName(QString::fromUtf8("actionRoundCreate"));
        actionRoundFinish = new QAction(MainWindow);
        actionRoundFinish->setObjectName(QString::fromUtf8("actionRoundFinish"));
        actionBackup = new QAction(MainWindow);
        actionBackup->setObjectName(QString::fromUtf8("actionBackup"));
        actionPlayerAdd = new QAction(MainWindow);
        actionPlayerAdd->setObjectName(QString::fromUtf8("actionPlayerAdd"));
        actionRoundPrint = new QAction(MainWindow);
        actionRoundPrint->setObjectName(QString::fromUtf8("actionRoundPrint"));
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_0 = new QVBoxLayout(centralwidget);
        verticalLayout_0->setObjectName(QString::fromUtf8("verticalLayout_0"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QFont font;
        font.setPointSize(9);
        tabWidget->setFont(font);
        tabPlayer = new QWidget();
        tabPlayer->setObjectName(QString::fromUtf8("tabPlayer"));
        verticalLayout_1 = new QVBoxLayout(tabPlayer);
        verticalLayout_1->setObjectName(QString::fromUtf8("verticalLayout_1"));
        tvPlayerList = new QTreeView(tabPlayer);
        tvPlayerList->setObjectName(QString::fromUtf8("tvPlayerList"));
        tvPlayerList->setRootIsDecorated(false);
        tvPlayerList->setSortingEnabled(true);
        tvPlayerList->setExpandsOnDoubleClick(false);
        tvPlayerList->header()->setProperty("showSortIndicator", QVariant(true));

        verticalLayout_1->addWidget(tvPlayerList);

        tabWidget->addTab(tabPlayer, QString());
        tabSites = new QWidget();
        tabSites->setObjectName(QString::fromUtf8("tabSites"));
        verticalLayout = new QVBoxLayout(tabSites);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        tlSiteCnt = new QLabel(tabSites);
        tlSiteCnt->setObjectName(QString::fromUtf8("tlSiteCnt"));

        horizontalLayout_2->addWidget(tlSiteCnt);

        leSiteCnt = new QLineEdit(tabSites);
        leSiteCnt->setObjectName(QString::fromUtf8("leSiteCnt"));

        horizontalLayout_2->addWidget(leSiteCnt);

        pbSetSiteCnt = new QPushButton(tabSites);
        pbSetSiteCnt->setObjectName(QString::fromUtf8("pbSetSiteCnt"));

        horizontalLayout_2->addWidget(pbSetSiteCnt);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_2);

        tvSiteList = new QTreeView(tabSites);
        tvSiteList->setObjectName(QString::fromUtf8("tvSiteList"));
        tvSiteList->setRootIsDecorated(false);
        tvSiteList->header()->setVisible(false);

        verticalLayout->addWidget(tvSiteList);

        tabWidget->addTab(tabSites, QString());
        tabRound = new QWidget();
        tabRound->setObjectName(QString::fromUtf8("tabRound"));
        verticalLayout_3 = new QVBoxLayout(tabRound);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tlRound = new QLabel(tabRound);
        tlRound->setObjectName(QString::fromUtf8("tlRound"));

        horizontalLayout->addWidget(tlRound);

        cmbRound = new QComboBox(tabRound);
        cmbRound->setObjectName(QString::fromUtf8("cmbRound"));

        horizontalLayout->addWidget(cmbRound);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout);

        tvMatchList = new QTreeView(tabRound);
        tvMatchList->setObjectName(QString::fromUtf8("tvMatchList"));
        tvMatchList->setRootIsDecorated(false);
        tvMatchList->header()->setVisible(false);

        verticalLayout_3->addWidget(tvMatchList);

        tabWidget->addTab(tabRound, QString());
        tabPlayerResult = new QWidget();
        tabPlayerResult->setObjectName(QString::fromUtf8("tabPlayerResult"));
        verticalLayout_4 = new QVBoxLayout(tabPlayerResult);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tvPlayerResultList = new QTreeView(tabPlayerResult);
        tvPlayerResultList->setObjectName(QString::fromUtf8("tvPlayerResultList"));
        tvPlayerResultList->setRootIsDecorated(false);

        verticalLayout_4->addWidget(tvPlayerResultList);

        tabWidget->addTab(tabPlayerResult, QString());
        tabTeamResult = new QWidget();
        tabTeamResult->setObjectName(QString::fromUtf8("tabTeamResult"));
        verticalLayout_5 = new QVBoxLayout(tabTeamResult);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tvTeamResultList = new QTreeView(tabTeamResult);
        tvTeamResultList->setObjectName(QString::fromUtf8("tvTeamResultList"));
        tvTeamResultList->setRootIsDecorated(false);

        verticalLayout_5->addWidget(tvTeamResultList);

        tabWidget->addTab(tabTeamResult, QString());

        verticalLayout_0->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 620, 26));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuPlayer = new QMenu(menubar);
        menuPlayer->setObjectName(QString::fromUtf8("menuPlayer"));
        menuRound = new QMenu(menubar);
        menuRound->setObjectName(QString::fromUtf8("menuRound"));
        menuHilfe = new QMenu(menubar);
        menuHilfe->setObjectName(QString::fromUtf8("menuHilfe"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuPlayer->menuAction());
        menubar->addAction(menuRound->menuAction());
        menubar->addAction(menuHilfe->menuAction());
        menuFile->addAction(actionBackup);
        menuFile->addAction(actionSettings);
        menuFile->addAction(actionQuit);
        menuPlayer->addAction(actionPlayerAdd);
        menuPlayer->addSeparator();
        menuPlayer->addAction(actionPlayerLoad);
        menuPlayer->addAction(actionPlayerSave);
        menuRound->addAction(actionRoundCreate);
        menuRound->addAction(actionRoundFinish);
        menuHilfe->addAction(actionAbout);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Petanque Turnier", nullptr));
        actionPlayerLoad->setText(QCoreApplication::translate("MainWindow", "Spieler-Liste &laden", nullptr));
        actionPlayerSave->setText(QCoreApplication::translate("MainWindow", "Spieler-Liste &speichern", nullptr));
        actionQuit->setText(QCoreApplication::translate("MainWindow", "&Ende", nullptr));
        actionPlayerPrint->setText(QCoreApplication::translate("MainWindow", "Liste drucken", nullptr));
        actionRoundCreate->setText(QCoreApplication::translate("MainWindow", "Runde &erzeugen", nullptr));
        actionRoundFinish->setText(QCoreApplication::translate("MainWindow", "Runde &beenden", nullptr));
        actionBackup->setText(QCoreApplication::translate("MainWindow", "&Sicherung erstellen", nullptr));
        actionPlayerAdd->setText(QCoreApplication::translate("MainWindow", "Spieler &hinzuf\303\274gen", nullptr));
        actionRoundPrint->setText(QCoreApplication::translate("MainWindow", "Runde drucken", nullptr));
        actionSettings->setText(QCoreApplication::translate("MainWindow", "Einstellungen...", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "\303\234ber...", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabPlayer), QCoreApplication::translate("MainWindow", "Spieler-Liste", nullptr));
        tlSiteCnt->setText(QCoreApplication::translate("MainWindow", "Anzahl", nullptr));
        pbSetSiteCnt->setText(QCoreApplication::translate("MainWindow", "&Anzahl \303\274bernehmen", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabSites), QCoreApplication::translate("MainWindow", "Pl\303\244tze", nullptr));
        tlRound->setText(QCoreApplication::translate("MainWindow", "Runde", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabRound), QCoreApplication::translate("MainWindow", "Paarungen", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabPlayerResult), QCoreApplication::translate("MainWindow", "Ergebnisse", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabTeamResult), QCoreApplication::translate("MainWindow", "Ergebnisse", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "&Datei", nullptr));
        menuPlayer->setTitle(QCoreApplication::translate("MainWindow", "&Spieler", nullptr));
        menuRound->setTitle(QCoreApplication::translate("MainWindow", "&Runde", nullptr));
        menuHilfe->setTitle(QCoreApplication::translate("MainWindow", "Hilfe", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
