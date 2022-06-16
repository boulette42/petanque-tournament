/********************************************************************************
** Form generated from reading UI file 'show_result_dlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOW_RESULT_DLG_H
#define UI_SHOW_RESULT_DLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ShowPlayerResultDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTreeView *tvMatches;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pbOk;

    void setupUi(QDialog *ShowPlayerResultDialog)
    {
        if (ShowPlayerResultDialog->objectName().isEmpty())
            ShowPlayerResultDialog->setObjectName(QString::fromUtf8("ShowPlayerResultDialog"));
        ShowPlayerResultDialog->resize(380, 264);
        verticalLayout = new QVBoxLayout(ShowPlayerResultDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tvMatches = new QTreeView(ShowPlayerResultDialog);
        tvMatches->setObjectName(QString::fromUtf8("tvMatches"));
        tvMatches->setRootIsDecorated(false);
        tvMatches->header()->setVisible(false);

        verticalLayout->addWidget(tvMatches);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pbOk = new QPushButton(ShowPlayerResultDialog);
        pbOk->setObjectName(QString::fromUtf8("pbOk"));

        horizontalLayout->addWidget(pbOk);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ShowPlayerResultDialog);
        QObject::connect(pbOk, SIGNAL(pressed()), ShowPlayerResultDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ShowPlayerResultDialog);
    } // setupUi

    void retranslateUi(QDialog *ShowPlayerResultDialog)
    {
        ShowPlayerResultDialog->setWindowTitle(QCoreApplication::translate("ShowPlayerResultDialog", "Spieler", nullptr));
        pbOk->setText(QCoreApplication::translate("ShowPlayerResultDialog", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ShowPlayerResultDialog: public Ui_ShowPlayerResultDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOW_RESULT_DLG_H
